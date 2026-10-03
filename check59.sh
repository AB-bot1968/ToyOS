#!/bin/sh
# check59.sh — три независимых компиляционных прохода NETDRV.EXE.
set -eu

case "`gcc -dumpmachine`" in
    i[3-6]86-*|i686-*) ;;
    *) echo "ERROR: x86 W64DevKit required"; exit 1;;
esac

CFLAGS="-Os -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-builtin -fno-tree-vectorize -fno-tree-slp-vectorize -mno-sse -mno-sse2 -mno-mmx -mno-80387 -nostdinc -nostdlib"

echo "[NETDRV CHECK 1/3] syntax"
gcc $CFLAGS -fsyntax-only src/netdrv.c

echo "[NETDRV CHECK 2/3] object"
gcc $CFLAGS -c src/netdrv.c -o build/netdrv.check2.o
test -s build/netdrv.check2.o

echo "[NETDRV CHECK 3/3] object with explicit frame pointer"
gcc $CFLAGS -fno-omit-frame-pointer -c src/netdrv.c -o build/netdrv.check3.o
test -s build/netdrv.check3.o

test -s build/NETDRV.EXE
test -s build/NET.CFG

./build/fat16check.exe build/disk.img NETDRV.EXE
./build/fat16check.exe build/disk.img NET.CFG

echo "NETDRV: three compile passes OK"
