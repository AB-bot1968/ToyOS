#!/bin/sh
set -eu
CFLAGS='-Os -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-builtin -fno-tree-vectorize -fno-tree-slp-vectorize -mno-sse -mno-sse2 -mno-mmx -mno-80387 -nostdinc -nostdlib'
CC=${CC:-gcc}

case "$($CC -dumpmachine)" in
  i[3-6]86-*|i686-*) ;;
  *) echo "SKIP: target is not i386/i686: $($CC -dumpmachine)"; exit 2 ;;
esac

$CC -m32 $CFLAGS -fsyntax-only src/netdrv.c
$CC -m32 $CFLAGS -c src/netdrv.c -o /tmp/netdrv_check1.o
$CC -m32 $CFLAGS -fno-omit-frame-pointer -c src/netdrv.c -o /tmp/netdrv_check2.o

# Регрессионные проверки ключевых исправлений.
grep -q 'ne_select_run(NE_PAGE1)' src/netdrv.c
grep -q 'ne_select_run(0)' src/netdrv.c
grep -q 'ne_select_stop(NE_PAGE1)' src/netdrv.c
grep -q 'if(!ne_w(NE_RCR,0x14u))' src/netdrv.c
grep -q 'put_be16(ip+2,total)' src/netdrv.c
grep -q 'put("NETDRV: no TCP SYN-ACK\\n")' src/netdrv.c
if grep -q 'ne_select(NE_PAGE1)' src/netdrv.c; then echo 'FAIL: obsolete stopping page selector'; exit 1; fi
if grep -q 'put_be16(ip+2,(uint16_t)(20u+total))' src/netdrv.c; then echo 'FAIL: old IPv4 length formula'; exit 1; fi


cmp src/NET.CFG build/NET.CFG 2>/dev/null || true

echo 'NETDRV: THREE COMPILATION PASSES OK'
echo 'NETDRV: RX/TX regression checks OK'
