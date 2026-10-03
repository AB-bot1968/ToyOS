#!/bin/sh
set -eu

grep -q '#define RT_MAX_TASKS      4u' src/kernel.c
grep -q 'static volatile uint32_t rt_task_count' src/kernel.c
grep -q 'static int rt_find_current(void)' src/kernel.c
grep -q 'static int rt_pick_ready(void)' src/kernel.c
grep -q 'for(i=0;i<RT_MAX_TASKS;i++)' src/kernel.c
grep -q 'rt_task_count>=RT_MAX_TASKS' src/kernel.c
grep -q 'sched_tasks\[id\].state==TASK_CREATE||sched_tasks\[id\].state==TASK_STOPPED||sched_tasks\[id\].state==TASK_EXIT' src/kernel.c
grep -q 'rt_task_count++;rt_background_active=1u' src/kernel.c
grep -q 'if(rt_task_count)rt_task_count--' src/kernel.c
grep -q 'rt_stop_requested|=(1u<<id)' src/kernel.c
grep -q 'rt_stop_requested&=~(1u<<id)' src/kernel.c
grep -q 'rt_pick_ready()' src/kernel.c
# Stage 3 must retain the FIX2 shell-frame safety rule.
grep -q 'Only a Ring-3 interrupt frame is safe to save' src/kernel.c
# Two invocations of RTD must be allowed while RT background is already active.
awk '/if\(sched_active\|\|queue_active\)return EXE_ERR_BUSY;/{seen=1} seen&&/if\(!exe_active\)return EXE_ERR_BUSY;/{print;exit}' src/kernel.c >/dev/null
if grep -q 'if(rt_background_active)return EXE_ERR_BUSY' src/kernel.c; then
  echo 'FAIL: Stage 3 still rejects a second RT task'; exit 1
fi
# RTD command ABI remains unchanged.
grep -q 'SYS_RT_START' src/user_shell.c
grep -q 'exec RTD.EXE SENSOR.EXE PERIOD_MS DEADLINE_MS PRIORITY \[DELAY_MS\]' src/user_shell.c
# Ordinary SENSOR remains an ordinary EXE1 and still uses RT_WAIT/ESC.
grep -q 'SYS_RT_WAIT' src/rt_sensor.c
grep -q 'SYS_CONSOLE_POLL' src/rt_sensor.c
grep -q 'SENSOR: ESC -> stopped' src/rt_sensor.c

echo 'RTD stage 3 checks passed'
