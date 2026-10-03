#!/bin/sh
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }

test -f src/rt_sensor_diag.c || fail 'sensor diagnostic source missing'
grep -q '#define SYS_RT_EXEC_INFO   47u' src/kernel.c || fail 'extended RT execution-info syscall missing'
grep -q 'rt_job_cpu_ticks' src/kernel.c || fail 'per-job CPU tick accounting missing'
grep -q 'case SYS_RT_EXEC_INFO' src/kernel.c || fail 'extended RT execution-info handler missing'
for id in 1 2 3 4; do
    grep -q "RT_SENSOR_DIAG_ID=$id" build.sh || fail "sensor$id build missing"
done
grep -q 'SENSOR3.EXE' build.sh || fail 'SENSOR3 image install missing'
grep -q 'SENSOR4.EXE' build.sh || fail 'SENSOR4 image install missing'
grep -q 'SYS_RT_EXEC_INFO' src/rt_sensor_diag.c || fail 'sensors do not use extended timing data'
grep -q 'jitter=' src/rt_sensor_diag.c || fail 'sensor jitter field missing'
grep -q 'cpu=' src/rt_sensor_diag.c || fail 'sensor CPU time field missing'
grep -q 'wall=' src/rt_sensor_diag.c || fail 'sensor wall span field missing'
grep -q 'deadline=' src/rt_sensor_diag.c || fail 'sensor deadline field missing'
grep -q 'SENSOR4.EXE' build.sh || fail 'SENSOR4 not packaged'
# Permanent command-input invariant remains covered by the v67.8 shell parser.
grep -q 'ascii_fold' src/user_shell.c || fail 'case-insensitive parser missing'
grep -q 'if(eq(line,"help"))' src/user_shell.c || fail 'command equality parser regression'
grep -q 'if(prefix(line,"exec RTD.EXE "))' src/user_shell.c || fail 'RTD command parser regression'

tmpdir=${TMPDIR:-/tmp}
tmp="$tmpdir/toyos_v67_9_$$"
trap 'rm -rf "$tmp"' EXIT HUP INT TERM
mkdir -p "$tmp"
cc=${CC:-gcc}
CFLAGS='-m32 -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-builtin -fno-tree-vectorize -fno-tree-slp-vectorize -mno-sse -mno-sse2 -mno-mmx -mno-80387 -nostdinc -nostdlib'
$cc $CFLAGS -Werror -c src/kernel.c -o "$tmp/kernel.o"
for id in 1 2 3 4; do $cc $CFLAGS -DRT_SENSOR_DIAG_ID=$id -Werror -c src/rt_sensor_diag.c -o "$tmp/sensor$id.o"; done
$cc $CFLAGS -Werror -c src/rtd.c -o "$tmp/rtd.o"
$cc $CFLAGS -Werror -c src/rt_jitter_diag.c -o "$tmp/jitter.o"
$cc -std=c99 -Wall -Wextra -Werror tools/test_rt_jitter.c -o "$tmp/test_rt_jitter"
"$tmp/test_rt_jitter"

echo 'CHECK135 PASS: v67.9 four-sensor RT timing observability and extended CPU accounting verified'
