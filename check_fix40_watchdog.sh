#!/bin/sh
set -eu
grep -q 'SYS_PROCESS_HEARTBEAT 60u' src/kernel.c
grep -q 'SYS_PROCESS_STOP_PID  61u' src/kernel.c
grep -q 'PROCESS_EXIT_WATCHDOG 3u' src/kernel.c
grep -q 'WATCHDOG_TIMEOUT_TICKS 100u' src/supervis.c
grep -q 'TST/TESTWD.TST' build.sh
grep -q 'TST/TESTHB.TST' build.sh
grep -q 'HBEATOK.EXE' build.sh
grep -q 'HANGWD.EXE' build.sh
[ ! -d build ]
echo 'FIX40 watchdog static check: OK'
