# AGENTS.md

Small repo around a committed SQLite database of Polish postal codes (`pc_base.db`) plus example client programs in Python (GTK+ 3) and C. No build system, linter, or CI — verify changes by running the programs and the test suite directly.

## Layout

- All source lives in `examples/` (moved there in v1.3.0): `pcodespl.py` (shared Python DB-access library), `codes1.py` / `codes2.py` (GTK+ 3 GUI apps), `postc.c` + `utf8.c` / `utf8.h` (terminal C example).
- `tests/` — Unity-based unit test suite for `postc.c`; `tests/unity/` is vendored third-party (MIT) — leave as-is, no GPL headers there.
- `pc_base.db` (~5 MB) sits at the repo root and is the main deliverable. Handle with care — it is not regenerable from anything in this repo.

## Running & verifying

- `./codes1` / `./codes2` are bash wrappers: they `cd` to the repo root, detect `python3` or `python`, then run `python -O examples/codesX.py`. They work from any cwd; run the `.py` files directly only from the repo root (`PYTHONPATH=examples python3 -O examples/codes1.py`), because the DB is opened by path relative to CWD.
- GUI apps need a display and `python3-gi`. For headless verification, query the DB with `sqlite3` or import the library: `PYTHONPATH=examples python3 -c "from pcodespl import sql_get_post_code_info; print(sql_get_post_code_info('00','950'))"`.
- C example (from repo root): `gcc -std=c11 -Wall examples/postc.c examples/utf8.c -lsqlite3 -o postc` — requires libsqlite3 headers (`libsqlite3-dev`). The same command is repeated in a comment near the top of `examples/postc.c`; keep the two in sync.
- Tests: `tests/run_tests.sh` (compiles and runs them; cds to the repo root itself, so it works from any cwd). The DB-backed tests assert known rows from `pc_base.db` (e.g. code `01-001` → Warszawa/Wola); if the DB data ever changes, expected values in `tests/test_postc.c` need updating.
- `tests/test_postc.c` reaches `static` functions by `#define main postc_main_unused` + `#include "../examples/postc.c"`. Do not split `postc.c` into headers/modules to make it "properly" testable — the include trick is intended.

## Database

- Schema is documented in the header comment of `examples/pcodespl.py`: normalized tables `post_codes`, `city`, `city_det`, `street`, `street_no`, `voivod` joined by id columns. (An unused, empty denormalized `post_codes1` table was dropped in 1.2.0.)
- Postal codes are stored split into two TEXT columns: `code1` (2 chars) and `code2` (3 chars), e.g. `00` + `950`.

## Conventions

- New DB queries belong in `examples/pcodespl.py`, not in the GUI files.
- Source files use short `SPDX-License-Identifier: GPL-3.0-or-later` headers (the old full GPL blocks were removed); `pcodespl.py` and `postc.c` keep the ASCII-art banner. Keep the `#---...---#` section dividers. C code is GNU-style formatted (return type on own line, Doxygen `/** @brief */`) and must compile clean with `-std=c11 -Wall`.
- Changes are logged in `CHANGELOG.md` (Keep a Changelog format; version + date headings like `## [1.2.0] - 2026-09-12`). Commit messages use Conventional Commits style (`type(scope): description`).
- Data originated from http://www.kody-pocztowe.biz (site is defunct; do not try to fetch from it).
- `CLAUDE.md` has extended guidance (full schema reference, per-program descriptions) — consult it for details beyond this file.
