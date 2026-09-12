# AGENTS.md

Small example-code repo around a committed SQLite database of Polish postal codes. No build system, tests, linter, or CI — verify changes by running the programs directly.

## Running

- `./codes1` / `./codes2` are POSIX sh wrappers that `cd` to the repo root, then run `python3 -O codes1.py` (or `codes2.py`). Run them from anywhere; run the `.py` files only from the repo root, because the database path is relative to CWD.
- `codes1.py` / `codes2.py` are GTK+ 3 GUI apps (`gi.require_version('Gtk', '3.0')`); they need a display and `python3-gi`. For headless verification, query the DB directly with `sqlite3` or import `pcodespl`.
- C example: `gcc -std=c11 -Wall postc.c utf8.c -lsqlite3 -o postc` (command is also in a comment at top of `postc.c`). Requires libsqlite3 headers (`libsqlite3-dev`).

## Database

- `pc_base.db` (~5 MB) is committed and is the main deliverable. Handle with care — it is not regenerable from anything in this repo.
- Schema is documented in the header comment of `pcodespl.py`: normalized tables `post_codes`, `city`, `city_det`, `street`, `street_no`, `voivod` joined by id columns.
- An undocumented denormalized table `post_codes1` also exists (same data as flat rows); don't rely on the schema comment alone — check with `.schema`.
- Postal codes are stored split into two TEXT columns, `code1` (2 chars) and `code2` (3 chars), e.g. `00` + `950`.

## Conventions

- `pcodespl.py` is the shared DB-access library; both GUI scripts import query helpers from it. New DB queries belong there, not in the GUI files.
- Data originated from http://www.kody-pocztowe.biz (site is defunct; do not try to fetch from it).
- Source files carry a large ASCII-art banner and GPL-3 header comments — keep the style when editing existing files.
- Changes are logged in `CHANGELOG` (old-style GNU ChangeLog format with dates and per-file entries).
