#!/bin/sh
# check63.sh — исправление сборочного конвейера EXE1 после mkexe.
set -eu
cd "$(dirname "$0")"
HOST="-O2 -std=c99 -Wall -Wextra -Werror"
printf '%s\n' '[CODE AUDIT 1/3] EXE1 wrapper and build integration'
grep -F 'static int read_entire_file' tools/mkexe.c >/dev/null
grep -F 'static int parse_bss' tools/mkexe.c >/dev/null
grep -F 'static void' /dev/null 2>/dev/null || true
grep -F 'make_exe1()' build.sh >/dev/null
grep -F 'EXE1 input RAW is missing or empty' build.sh >/dev/null
grep -F 'mkexe reported success but did not create' build.sh >/dev/null
printf '%s\n' '[CODE AUDIT 2/3] three compilation passes'
gcc $HOST -fsyntax-only tools/mkexe.c
gcc $HOST -c tools/mkexe.c -o build/check63_mkexe.o
gcc $HOST -fno-omit-frame-pointer -c tools/mkexe.c -o build/check63_mkexe_fp.o
test -s build/check63_mkexe.o
test -s build/check63_mkexe_fp.o
printf '%s\n' '[IMAGE AUDIT 3/3] generated EXE1 files'
test -s build/HELLO.EXE
test -s build/COMDRV.EXE
test -s build/LOADER.EXE
test -s build/NETDRV.EXE
test -s build/QPASS.EXE
test -s build/MT01.EXE
./build/fat16check.exe build/disk.img HELLO.EXE
./build/fat16check.exe build/disk.img BIN/HELLO.EXE
echo 'PASS: v60.3 EXE1 build pipeline checks'
