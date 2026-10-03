#!/bin/sh
set -eu
printf '[check99] GCC PNG2RAW sources and wrapper... '
test -s tools/png2raw.c
test -s tools/PNG2RAW.BAT
test -s tools/PNG2RAW_RU.md
! grep -F '#include <zlib.h>' tools/png2raw.c >/dev/null
! grep -F -- '-lz' tools/png2raw.c >/dev/null
grep -F '64000' tools/png2raw.c >/dev/null
grep -F '320' tools/png2raw.c >/dev/null
grep -F '200' tools/png2raw.c >/dev/null
grep -F 'PNG2RAW.EXE' tools/PNG2RAW.BAT >/dev/null
echo PASS
