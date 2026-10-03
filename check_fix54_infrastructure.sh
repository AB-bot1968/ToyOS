#!/bin/sh
set -eu
fail(){ echo "FIX54-INFRA CHECK: FAIL: $*"; exit 1; }
grep -q 'safe_mode==2u){safe_deny_mt++;f->eax=SAFE_POLICY_DENIED;}else sched_start' src/kernel.c || fail 'legacy SYS_MT_START SAFE guard missing'
grep -q 'PROCESS_EXIT_STOPPED 4u' src/kernel.c || fail 'STOPPED process result missing'
grep -q 'process_result_find(a))==0' src/kernel.c || fail 'Recovery STOPPED process-result guard missing'
grep -q 'q\[0\]==q\[1\].*RECOVERY_STATE_STOPPED.*process_pid_known' src/kernel.c || fail 'Recovery SPAWNED guards missing'
grep -q 'process_heartbeat_find(a)).*h->seq==0u' src/kernel.c || fail 'Recovery VERIFIED heartbeat guard missing'
grep -q 'SAFE_DENY_MT_START' src/user_shell.c || fail 'TST SAFE legacy-start expectation missing'
grep -q 'WAIT LAST did not obtain process result' src/user_shell.c || fail 'WAIT LAST truth guard missing'
! grep -q 'if(test_run_command(line+4))pass++' src/user_shell.c || fail 'RUN still inflates PASS'
test -f TST/TESTINF.TST || fail 'TST/TESTINF.TST missing'
grep -q '^EXPECT SAFE_DENY_MT_START$' TST/TESTINF.TST || fail 'SAFE acceptance missing'
grep -q '^EXPECT RM_GUARDS$' TST/TESTINF.TST || fail 'Recovery acceptance missing'
grep -q '^ASSERT LAST_REASON 4$' TST/TESTINF.TST || fail 'STOP result acceptance missing'
grep -q 'TST/TESTINF.TST=TST/TESTINF.TST' build.sh || fail 'TESTINF not packed'
# Explicitly protect the accepted I/O architecture.
grep -q 'case SYS_PORT_OUT8' src/kernel.c || fail 'SYS_PORT_OUT8 missing'
grep -q 'case SYS_PORT_IN8' src/kernel.c || fail 'SYS_PORT_IN8 missing'
echo 'FIX54-INFRA CHECK: PASS'
