#!/bin/sh
set -eu
gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -fno-builtin \
  -fno-tree-vectorize -fno-tree-slp-vectorize -mno-sse -mno-sse2 -mno-mmx \
  -mno-80387 -nostdinc -nostdlib -fsyntax-only src/netdrv.c
grep -q '#define SYS_FILE_SIZE    17u' src/netdrv.c
grep -q 'SYS_FILE_WRITE failed' src/netdrv.c
grep -q 'RECV size mismatch' src/netdrv.c
grep -q 'file_size_sys' src/netdrv.c
grep -q 'header_done' src/netdrv.c
echo NETDRV_V58_7_CHECK_OK
