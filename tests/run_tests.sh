#!/bin/sh
# Build and run the postc.c unit tests (Unity framework, vendored in tests/unity).
#
# Must run from the repository root so the DB-backed tests can open
# pc_base.db by its relative path, same as postc itself.
set -e

PDIR=$(dirname "$0")
cd "${PDIR}/.."

gcc -std=c11 -Wall -Wno-unused-function \
    tests/test_postc.c utf8.c tests/unity/unity.c \
    -lsqlite3 -o tests/test_postc

exec ./tests/test_postc
