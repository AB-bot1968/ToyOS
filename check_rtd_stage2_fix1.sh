#!/bin/sh
set -eu
f=src/kernel.c
s=src/rt_sensor.c
ok=1
grep -q 'SYS_RT_WAIT' "$f" || ok=0
grep -q 'mem_copy(sched_tasks\[rt_task_id\].words,(const void\*)f,MT_CONTEXT_WORDS\*4u);' "$f" || ok=0
# The save must occur before TASK_BLOCKED is assigned.
awk '/static void rt_wait\(struct frame\*f\)/,/^}/' "$f" | awk '
/mem_copy\(sched_tasks\[rt_task_id\]\.words/ {save=NR}
/state=TASK_BLOCKED/ {block=NR}
END { if (!(save && block && save < block)) exit 1 }
' || ok=0
# SENSOR remains a normal EXE1 and waits after each job.
grep -q 'sc(SYS_RT_WAIT,0,0,0)' "$s" || ok=0
grep -q 'sc(SYS_EXIT' "$s" || ok=0
if [ "$ok" -eq 1 ]; then echo 'RTD stage2 FIX1 checks passed'; exit 0; fi
exit 1
