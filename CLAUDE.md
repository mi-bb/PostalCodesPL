# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

PostalCodesPL distributes an SQLite database (`pc_base.db`) of Polish postal codes — cities,
districts, streets, street numbers, voivodeships — sourced from http://www.kody-pocztowe.biz (site
is defunct; do not try to fetch from it). Everything else in the repo is example client code, in
Python and C, showing how to query that database.

There is no package manifest, linter config, or CI — just a root `Makefile` for the C example and
its tests (Python has no build step). `pc_base.db` (~5 MB, committed, 123752 rows in `post_codes`)
is the actual deliverable and is **not regenerable** from anything in this repo — treat it as
read-only unless explicitly asked otherwise.

`AGENTS.md` covers the same ground more briefly; keep the two in sync when either changes.

## Layout

All source lives in `examples/` (moved there in v1.3.0 — older docs/comments may still show
`src/` or root-level paths):

- `examples/pcodespl.py` — shared Python data-access layer. Every SQLite call from the Python
  examples goes through it: `sql_get_post_code_info`, `sql_get_city_voivodeship`, `sql_get_info`,
  `sql_get_city_names_like`, over the low-level `sql_command_get` / `sql_command_save` /
  `sql_command_exec` helpers.
- `examples/codes1.py` (run via `./codes1`) — GTK+ 3 app: enter a postal code (XX-XXX), see
  matching places/streets.
- `examples/codes2.py` (run via `./codes2`) — GTK+ 3 app: pick city + voivodeship + optional
  street filter to look up postal codes.
- `examples/postc.c` + `examples/utf8.c` / `examples/utf8.h` — terminal C program covering both
  Python tools in one menu-driven binary. `utf8.c` (Jeff Bezanson's public-domain `utf8.c`)
  supplies UTF-8-aware length/iteration used to align table output, since Polish city/street
  names are UTF-8 — column widths must count codepoints, not bytes.
- `tests/` — Unity test suite for `postc.c`; `tests/unity/` is vendored third-party MIT code.
- `other/` — README screenshots.
- `Makefile` (repo root) — builds `postc` (`make`) and the test binary (`make test`); see
  "Running the examples" and "Tests" below.

The three programs are independent entry points onto the same schema; there is no shared C/Python
code beyond the database itself.

## Database schema (`pc_base.db`)

Documented in full in the header comment of `examples/pcodespl.py`:

- `post_codes` — central fact table: `code1` (2-char TEXT prefix), `code2` (3-char TEXT suffix),
  `city_id`, `city_det_id`, `street_id`, `street_no_id`, `post_un`, `voivod_id`.
- `city` (`id`, `city_name`), `city_det` (`id`, `city_detcr` — district/detail), `street` (`id`,
  `street_name`), `street_no` (`id`, `street_number`), `voivod` (`id`, `voivodeship`) — all joined
  back to `post_codes` by id.

A full postal code is `code1-code2` (e.g. `00-001`), stored split across the two columns — never as
one string. Every "full info" query inner-joins all five lookup tables against `post_codes`.

## Running the examples

```sh
# Python (needs Python 3, GTK+ 3, PyGObject / python3-gi, and a display)
./codes1
./codes2

# C (needs GCC + libsqlite3-dev), from the repo root
make
./postc
```

`codes1` / `codes2` are bash wrappers: they `cd` to the repo root, pick `python3` or `python`, then
`exec` `python -O examples/codesX.py`. They work from any cwd.

**Everything opens `pc_base.db` by a path relative to the cwd, so run from the repo root.** In
Python the path is overridable — `DATABASE_FILE_NAME` / the `db_file_name` argument to `db_open`,
or the `bn` argument threaded through the `sql_command_*` helpers. In C it is not: `"pc_base.db"`
is hardcoded at three separate `sqlite3_open` sites in `examples/postc.c`.

For headless verification without a display, import the library instead of launching a GUI:

```sh
PYTHONPATH=examples python3 -c "from pcodespl import sql_get_post_code_info; print(sql_get_post_code_info('01','001'))"
# (True, [('Warszawa', 'Wola', 'Jana Pawła Ii Al.', 'numery od 41 do 43a nieparzyste', '', 'mazowieckie')])
```

`PYTHONPATH=examples` is only needed for this direct-import form; running `examples/codes1.py`
as a script puts `examples/` on `sys.path` automatically.

## Tests (`postc.c` only — the Python examples have none)

```sh
make test        # builds and runs tests/test_postc; run from the repo root
```

13 tests, all passing as of v1.2.0. `make test` builds `tests/test_postc.c` +
`examples/utf8.c` + `tests/unity/unity.c` with `-std=c11 -Wall -Wno-unused-function`
(see the `Makefile` at the repo root).

Two things about this suite are deliberate and easy to "fix" wrongly:

- **The include trick.** Nearly every function under test in `examples/postc.c` — `clear_pdata`,
  `clear_cdata`, `get_pd_column_width`, `get_post_code_info`, `get_city_names_like`,
  `get_city_voivodeship_info` (all `static`; only `set_codes` is not) — is invisible to a normal
  link, and `postc.c` defines its own `main`. The test file therefore pulls the whole source in
  (`#define main postc_main_unused`, then `#include "../examples/postc.c"`) instead of linking
  against it normally. **Do not** split `postc.c` into a header/module to make it "properly"
  testable; the include trick is the intended way to reach these internals without widening
  production visibility.
- **The DB-backed tests hit the real committed `pc_base.db`** — no mocking of `sqlite3_*` — and
  assert specific rows and counts (postal code `01-001` → Warszawa/Wola; city `Kraków` → id
  `14381`). If the database is ever regenerated with different data, those expected values in
  `tests/test_postc.c` must be updated to match.

Unity's `main` here is `main (void)` with a fixed list of `RUN_TEST` calls and no filtering
argument, so there is no command-line way to run a single test — to isolate one, temporarily
comment out the other `RUN_TEST` lines at the bottom of `tests/test_postc.c`.

## Conventions to follow when editing

- **Python:** UTF-8, Python 3, type hints on new function signatures. Keep the `#---...---#`
  banner-style section dividers used throughout `pcodespl.py` / `codes1.py` / `codes2.py`. New DB
  queries belong in `examples/pcodespl.py`, never as direct `sqlite3` connections in the GTK
  files.
- **The `(status, data)` contract:** every `sql_*` helper in `pcodespl.py` returns a
  `tuple[bool, list | str]` — success flag plus rows, or the error string on failure. SQL errors
  are caught, printed, and rolled back rather than raised, so callers must unpack both values and
  check the flag; they never see an exception.
- **C:** GNU-style formatting (return type on its own line, space before parens, Doxygen
  `/** @brief ... */` comments). Must compile clean under `-std=c11 -Wall`. The `get_*` functions
  allocate their `**_data` out-parameter and return the row count; the caller frees.
- **Licensing:** GPLv3 (`COPYING`). Source files carry a short
  `SPDX-License-Identifier: GPL-3.0-or-later` header — the old full GPL blocks were removed;
  `pcodespl.py` and `postc.c` additionally keep the ASCII-art banner. Exception: `tests/unity/` is
  vendored MIT third-party code (`tests/unity/LICENSE.txt`) — leave it as-is, never add GPL
  headers there.
- **Changelog & commits:** log notable changes in `CHANGELOG.md` (Keep a Changelog format, version
  + date headings like `## [1.2.0] - 2026-09-12`). Commit messages use Conventional Commits
  (`type(scope): description`).
