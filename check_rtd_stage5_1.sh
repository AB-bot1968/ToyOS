#!/bin/sh
set -eu

test -f src/rt_deadline.h
test -f src/rt_deadline_diag.c
test -f tools/test_rt_deadline.c

grep -Eq '#define SYS_RT_DEADLINE_INFO[[:space:]]+42u' src/kernel.c
grep -q 'rt_job_release_tick' src/kernel.c
grep -q 'rt_deadline_ms_to_ticks' src/kernel.c
grep -q 'rt_deadline_reached' src/kernel.c
grep -q 'rt_deadline_absolute' src/kernel.c
grep -q 'rt_begin_job' src/kernel.c
grep -Eq 'rt_job_release_tick=timer_ticks|rt_begin_job\(&sched_tasks\[id\],timer_ticks\)|rt_begin_job\(&sched_tasks\[id\],rt_time_ticks\)' src/kernel.c
grep -q 'case SYS_RT_DEADLINE_INFO' src/kernel.c
grep -q 'sched_tasks\[rid\].rt_job_release_tick' src/kernel.c

grep -q 'rt_deadline_reached(now,t->rt_deadline_tick)' src/kernel.c
grep -q 'rt_deadline_absolute(t->rt_next_release_tick' src/kernel.c
grep -q 'build/DEADLINE.EXE=DEADLINE.EXE' build.sh
grep -q 'make_exe1 build/rt_deadline_diag.raw build/DEADLINE.EXE 0' build.sh
grep -q 'src/rt_deadline_diag.c' build.sh
grep -q 'check_rtd_stage5_1.sh' build.sh

grep -q 'DEADLINE: started release=' src/rt_deadline_diag.c
grep -q 'DEADLINE: sample release=' src/rt_deadline_diag.c
grep -q 'SYS_RT_DEADLINE_INFO' src/rt_deadline_diag.c
grep -q 'DEADLINE: job completed before deadline' src/rt_deadline_diag.c

grep -q 'exec RTD.EXE DEADLINE.EXE 1000 100 3' README.md

grep -q 'changes/RTD/Stage5/Stage5_1' README.md

cc=${CC:-gcc}
tmpdir=${TMPDIR:-/tmp}
tmp="$tmpdir/toyos_rt_stage51_$$"
trap 'rm -rf "$tmp"' EXIT HUP INT TERM
mkdir -p "$tmp"
$cc -std=c99 -Wall -Wextra -Werror tools/test_rt_deadline.c -o "$tmp/test_rt_deadline"
"$tmp/test_rt_deadline"
$cc -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib -c src/kernel.c -o "$tmp/kernel.o"
$cc -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib -c src/user_shell.c -o "$tmp/user_shell.o"
$cc -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib -c src/rtd.c -o "$tmp/rtd.o"
$cc -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib -c src/rt_sensor.c -o "$tmp/rt_sensor.o"
$cc -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib -c src/rt_sensor_diag.c -o "$tmp/rt_sensor_diag.o"
$cc -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib -c src/rt_deadline_diag.c -o "$tmp/rt_deadline_diag.o"
echo 'RTD Stage 5.1 absolute deadline checks passed'
