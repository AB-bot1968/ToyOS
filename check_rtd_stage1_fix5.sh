#!/bin/sh
set -eu
fail=0
check(){ if "$@"; then :; else echo "FAIL: $*"; fail=1; fi; }
check grep -q 'SYS_CONSOLE_POLL.*0x1bu' src/rt_sensor.c
check grep -q 'key==0x1bu' src/rt_sensor.c
check grep -q 'delay_ms' src/rtd.c
check grep -q 'delay_ms+19u' src/rtd.c
check grep -q 'period_ticks=(period_ms+19u)/20u' src/rt_sensor.c
if grep -q 'SYS_CONSOLE_POLL.*)==1u&&key==0x1bu' src/rt_sensor.c; then echo 'FAIL: stale ESC return-value check'; fail=1; fi
if [ "$fail" -ne 0 ]; then exit 1; fi
echo 'RTD stage1 FIX5 focused checks passed'
