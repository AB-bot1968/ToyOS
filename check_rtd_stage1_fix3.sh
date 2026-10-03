#!/bin/sh
set -eu
fail(){ echo "FAIL: $1"; exit 1; }
grep -q 'rt_shell_frame\[MT_CONTEXT_WORDS\]' src/kernel.c || fail 'private RT shell context missing'
grep -q 'rt_shell_waiting' src/kernel.c || fail 'RT shell waiting state missing'
grep -q 'rt_shell_waiting=1u' src/kernel.c || fail 'console read does not yield to RT task'
grep -q 'read_cr3()==sched_saved_shell_cr3&&rt_shell_waiting' src/kernel.c || fail 'timer can preempt arbitrary kernel context'
! grep -q 'syscall_dispatch_active' src/kernel.c || fail 'obsolete global syscall gate remains'
grep -q 'rt_shell_frame_valid=0u' src/kernel.c || fail 'RT shell frame cleanup missing'
grep -q 'rt_stop_requested' src/kernel.c || fail 'ESC stop request missing'
grep -q 'SYS_CONSOLE_POLL' src/rt_sensor.c || fail 'SENSOR ESC polling missing'
echo 'RTD stage1 FIX3 checks passed'
