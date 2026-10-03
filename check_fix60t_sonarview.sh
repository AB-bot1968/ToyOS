#!/bin/sh
set -eu
fail(){ echo "FIX60T SONARVIEW CHECK: FAIL: $1"; exit 1; }
grep -q 'src/sonarview.c' build.sh || fail 'viewer source not built'
grep -q 'SONARVWR.EXE=SONARVWR.EXE' build.sh || fail 'viewer not packaged'
./check_fix60c_fat83_names.sh >/dev/null || fail 'global FAT 8.3 gate failed'
! grep -R 'SONARVIEW\.EXE' build.sh TST/TESTLGR.TST RELEASE_NOTES_V67_11_B_FIX60T_RU.md VERIFICATION_V67_11_B_FIX60T_RU.md >/dev/null 2>&1 || fail 'stale non-8.3 viewer filename'
grep -q 'SYS_FILE_OPEN' src/sonarview.c || fail 'viewer does not open log'
grep -q 'SYS_FILE_PREAD' src/sonarview.c || fail 'viewer does not use positional reads'
grep -q 'MODE_READ' src/sonarview.c || fail 'viewer lacks read-only mode'
! grep -q 'SYS_FILE_WRITE' src/sonarview.c || fail 'viewer contains sequential write syscall'
! grep -q 'SYS_FILE_PWRITE' src/sonarview.c || fail 'viewer contains positional write syscall'
! grep -q 'MODE_CREATE\|MODE_TRUNC\|MODE_WRITE' src/sonarview.c || fail 'viewer contains writable open mode'
grep -q 'tlog_header_valid' src/sonarview.c || fail 'header CRC/format validation missing'
grep -q 'tlog_record_valid' src/sonarview.c || fail 'record CRC validation missing'
grep -q 'h.write_index+TLOG_CAPACITY-h.valid_count' src/sonarview.c || fail 'chronological ring traversal missing'
grep -q 'RUN exec SONARVWR.EXE' TST/TESTLGR.TST || fail 'runtime viewer regression missing'
grep -q 'ASSERT SONAR_LOG_VALID 128' TST/TESTLGR.TST || fail 'post-view log validation missing'
# FIX60S accepted transport path remains untouched.
echo 'd63f11028e836b93e341ae00ed13f31ee0ec3f6b7eee1ccf52c2a661eadbdff9  src/sonardrv.c' | sha256sum -c - >/dev/null 2>&1 || fail 'SONARDRV changed'
echo '6264bb7eb3a07785da20570e4cc699e36c64e5be588d9d04e72d597ba90fd6fb  src/uartrx.c' | sha256sum -c - >/dev/null 2>&1 || fail 'UARTRX changed'
echo 'e38af943c280b88e84b4817e5c28a2481e32bf7c10f81e3f07b1d9acb61bf3ab  tools/sonarsim/sonarsim.c' | sha256sum -c - >/dev/null 2>&1 || fail 'SONARSIM changed'
echo 'FIX60T SONARVIEW CHECK: PASS'
