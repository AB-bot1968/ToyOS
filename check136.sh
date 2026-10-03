#!/bin/sh
set -eu
fail(){ echo "CHECK136 FAIL: $1" >&2; exit 1; }
test -f src/kernel.c || fail 'kernel source missing'
test -f src/user_shell.c || fail 'shell source missing'
test -f src/rtd.c || fail 'RTD source missing'
test -f src/rt_sensor_diag.c || fail 'sensor diagnostic source missing'
grep -q '#define SYS_RT_STATS[[:space:]]*48u' src/kernel.c || fail 'SYS_RT_STATS 48 missing'
grep -q 'case SYS_RT_STATS' src/kernel.c || fail 'kernel syscall case missing'
grep -q 'rt_stats_record' src/kernel.c || fail 'aggregate recorder missing'
grep -q 'ring\[8u\]\[8u\]' src/kernel.c || fail '8-entry RT ring missing'
grep -q 'rtstat compare' src/user_shell.c || fail 'rtstat compare missing'
grep -q 'rtstat watch' src/user_shell.c || fail 'rtstat watch missing'
grep -q 'rtstat dump' src/user_shell.c || fail 'rtstat dump missing'
grep -q 'rtstat reset' src/user_shell.c || fail 'rtstat reset missing'
grep -q 'VERBOSE' src/rtd.c || fail 'RTD VERBOSE missing'
grep -q 'SYS_RT_STATS 48u' src/rt_sensor_diag.c || fail 'sensor monitor syscall missing'
grep -q 'RT_SENSOR_DIAG_ID=4' build.sh || fail 'SENSOR4 build missing'
cc=${CC:-gcc}
CFLAGS='-Os -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-builtin -fno-tree-vectorize -fno-tree-slp-vectorize -mno-sse -mno-sse2 -mno-mmx -mno-80387 -nostdinc -nostdlib -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast'
$cc $CFLAGS -Werror -fsyntax-only src/kernel.c
$cc $CFLAGS -Werror -fsyntax-only src/user_shell.c
$cc $CFLAGS -Werror -fsyntax-only src/rtd.c
for id in 1 2 3 4; do $cc $CFLAGS -Werror -DRT_SENSOR_DIAG_ID=$id -fsyntax-only src/rt_sensor_diag.c; done
echo 'CHECK136 PASS: v67.10 RT monitor source audit and syntax checks'
