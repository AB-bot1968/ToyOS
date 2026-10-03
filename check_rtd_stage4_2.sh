#!/bin/sh
set -eu

test -f src/rt_priority.h
test -f tools/test_rt_priority_selection.c

grep -q 'rt_priority_compare(t->rt_priority,sched_tasks\[best\].rt_priority)' src/kernel.c
grep -Rq 'Stage 4.2: fixed-priority arbitration is now active' changes/RTD
grep -q 'rt_deadline_before(t->rt_deadline_tick,sched_tasks\[best\].rt_deadline_tick)' src/kernel.c
grep -q 'i<(uint32_t)best' src/kernel.c

cc=${CC:-gcc}
tmpdir=${TMPDIR:-/tmp}
tmp="$tmpdir/toyos_rt_priority_selection_$$"
trap 'rm -rf "$tmp"' EXIT HUP INT TERM
mkdir -p "$tmp"
$cc -std=c99 -Wall -Wextra -Werror tools/test_rt_priority.c -o "$tmp/test_rt_priority"
"$tmp/test_rt_priority"
$cc -std=c99 -Wall -Wextra -Werror tools/test_rt_priority_selection.c -o "$tmp/test_rt_priority_selection"
"$tmp/test_rt_priority_selection"

echo 'RTD Stage 4.2 priority arbitration checks passed'
