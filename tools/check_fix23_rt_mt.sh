#!/bin/sh
set -eu
K="$(dirname "$0")/../src/kernel.c"
grep -q 'static int mt_current_running(void)' "$K"
grep -q 'else if(mt_current_running())sched_exit(f)' "$K"
grep -q 'if(exe_active&&cr3==sched_saved_shell_cr3)return;' "$K"
grep -q 'sched_active&&rt_return_cr3==sched_saved_shell_cr3' "$K"
grep -q 'rt_tasks\[i\]\.rt_period_ms!=0u)active++' "$K"
grep -q '#define RT_MAX_TASKS      8u' "$K"
grep -q '#define SCHED_TASKS      25u' "$K"
grep -q '#define MT_QUANTUM_TICKS  2u' "$K"
echo 'PASS: FIX23 RT/MT integration static checks'
