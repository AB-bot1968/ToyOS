#!/bin/sh
set -eu

grep -q 'rt_release_jobs(now);' src/kernel.c
grep -q 'if(t->state==TASK_RUNNING)t->state=TASK_READY;' src/kernel.c
grep -q 't->rt_runtime_ticks>=t->rt_runtime_budget' src/kernel.c
grep -q 'rt_switch_to(f,next);' src/kernel.c
grep -q 'IRQ0 never stores its Ring-0 frame' src/kernel.c
grep -q 'static void rt_switch_to(struct frame\*f,int next)' src/kernel.c
grep -q 'rt_find_current()' src/kernel.c
grep -q 'rt_task_count' src/kernel.c
# The single-task path must remain compatible with FIX2: shell is still the
# fallback context and SYS_CONSOLE_READ still captures its user frame.
grep -q 'rt_shell_frame_valid' src/kernel.c
grep -q 'rt_shell_waiting' src/kernel.c
# Slot allocation must recognize the initial TASK_CREATE state.
grep -q 'state==TASK_CREATE||sched_tasks\[id\].state==TASK_STOPPED||sched_tasks\[id\].state==TASK_EXIT' src/kernel.c

echo 'RTD stage 3 FIX2 checks passed'
