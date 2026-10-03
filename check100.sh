#!/bin/sh
set -eu
printf '[check100] PNG2RAW has no external zlib dependency... '
test -s tools/png2raw.c
test -s tools/PNG2RAW.BAT
test -s tools/PNG2RAW_RU.md
! grep -F '#include <zlib.h>' tools/png2raw.c >/dev/null
! grep -E '(^|[^[:alnum:]_])-lz([^[:alnum:]_]|$)' tools/png2raw.c >/dev/null
! grep -F 'zlib.h' tools/png2raw.c >/dev/null
! grep -F -- '-lz' build.sh >/dev/null
grep -F 'inflate_zlib' tools/png2raw.c >/dev/null
grep -F 'crc32_update' tools/png2raw.c >/dev/null
grep -F 'adler32_update' tools/png2raw.c >/dev/null
grep -F 'dynamic Huffman' tools/PNG2RAW_RU.md >/dev/null
echo PASS
