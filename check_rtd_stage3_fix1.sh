#!/bin/sh
set -eu

grep -q 'sched_tasks\[id\].state==TASK_CREATE||sched_tasks\[id\].state==TASK_STOPPED||sched_tasks\[id\].state==TASK_EXIT' src/kernel.c
if grep -q 'for(id=0;id<RT_MAX_TASKS;id++)if(sched_tasks\[id\].state==TASK_STOPPED||sched_tasks\[id\].state==TASK_EXIT)' src/kernel.c; then
  echo 'FAIL: TASK_CREATE is still treated as busy'
  exit 1
fi

echo 'RTD stage 3 FIX1 checks passed'
