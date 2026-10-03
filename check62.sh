#!/bin/sh
# check62.sh — v60.2: аудит исправления FAT16 ROOT и состава системного образа.
# Проверка выполняется тремя независимыми уровнями: исходный код, компиляция,
# затем фактическая проверка уже собранного FAT16-образа.
set -eu
cd "$(dirname "$0")"
HOST="-O2 -std=c99 -Wall -Wextra -Werror"

printf '%s\n' '[CODE AUDIT 1/3] root directory implementation'
grep -F 'static int put_root_entry' tools/mkfat16.c >/dev/null
grep -F 'for(idx=0;idx<ROOT_ENTRIES;idx++)' tools/mkfat16.c >/dev/null
grep -F 'DATA_CLUSTERS' tools/mkfat16.c >/dev/null
grep -F 'MAX_CLUSTER' tools/mkfat16.c >/dev/null
grep -F 'ROOT DIRECTORY is full' tools/mkfat16.c >/dev/null
grep -F 'build/boot.bin=BOOT.BIN' build.sh >/dev/null

printf '%s\n' '[CODE AUDIT 2/3] compilation'
gcc $HOST -fsyntax-only tools/mkfat16.c
gcc $HOST -c tools/mkfat16.c -o build/check62_mkfat16.o
gcc $HOST -fno-omit-frame-pointer -c tools/mkfat16.c -o build/check62_mkfat16_fp.o
test -s build/check62_mkfat16.o
test -s build/check62_mkfat16_fp.o

printf '%s\n' '[IMAGE AUDIT 3/3] shipped root entries'
./build/fat16check.exe build/disk.img BOOT.BIN
./build/fat16check.exe build/disk.img HELLO.EXE
./build/fat16check.exe build/disk.img COMDRV.EXE
./build/fat16check.exe build/disk.img LOADER.EXE
./build/fat16check.exe build/disk.img NETDRV.EXE
./build/fat16check.exe build/disk.img NET.CFG
./build/fat16check.exe build/disk.img QPASS.EXE
./build/fat16check.exe build/disk.img QSIZE.EXE
./build/fat16check.exe build/disk.img BIN/HELLO.EXE
./build/fat16check.exe build/disk.img BIN/NETDRV.EXE
./build/fat16check.exe build/disk.img DOC/NET.CFG

echo 'PASS: v60.2 root directory and system image checks'
