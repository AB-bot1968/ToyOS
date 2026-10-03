#!/bin/sh
set -eu

# Stage 5.2 FIX1: verify the actual soft-deadline selection policy used by the
# runtime path. An on-time READY job must beat any missed READY job regardless
# of priority; a missed job remains runnable as best-effort when no on-time job
# is available. Absolute-deadline tie breaking must be wrap-safe.

test -f src/kernel.c
test -f src/rt_deadline.h
test -f src/rt_deadline_miss_diag.c
test -f tools/test_rt_deadline_miss_integration.c

grep -q 'rt_deadline_before' src/kernel.c
grep -q 'static inline int rt_deadline_before' src/rt_deadline.h
grep -q 'RTDMISS: dispatch=' src/rt_deadline_miss_diag.c
grep -q 'RTDMISS: job missed -> best-effort' src/rt_deadline_miss_diag.c
grep -q 'Exact Stage 5.2 transition' tools/test_rt_deadline_miss_integration.c
grep -q 'When SENSOR1 blocks' tools/test_rt_deadline_miss_integration.c

grep -Eq '(^|[[:space:]])(sh[[:space:]]+)?\./check_rtd_stage5_2_fix1\.sh' build.sh

tmpdir=${TMPDIR:-/tmp}
tmp="$tmpdir/toyos_rt_stage52_fix1_$$"
trap 'rm -rf "$tmp"' EXIT HUP INT TERM
mkdir -p "$tmp"
cc=${CC:-gcc}
$cc -std=c99 -Wall -Wextra -Werror tools/test_rt_deadline_miss_integration.c -o "$tmp/test_rt_deadline_miss_integration"
"$tmp/test_rt_deadline_miss_integration"
$cc -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib -c src/kernel.c -o "$tmp/kernel.o"
$cc -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Os -Wall -Wextra -nostdinc -nostdlib -c src/rt_deadline_miss_diag.c -o "$tmp/rt_deadline_miss_diag.o"

echo 'RTD Stage 5.2 FIX1 deadline-miss policy checks passed'
