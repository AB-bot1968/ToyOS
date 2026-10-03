#!/usr/bin/env bash
set -euo pipefail
grep -q 'SYS_SYSTEM_HEALTH      65u' src/kernel.c
grep -q 'case SYS_SYSTEM_HEALTH' src/kernel.c
grep -q 'sys_system_health' src/user_shell.c
grep -q 'eq(line,"health")' src/user_shell.c
grep -q 'HEALTH_STATE ' src/user_shell.c
grep -q 'HEALTH_FLAGS ' src/user_shell.c
grep -q 'TST/TESTHLTH.TST' build.sh
grep -q 'ASSERT HEALTH_STATE 1' TST/TESTHLTH.TST
grep -q 'ASSERT HEALTH_STATE 2' TST/TESTHLTH.TST
grep -q 'RUN hwwd disarm' TST/TESTHLTH.TST
! grep -q 'hwwd_last_feed=timer_ticks' <(grep -n 'timer_irq\|pit' src/kernel.c || true)
echo 'FIX47 system health static check: OK'
