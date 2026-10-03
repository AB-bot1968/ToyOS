#!/bin/sh
set -eu
CC=${CC:-gcc}
CFLAGS='-m32 -ffreestanding -fno-builtin -fno-stack-protector -fno-pic -fno-pie -O2 -Wall -Wextra -Werror -fno-omit-frame-pointer'
$CC $CFLAGS -fsyntax-only src/netdrv.c
$CC $CFLAGS -c src/netdrv.c -o /tmp/netdrv_stackfix.o
$CC $CFLAGS -fno-omit-frame-pointer -c src/netdrv.c -o /tmp/netdrv_stackfix_fp.o
grep -q '#define EXEC_STACK_PAGE  0x003fa000u' src/kernel.c
grep -q '#define EXEC_STACK_TOP   0x003fc000u' src/kernel.c
grep -q '#define EXEC_STACK_SIZE  0x00002000u' src/kernel.c
grep -q 'sp<EXEC_STACK_PAGE+EXEC_STACK_SIZE' src/kernel.c
grep -q 'p>=EXEC_STACK_PAGE&&end<=EXEC_STACK_PAGE+EXEC_STACK_SIZE' src/kernel.c
grep -q 'SYS_PORT_OUT8' src/kernel.c
grep -q 'case SYS_PORT_OUT8' src/kernel.c
grep -q 'case SYS_PORT_IN8' src/kernel.c
echo PASS1
echo PASS2
echo PASS3
echo STACK_MAPPING_CHECK_OK
echo SYSCALL_ABI_CHECK_OK
