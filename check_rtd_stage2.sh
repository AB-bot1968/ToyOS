#!/bin/sh
set -eu
fail=0
ok(){ echo "PASS: $1"; }
bad(){ echo "FAIL: $1"; fail=1; }

grep -q '#define SYS_RT_WAIT[[:space:]]*40u' src/kernel.c && ok 'SYS_RT_WAIT=40' || bad 'SYS_RT_WAIT=40'
grep -q 'rt_period_ticks' src/kernel.c && ok 'period converted to scheduler ticks' || bad 'period conversion'
grep -q 'rt_next_release_tick' src/kernel.c && ok 'absolute periodic release' || bad 'periodic release state'
grep -q 'rt_deadline_tick' src/kernel.c && ok 'absolute deadline state' || bad 'deadline state'
grep -q 'rt_missed_deadlines' src/kernel.c && ok 'deadline miss accounting' || bad 'deadline miss accounting'
grep -q 'rt_runtime_budget' src/kernel.c && ok 'priority-derived runtime budget' || bad 'priority budget'
grep -q 'SYS_RT_WAIT:rt_wait' src/kernel.c && ok 'RT wait syscall dispatch' || bad 'RT wait dispatch'
grep -q 'sc(SYS_RT_WAIT' src/rt_sensor.c && ok 'SENSOR completes periodic job with RT_WAIT' || bad 'SENSOR RT_WAIT'
if grep -q 'rt_period_ticks\*req.priority' src/kernel.c; then bad 'overflow-prone priority multiplication remains'; else ok 'priority budget multiplication is overflow-safe'; fi
if grep -q 'if(rt_background_active){rt_shell_waiting' src/kernel.c; then bad 'shell read dispatches RT without active job'; else ok 'shell read requires an active RT job'; fi
# RT_WAIT must block the RT task until next release rather than spinning.
grep -A22 'static void rt_wait' src/kernel.c | grep -q 'TASK_BLOCKED' && ok 'RT_WAIT blocks until next release' || bad 'RT_WAIT blocking'
exit $fail
