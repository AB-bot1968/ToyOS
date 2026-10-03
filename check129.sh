#!/bin/sh
set -eu
ROOT="`dirname "$0"`"
cd "$ROOT"

for f in src/comdrv.c src/netdrv.c src/loader.c src/netdrv.c src/vgadrv.c; do
    test -s "$f"
done

grep -q 'upper_copy(port_norm,port' src/comdrv.c
grep -q 'upper_copy(dir_norm,dir' src/comdrv.c
grep -q 'upper_copy(dir_norm,dir' src/loader.c
grep -q 'upper_copy(mode_norm,a0' src/netdrv.c
grep -q 'upper_copy(arg1_norm,a1' src/netdrv.c
grep -q 'file_norm' src/vgadrv.c

CFLAGS="-Os -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-builtin -fno-tree-vectorize -fno-tree-slp-vectorize -mno-sse -mno-sse2 -mno-mmx -mno-80387 -nostdinc -nostdlib"
for f in comdrv loader netdrv vgadrv; do
    gcc $CFLAGS -c "src/$f.c" -o "/tmp/toyos_v67_3_$f.o"
done

echo "CHECK129 PASS: driver textual parameters are normalized case-insensitively"
