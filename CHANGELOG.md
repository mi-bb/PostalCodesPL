Copyright (C) 2016-2026 Michal Babik

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.


# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.2.0] - 2026-09-12

### Added
- AGENTS.md: Added guidance file for AI coding assistants
- .gitignore: Added to exclude __pycache__/
- codes1, codes2: Wrappers now detect Python interpreter (python3 or python),
  with an error message if neither is found

### Changed
- codes1.py, codes2.py, pcodespl.py, postc.c: Bumped version to 1.2.0
- codes1.py, codes2.py, pcodespl.py, postc.c: Updated author email address
- pcodespl.py: Refactored b_dbop with type hints and clearer parameter name;
  guarded rollback/close in sql_command_exec against failed connection
- codes1.py, codes2.py: Code style cleanup (is not None comparisons,
  multi-line statements, removed trailing semicolons)
- README.md: Fixed heading hierarchy and grammar in English and Polish sections
- postc.c: Updated the gcc command in the header comment to the src/ paths

### Removed
- codes1.py, codes2.py: Removed unused sqlite3 import
- pcodespl.py: Removed redundant UTF-8 coding declaration (Python 3 default)
- pc_base.db: Dropped the unused, empty post_codes1 table (denormalized
  leftover; 0 rows, referenced by no code)

## [2026-07-23]

### Added
- postc.c: Added error handling for failed query preparation
- postc.c: Added missing buffer initialization in get_show_info_by_city_street
- tests/: Added Unity-based unit test suite for postc.c
- CLAUDE.md: Added guidance file for AI coding assistants

### Fixed
- postc.c: Replaced sprintf with bounded snprintf when filling result rows
- postc.c: Fixed result loops to stop on query error instead of looping forever
- postc.c: Fixed null checks in get_post_code_info/get_city_names_like/
  get_city_voivodeship_info that tested the wrong pointer
- postc.c: Fixed place-count format specifier and column width padding

### Changed
- postc.c: Added explicit return 0 in main
- codes1.py: Removed redundant UTF-8 coding declaration (Python 3 default)
- codes2.py: Removed redundant UTF-8 coding declaration (Python 3 default)

## [2019-02-22]

### Added
- postc.c: File added to package
- utf8.c: File added to package
- utf8.h: File added to package

### Changed
- codes1.py: Changed language to English
- codes2.py: Changed language to English

## [2019-01-28]

### Changed
- pcodespl.py: Changed name from pyKodyPocztowe to PostalCodesPL
- codes1: Changed Python execute command from python to python3
- codes2: Changed Python execute command from python to python3
- codes1.py: Changed deprecated pygtk functions to operating ones
- codes2.py: Changed deprecated pygtk functions to operating ones
