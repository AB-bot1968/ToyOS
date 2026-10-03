#!/bin/sh
set -eu
CC="${CC:-gcc}"
TARGET="$($CC -dumpmachine 2>/dev/null || true)"
case "$TARGET" in i386-*|i686-*) ;; *) echo "ERROR: нужен 32-bit i386/i686 W64DevKit; найден $TARGET" >&2; exit 1;; esac
CFLAGS='-m32 -ffreestanding -fno-builtin -fno-stack-protector -fno-pic -fno-pie -O2 -Wall -Wextra'
$CC $CFLAGS -fsyntax-only src/netdrv.c
$CC $CFLAGS -c src/netdrv.c -o build_netdrv_check1.o
$CC $CFLAGS -fno-omit-frame-pointer -c src/netdrv.c -o build_netdrv_check2.o
$CC $CFLAGS -c src/netdrv.c -o build_netdrv_check3.o
rm -f build_netdrv_check1.o build_netdrv_check2.o build_netdrv_check3.o
python3 - <<'PY'
from pathlib import Path
n=Path('src/netdrv.c').read_text()
k=Path('src/kernel.c').read_text()
for x in ['SYS_TIMER_GET','SYS_FILE_OPEN','SYS_FILE_READ','SYS_FILE_WRITE','SYS_FILE_CLOSE','SYS_EXIT','SYS_PORT_OUT8','SYS_PORT_IN8']:
    assert x in n and x in k, x
for x in ['case SYS_TIMER_GET:','case SYS_FILE_OPEN:','case SYS_FILE_READ:','case SYS_FILE_WRITE:','case SYS_FILE_CLOSE:','case SYS_PORT_OUT8:','case SYS_PORT_IN8:']:
    assert x in k, x
assert 'be16(tcp+0)!=t->dport||be16(tcp+2)!=t->sport' in n
assert 'be16(tcp)!=t->sport||be16(tcp+2)!=t->dport' not in n
assert 'put_be16(ip+2,total)' in n
assert 'put_be16(ip+2,20+total)' not in n
assert 'PORT=8080' in Path('src/NET.CFG').read_text()
print('PASS1')
print('PASS2')
print('PASS3')
print('SYSCALL_ABI_CHECK_OK')
print('TCP_PORT_DIRECTION_CHECK_OK')
PY
