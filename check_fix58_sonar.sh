#!/bin/sh
set -eu
fail(){ echo "FIX58 SONAR CHECK: FAIL: $1"; exit 1; }
grep -q 'src/sonardrv.c' build.sh || fail build-source
grep -q 'build/SONARDRV.EXE=SONARDRV.EXE' build.sh || fail fat-pack
grep -q 'TST/TESTSON.TST' build.sh || fail test-pack
grep -q 'EXPECT SONAR_DRIVER' TST/TESTSON.TST || fail acceptance
grep -q 'mb_build_read_input(1u,0u,6u' src/sonardrv.c || fail f04-six-registers
grep -q 'sonar_decode_xyz(regs,6u' src/sonardrv.c || fail xyz-decode
grep -q 'SYS_DATA_CHANNEL' src/sonardrv.c || fail data-channel
grep -q 'SYS_PROCESS_HEARTBEAT' src/sonardrv.c || fail heartbeat
grep -q 'SYS_UART_TRANSPORT' src/sonardrv.c || fail uart
grep -q 'SYS_PORT_OUT8.*13u' src/kernel.c || fail legacy-port-out
grep -q 'SYS_PORT_IN8.*14u' src/kernel.c || fail legacy-port-in
echo 'FIX58 SONAR CHECK: PASS'
