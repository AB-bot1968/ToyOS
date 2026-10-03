#!/bin/sh
set -eu
fail(){ echo "FAIL: $1"; exit 1; }
mkdir -p build/check25
HOST_CFLAGS="-O2 -std=c99 -Wall -Wextra -Werror"
for src in tools/mkfat16.c tools/fat16check.c tools/mkexe.c; do
    gcc $HOST_CFLAGS -fsyntax-only "$src" || fail "syntax: $src"
done
gcc $HOST_CFLAGS tools/mkfat16.c -o build/check25/mkfat16.exe
gcc $HOST_CFLAGS tools/fat16check.c -o build/check25/fat16check.exe
gcc $HOST_CFLAGS tools/mkexe.c -o build/check25/mkexe.exe
[ -f build/check25/mkfat16.exe ] || fail mkfat16.exe
[ -f build/check25/fat16check.exe ] || fail fat16check.exe
[ -f build/check25/mkexe.exe ] || fail mkexe.exe
rm -rf build/check25
echo "PASS: v25 host utilities (mkfat16, fat16check, mkexe)"
