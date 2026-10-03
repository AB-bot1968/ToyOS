#!/bin/sh
set -eu
fail(){ echo "FIX60R CHECK FAIL: $1" >&2; exit 1; }
grep -q 'const char name\[\]="/SONAR.LOG"' src/sonarlog.c || fail "absolute SONAR.LOG"
grep -q 'SYS_PROCESS_HEARTBEAT' src/sonarlog.c || fail "logger heartbeat"
grep -q 'reader==0xffffffffu.*idle_delay.*continue' src/sonarlog.c || fail "producer wait/retry"
grep -q 'MODE_TRUNC' src/sonarlog.c || fail "bounded corrupt-file recovery"
grep -q 'owner_pid' src/kernel.c || fail "reader owner"
grep -q 'data_readers_close_pid' src/kernel.c || fail "reader lifecycle cleanup"
grep -q 'MT_HEARTBEAT_WAIT' src/user_shell.c || fail "bounded logger wait assertion"
grep -q 'SONARTST.EXE SONARLOG.EXE' TST/TESTLGR.TST || fail "dual-task test"
if grep -q 'FIX60S SONARLOG PROCESS/CYCLIC LOG TEST' TST/TESTLGR.TST; then
  grep -q 'SONARLOG.EXE SONARPUB.EXE' TST/TESTLGR.TST || fail "FIX60S consumer-first test"
else
  grep -q 'SONARLOG.EXE SONARTST.EXE' TST/TESTLGR.TST || fail "consumer-first test"
fi
grep -q 'FILE_SIZE /SONAR.LOG 4160' TST/TESTLGR.TST || fail "bounded file assertion"
grep -q 'TST/TESTLGR.TST' build.sh || fail "test packaging"
grep -q 'objcopy --only-section=.text -O binary build/sonarlog.pe' build.sh || fail "sonarlog raw extraction"
grep -q 'FIX60R — autonomous lifecycle SONARLOG' TOYOS_ARCHITECTURE_VISION.md || fail "architecture vision"
test -f VERIFICATION_V67_11_B_FIX60R_RU.md || fail "verification doc"
test -f RELEASE_NOTES_V67_11_B_FIX60R_RU.md || fail "release notes"
echo "FIX60R SONARLOG LIFECYCLE CHECK: PASS"
