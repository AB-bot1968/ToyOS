#!/bin/sh
set -eu
fail(){ echo "FAIL: $1"; exit 1; }
grep -q '#define SYS_RT_YIELD[[:space:]]*49u' src/kernel.c || fail 'kernel SYS_RT_YIELD ABI missing'
grep -q '#define SYS_RT_YIELD[[:space:]]*49u' src/user_shell.c || fail 'shell SYS_RT_YIELD ABI missing'
grep -q 'sys_rt_yield();}return;' src/user_shell.c || fail 'rtstat watch does not yield'
grep -q 'case SYS_RT_YIELD' src/kernel.c || fail 'kernel SYS_RT_YIELD handler missing'
grep -q 'sti;hlt;cli' src/kernel.c || fail 'yield idle path missing timer sleep'
grep -q 'RTSTAT WATCH tick=' src/user_shell.c || fail 'watch tick output missing'
grep -q 'RTD: started ' src/user_shell.c || fail 'F10 success message missing'
grep -q 'SYS_RT_WAIT' src/rt_sensor.c || fail 'sensor RT wait missing'
if grep -q 'SYS_CONSOLE_POLL' src/rt_sensor.c; then fail 'sensor still polls console'; fi
CFLAGS='-Os -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-builtin -fno-tree-vectorize -fno-tree-slp-vectorize -mno-sse -mno-sse2 -mno-mmx -mno-80387 -nostdinc -nostdlib -m32'
mkdir -p build/check_fix7
gcc $CFLAGS -c src/kernel.c -o build/check_fix7/kernel.o
gcc $CFLAGS -c src/user_shell.c -o build/check_fix7/user_shell.o
gcc $CFLAGS -c src/rtd.c -o build/check_fix7/rtd.o
gcc $CFLAGS -c src/rt_sensor.c -o build/check_fix7/rt_sensor.o
echo 'PASS: RT WATCH starvation fix, F10 reporting, and sensor isolation checks'
