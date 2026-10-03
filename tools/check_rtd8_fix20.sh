#!/bin/sh
set -eu
grep -q '#define RT_MAX_TASKS      8u' src/kernel.c
grep -q '#define RT_PENDING_MAX 8u' src/user_shell.c
grep -q 'slot8' src/user_shell.c
grep -q 'RT_SENSOR_DIAG_ID > 8' src/rt_sensor_diag.c
grep -q 'SENSOR8.EXE=SENSOR8.EXE' build.sh
grep -q '#define SCHED_TASKS      25u' src/kernel.c
grep -q '#define MT_QUANTUM_TICKS  2u' src/kernel.c
grep -q 'for(i=0u;i<RT_MAX_TASKS;i++)if(rt_tasks\[i\].state' src/kernel.c
! grep -q '#define RT_MAX_TASKS      4u' src/kernel.c
echo 'PASS: FIX20 RTD8 static checks'
