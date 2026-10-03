#!/bin/sh
set -eu
K=src/kernel.c
S=src/user_shell.c
grep -q '#define SYS_MT_STATS       54u' "$K"
grep -q '#define SYS_MT_STATS        54u' "$S"
grep -q 'mt_stat_dispatch\[SCHED_TASKS\]' "$K"
grep -q 'mt_stat_quanta\[SCHED_TASKS\]' "$K"
grep -q 'mt_stat_cpu_ticks\[SCHED_TASKS\]' "$K"
grep -q 'case SYS_MT_STATS:' "$K"
grep -q 'static void mtstat_command' "$S"
grep -q 'eq(p,"watch")' "$S"
grep -q '#define MT_QUANTUM_TICKS  2u' "$K"
grep -q '#define RT_MAX_TASKS      8u' "$K"
echo 'PASS: FIX22 MTSTAT static checks'
