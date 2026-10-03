#!/usr/bin/env bash
set -eu
fail(){ echo "FIX49 CHECK: FAIL: $*"; exit 1; }
grep -q 'SYS_RECOVERY_EVENT     67u' src/kernel.c || fail syscall
grep -q 'EVENT_REC_DEGRADED 101u' src/kernel.c || fail degraded
grep -q 'EVENT_REC_BUDGET 105u' src/kernel.c || fail budget
grep -q 'process_event_store(0u,a==0u?EVENT_REC_NORMAL' src/kernel.c || fail safe_transition
grep -q 'recovery_event(EVENT_REC_WATCHDOG' src/supervis.c || fail watchdog_marker
grep -q 'recovery_event(EVENT_REC_RESTART' src/supervis.c || fail restart_marker
grep -q 'recovery_event(EVENT_REC_BUDGET' src/supervis.c || fail budget_marker
grep -q 'recoverylog' src/user_shell.c || fail command
grep -q 'RECOVERY_FAULT_CHAIN' src/user_shell.c || fail structured_assert
grep -q 'TST/TESTRCV.TST' build.sh || fail fat_test
[ -f TST/TESTRCV.TST ] || fail missing_test
# No filesystem calls are introduced in kernel/supervisor recovery markers.
! grep -n 'SYS_FILE_\|fat_' src/supervis.c >/dev/null || fail supervisor_filesystem_io
echo 'FIX49 CHECK: PASS'
