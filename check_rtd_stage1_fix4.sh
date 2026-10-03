#!/bin/sh
set -eu
fail(){ echo "FAIL: $1"; exit 1; }
grep -q '#define SYS_RT_INFO        39u' src/kernel.c || fail 'SYS_RT_INFO missing in kernel'
grep -q 'case SYS_RT_INFO:' src/kernel.c || fail 'RT info syscall missing'
grep -q 'rt_stop_requested' src/kernel.c || fail 'RT stop state missing'
! grep -q 'if(rt_stop_requested){' src/kernel.c || fail 'timer must not terminate RT before SENSOR consumes ESC'
grep -q 'period_ticks=(period_ms+19u)/20u' src/rt_sensor.c || fail 'period is not reflected in SENSOR output cadence'
grep -q 'delay_ms' src/rtd.c || fail 'RTD delay missing'
grep -q '\[DELAY_MS\]' src/user_shell.c || fail 'shell optional delay syntax missing'
echo 'RTD stage1 FIX4 checks passed'
