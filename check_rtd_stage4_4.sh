#!/bin/sh
set -eu

test -f src/rt_sensor_diag.c
test -f tools/test_rt_priority_preemption.c

grep -q '#define SYS_RT_TRACE[[:space:]]*41u' src/kernel.c
grep -q 'rt_dispatch_sequence' src/kernel.c
grep -q 'static void rt_record_dispatch' src/kernel.c
grep -q 'rt_dispatch_seq' src/kernel.c
grep -q 'case SYS_RT_TRACE' src/kernel.c
grep -Fq 'sched_tasks[next].rt_dispatch_seq=rt_dispatch_sequence' src/kernel.c
grep -q 'rt_record_dispatch(rid)' src/kernel.c
grep -q 'SYS_RT_TRACE[[:space:]]*41u' src/rt_sensor_diag.c
grep -q 'PRIORITY-PROBE' src/rt_sensor_diag.c
grep -q 'hold_ticks' src/rt_sensor_diag.c
grep -q 'target_hold=(priority>=7u)?2u:1u' src/rt_sensor_diag.c
grep -q 'timer_wait_change' src/rt_sensor_diag.c
grep -q 'SENSOR1' src/rt_sensor_diag.c
grep -q 'SENSOR2' src/rt_sensor_diag.c

grep -q 'build/SENSOR1.EXE=SENSOR1.EXE' build.sh
grep -q 'build/SENSOR2.EXE=SENSOR2.EXE' build.sh

grep -q 'rt_pick_ready' src/kernel.c

grep -q 'rt_priority_compare' src/kernel.c

grep -q 'SENSOR1.EXE 1000 1000 3' README.md
grep -q 'SENSOR2.EXE 1000 1000 7' README.md

cc=${CC:-gcc}
tmpdir=${TMPDIR:-/tmp}
tmp="$tmpdir/toyos_rt_stage44_$$"
trap 'rm -rf "$tmp"' EXIT HUP INT TERM
mkdir -p "$tmp"
$cc -std=c99 -Wall -Wextra -Werror tools/test_rt_priority_preemption.c -o "$tmp/test_preemption"
"$tmp/test_preemption"

$cc -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib -c src/kernel.c -o "$tmp/kernel.o"
$cc -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib -c src/user_shell.c -o "$tmp/user_shell.o"
$cc -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib -c src/rt_sensor_diag.c -o "$tmp/rt_sensor_diag.o"

echo 'RTD Stage 4.4 priority-preemption checks passed'
