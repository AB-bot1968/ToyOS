#!/bin/sh
set -eu
grep -q 'SYS_HW_WATCHDOG       64u' src/kernel.c
grep -q 'hwwd_backend=1u' src/kernel.c
grep -q 'case SYS_HW_WATCHDOG' src/kernel.c
grep -q 'hwwd \[status|arm TICKS|feed|disarm\]' src/user_shell.c
grep -q 'HWWD_BACKEND ' src/user_shell.c
grep -q 'HWWD_ARMED ' src/user_shell.c
grep -q 'HWWD_TIMEOUT ' src/user_shell.c
grep -q 'HWWD_FEEDS ' src/user_shell.c
grep -q 'TST/TESTHWWD.TST' build.sh
grep -q 'ASSERT HWWD_FEEDS 2' TST/TESTHWWD.TST
sed -n '/void interrupt_dispatch/,/case SYS_TIMER_GET/p' src/kernel.c | grep -q 'hwwd_last_feed' && exit 1 || true
echo 'FIX46 hardware watchdog static check: OK'
