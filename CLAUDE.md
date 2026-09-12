# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

PostalCodesPL is a small package distributing an SQLite database (`pc_base.db`) of Polish postal
codes (cities, districts, streets, street numbers, voivodeships), sourced from
http://www.kody-pocztowe.biz. The rest of the repo is a set of example client programs, in Python
and C, showing how to query that database. There is no build system or package manifest — these
are standalone example scripts; `postc.c` has a Unity-based unit test suite (see below), but the
Python examples have none.

## Programs

- `pcodespl.py` — shared Python data-access layer. All SQLite access for the Python examples goes
  through this module (`sql_get_post_code_info`, `sql_get_city_voivodeship`, `sql_get_info`,
  `sql_get_city_names_like`, plus the low-level `sql_command_get`/`sql_command_save`/
  `sql_command_exec` helpers). It opens `pc_base.db` in the current working directory by default.
- `codes1.py` (run via `./codes1`) — GTK+ 3 app: enter a postal code (XX-XXX), see matching
  places/streets. Imports `sql_get_post_code_info` from `pcodespl.py`.
- `codes2.py` (run via `./codes2`) — GTK+ 3 app: pick a city, voivodeship, and optional street
  filter from comboboxes to look up postal codes. Imports `sql_get_city_names_like`,
  `sql_get_city_voivodeship`, `sql_get_info` from `pcodespl.py`.
- `postc.c` / `utf8.c` / `utf8.h` — a C, terminal-based equivalent of the two Python tools in one
  program (menu-driven: look up by postal code, or by city name). `utf8.c`/`utf8.h` (from Jeff
  Bezanson's public-domain `utf8.c`) provide UTF-8-aware string length/iteration used to align the
  table output, since the data (Polish city/street names) is UTF-8.

The three programs are independent entry points into the same database/schema; there's no shared
C/Python code beyond the DB schema itself.

## Database schema (`pc_base.db`)

Reference for the tables joined by every query in `pcodespl.py` and `postc.c` (documented in full
in the header comment of `pcodespl.py`):

- `post_codes` — central fact table: `code1` (2-digit prefix), `code2` (3-digit suffix),
  `city_id`, `city_det_id`, `street_id`, `street_no_id`, `post_un`, `voivod_id`.
- `city` (`id`, `city_name`), `city_det` (`id`, `city_detcr` — district/detail description),
  `street` (`id`, `street_name`), `street_no` (`id`, `street_number`), `voivod` (`id`,
  `voivodeship`) — all joined back to `post_codes` by id.

A full postal code is `code1-code2` (e.g. `00-001`). Every "get full info" query joins all five
lookup tables against `post_codes`.

## Running the examples

```sh
# Python (GTK+ 3, Python 3, and PyGObject required)
./codes1        # or: python3 -O codes1.py
./codes2        # or: python3 -O codes2.py

# C (GCC + libsqlite3-dev required)
gcc -std=c11 -Wall postc.c utf8.c -lsqlite3 -o postc
./postc
```

The `codes1`/`codes2` wrapper scripts `cd` to their own directory before running, so they work
from any cwd; running `python3 -O codes1.py` / `codes2.py` directly, or `./postc`, requires the
cwd to contain `pc_base.db` (opened by a relative path), i.e. the repo root, unless a different
`bn` is passed to the Python `b_dbop`/`sql_command_*` helpers.

There is no linter config or CI in this repo, and the Python examples have no automated tests.

## Tests (`postc.c` only)

```sh
./tests/run_tests.sh
```

`tests/test_postc.c` uses the [Unity](https://github.com/ThrowTheSwitch/Unity) C test framework,
vendored (MIT-licensed, unmodified) under `tests/unity/` — there's no package manager to pull it
in otherwise. Every function under test in `postc.c` (`clear_pdata`, `clear_cdata`, `set_codes`,
`get_pd_column_width`, `get_post_code_info`, `get_city_names_like`, `get_city_voivodeship_info`)
is `static`, and `postc.c` defines its own `main`, so the test file pulls the whole source in
directly — `#define main postc_main_unused` then `#include "../postc.c"` — instead of linking
against it normally. **Do not** add a header/`static` split to `postc.c` to "properly" expose
these for testing; the include trick is the intended way to reach `static` internals here without
changing production code's visibility.

The `get_post_code_info`/`get_city_names_like`/`get_city_voivodeship_info` tests query the real,
committed `pc_base.db` (no mocking of `sqlite3_*` calls) and assert against specific rows/counts
(e.g. postal code `01-001` → Warszawa/Wola; city `Kraków` → id `14381`). Run the suite from the
repo root, same constraint as `postc` itself. If `pc_base.db` is ever regenerated with different
data, these expected values will need updating to match.

## Conventions to follow when editing

- Python files are UTF-8 (`# -*- coding: UTF-8 -*-`), target Python 3, and keep the
  `#---...---#` banner-style section dividers used throughout `pcodespl.py`/`codes1.py`/`codes2.py`.
  DB access from new Python code should go through `pcodespl.py` rather than opening `sqlite3`
  connections directly in the GTK files.
  C code follows GNU-style formatting (return type on its own line, space before parens,
  Doxygen-style `/** @brief ... */` comments) and compiles clean with `-std=c11 -Wall`.
- Licensing: GPLv3 (see `COPYING`); keep the existing GPL header/copyright block when modifying or
  adding files. Exception: `tests/unity/` is vendored third-party code (MIT, see
  `tests/unity/LICENSE.txt`) — leave its files as-is, don't add GPL headers to them.
