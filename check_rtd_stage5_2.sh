#!/bin/sh
set -eu

# Stage 5.2: a missed absolute deadline becomes a soft/best-effort job. Any
# on-time READY job must outrank every missed READY job; among missed jobs the
# existing priority/deadline/slot ordering remains deterministic.
test -f src/kernel.c
test -f src/rt_deadline_miss_diag.c
test -f tools/test_rt_deadline_miss.c

grep -q 'static int rt_pick_ready_class' src/kernel.c
grep -q 'static int rt_pick_ready_missed' src/kernel.c
grep -q 'if((t->rt_job_missed?1u:0u)!=missed_class)continue;' src/kernel.c
grep -q 'int missed_next=rt_pick_ready_missed' src/kernel.c
grep -q 'job missed -> best-effort' src/rt_deadline_miss_diag.c
grep -q 'completed with deadline miss' src/rt_deadline_miss_diag.c
grep -q 'build/RTDMISS.EXE=RTDMISS.EXE' build.sh

grep -q 'sh ./check_rtd_stage5_2.sh' build.sh

cc=${CC:-gcc}
tmpdir=${TMPDIR:-/tmp}
tmp="$tmpdir/toyos_rt_stage52_$$"
trap 'rm -rf "$tmp"' EXIT HUP INT TERM
mkdir -p "$tmp"
$cc -std=c99 -Wall -Wextra -Werror tools/test_rt_deadline_miss.c -o "$tmp/test_rt_deadline_miss"
"$tmp/test_rt_deadline_miss"
$cc -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib -c src/kernel.c -o "$tmp/kernel.o"
$cc -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib -c src/rt_deadline_miss_diag.c -o "$tmp/rt_deadline_miss_diag.o"

echo 'RTD Stage 5.2 deadline-miss best-effort checks passed'
