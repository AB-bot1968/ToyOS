#!/bin/sh
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }

test -f src/rt_deadline.h || fail 'rt deadline header missing'
test -f src/rt_timebase_diag.c || fail 'RTTIME diagnostic source missing'
test -f tools/test_rt_timebase.c || fail 'RT timebase host test missing'

grep -q '#define PIT_HZ 50u' src/kernel.c || fail 'legacy 50 Hz system constant missing'
grep -q '#define RT_TIME_HZ 100u' src/kernel.c || fail '100 Hz RT hardware timebase missing'
grep -q 'static volatile uint32_t rt_time_ticks;' src/kernel.c || fail 'RT time counter missing'
grep -q 'rt_time_phase' src/kernel.c || fail 'dual-rate phase divider missing'
grep -q 'rt_time_ticks++;' src/kernel.c || fail 'RT tick increment missing'
grep -q 'timer_ticks++;' src/kernel.c || fail 'legacy 50 Hz counter increment missing'
grep -q 'if(rt_background_active)sched_irq_tick(f);else if(rt_time_phase==0u)sched_irq_tick(f);' src/kernel.c || fail 'dual-rate scheduler dispatch missing'
grep -q 'case SYS_RT_TIME_GET:f->eax=rt_time_ticks;break;' src/kernel.c || fail 'RT time syscall missing'
grep -q '#define SYS_RT_TIME_GET    45u' src/kernel.c || fail 'RT time syscall id missing'

grep -q 'unsigned int q=ms/10u' src/rt_deadline.h || fail '10 ms RT conversion missing'
grep -q 'rt_next_release_tick=rt_time_ticks' src/kernel.c || fail 'RT release no longer uses RT clock'
grep -q 'rt_begin_job(&sched_tasks\[id\],rt_time_ticks)' src/kernel.c || fail 'RT job release no longer uses RT clock'
grep -q 't=&sched_tasks\[id\];now=rt_time_ticks' src/kernel.c || fail 'SYS_RT_WAIT no longer uses RT clock'
grep -q 'uint32_t now=rt_time_ticks' src/kernel.c || fail 'RT IRQ scheduler no longer uses RT clock'

grep -q 'SYS_RT_TIME_GET' src/rtd.c || fail 'RTD delay is not using RT clock'
grep -q 'delay_ms/10u' src/rtd.c || fail 'RTD delay conversion is not 10 ms based'
grep -q 'RTTIME: started ordinary EXE1' src/rt_timebase_diag.c || fail 'RTTIME diagnostic banner missing'
grep -q 'rt_delta=' src/rt_timebase_diag.c || fail 'RTTIME RT delta output missing'
grep -q 'system_delta=' src/rt_timebase_diag.c || fail 'RTTIME legacy system delta output missing'

grep -q 'RTTIME.EXE' build.sh || fail 'RTTIME EXE1 not in build'
grep -q 'check_rtd_stage6_1.sh' build.sh || fail 'Stage 6.1 regression not in build'
grep -q 'SYS_RT_TIME_GET=100 Hz / 10 ms RT clock' src/user_shell.c || fail 'shell RT clock documentation missing'

grep -q 'PIT(50Hz sys, 100Hz RT)' src/kernel.c || fail 'dual-rate banner missing'

grep -q 'Stage 6.1' README.md || fail 'README Stage 6.1 missing'
grep -q 'changes/RTD/Stage6/Stage6_1' README.md || fail 'README Stage 6.1 docs path missing'

grep -q 'Stage 6.1' changes/RTD/Stage6/Stage6_1/RELEASE_NOTES_RU.md || fail 'Stage 6.1 release notes missing'
grep -q '100 Hz' changes/RTD/Stage6/Stage6_1/TEST_PLAN_RU.md || fail 'Stage 6.1 test plan missing 100 Hz'

tmpdir=${TMPDIR:-/tmp}
tmp="$tmpdir/toyos_rt_stage61_$$"
trap 'rm -rf "$tmp"' EXIT HUP INT TERM
mkdir -p "$tmp"
cc=${CC:-gcc}
$cc -std=c99 -Wall -Wextra -Werror tools/test_rt_timebase.c -o "$tmp/test_rt_timebase"
"$tmp/test_rt_timebase"
$cc -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib -c src/kernel.c -o "$tmp/kernel.o"
$cc -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib -c src/user_shell.c -o "$tmp/user_shell.o"
$cc -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib -c src/rtd.c -o "$tmp/rtd.o"
$cc -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib -c src/rt_timebase_diag.c -o "$tmp/rt_timebase_diag.o"
$cc -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib -c src/rt_sensor.c -o "$tmp/rt_sensor.o"
$cc -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib -c src/rt_deadline_diag.c -o "$tmp/rt_deadline_diag.o"
$cc -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib -c src/rt_deadline_miss_diag.c -o "$tmp/rt_deadline_miss_diag.o"
$cc -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib -c src/rt_period_skip_diag.c -o "$tmp/rt_period_skip_diag.o"
echo 'RTD Stage 6.1 dual-rate timebase checks passed'
