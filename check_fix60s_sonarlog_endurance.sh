#!/bin/sh
set -eu
fail(){ echo "FIX60S SONARLOG ENDURANCE CHECK: FAIL: $1"; exit 1; }
grep -q 'FIX60S deterministic telemetry producer' src/sonarpub.c || fail 'SONARPUB source marker'
grep -q 'SONARPUB.EXE' build.sh || fail 'SONARPUB build/package'
grep -q 'SONAR_LOG_VALID 128' TST/TESTLGR.TST || fail 'cyclic log content assertion'
grep -q 'MT_HEARTBEAT_WAIT 2 130' TST/TESTLGR.TST || fail 'real logger endurance heartbeat'
grep -q 'execmt SONARLOG.EXE SONARPUB.EXE' TST/TESTLGR.TST || fail 'consumer-first regression'
grep -q 'execmt SONARTST.EXE SONARLOG.EXE' TST/TESTLGR.TST || fail 'existing SONAR integration regression'
grep -q 'data_readers_close_pid' src/kernel.c || fail 'reader lifecycle cleanup'
grep -q 'reader==0xffffffffu' src/sonarlog.c || fail 'producer wait/reacquire'
grep -q 'init_file' src/sonarlog.c || fail 'bounded log recovery'
# Accepted FIX60Q sensor/transport sources must remain byte-identical.
echo 'd63f11028e836b93e341ae00ed13f31ee0ec3f6b7eee1ccf52c2a661eadbdff9  src/sonardrv.c' | sha256sum -c - >/dev/null 2>&1 || fail 'SONARDRV changed'
echo '6264bb7eb3a07785da20570e4cc699e36c64e5be588d9d04e72d597ba90fd6fb  src/uartrx.c' | sha256sum -c - >/dev/null 2>&1 || fail 'UARTRX changed'
echo 'e38af943c280b88e84b4817e5c28a2481e32bf7c10f81e3f07b1d9acb61bf3ab  tools/sonarsim/sonarsim.c' | sha256sum -c - >/dev/null 2>&1 || fail 'SONARSIM changed'
echo 'FIX60S SONARLOG ENDURANCE CHECK: PASS'
