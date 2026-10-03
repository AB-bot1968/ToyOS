#!/bin/sh
set -eu
fail(){ echo "FAIL: $1"; exit 1; }
pass(){ echo "PASS: $1"; }

grep -q 'static volatile uint32_t rt_shell_hold' src/kernel.c || fail 'shell hold state missing'
grep -q 'rt_shell_hold=1u;' src/kernel.c || fail 'RT-to-shell hold not armed'
grep -q 'if(rt_shell_hold){rt_shell_hold=0u;__asm__ volatile("sti;hlt;cli"' src/kernel.c || fail 'console read does not honor shell hold'
grep -q 'rt_shell_waiting=0;rt_shell_hold=0u' src/kernel.c || fail 'keyboard read does not clear shell hold'
# RT keyboard polling must not pop ordinary shell keys from the shared queue.
grep -q 'if(rid>=0){/\* Detached RT owns only the explicit ESC stop event' src/kernel.c || fail 'RT poll still owns ordinary keyboard input'
# Keep the FIX2/FIX3 prompt sentinel behavior intact.
grep -q 'rt_shell_frame\[7\]=2u;' src/kernel.c || fail 'RT quantum sentinel missing'
grep -q 'if(r==2u)continue;' src/user_shell.c || fail 'shell quantum sentinel handling missing'
grep -q 'if(r==3u){put("toy0> ");continue;}' src/user_shell.c || fail 'last RT exit prompt handling missing'
# Keep ordinary SENSOR EXE1 and ESC termination path.
grep -q 'SYS_RT_WAIT' src/rt_sensor.c || fail 'SENSOR RT wait missing'
grep -q 'SENSOR: ESC -> stopped' src/rt_sensor.c || fail 'SENSOR ESC message missing'

echo 'RTD stage 3 FIX4 checks passed'
