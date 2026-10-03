#!/bin/sh
set -eu
fail(){ echo "FIX56-DATA CHECK: FAIL: $*" >&2; exit 1; }
grep -q 'SYS_DATA_CHANNEL.*71u' src/kernel.c || fail syscall
grep -q '#define DATA_CH_DEPTH 8u' src/kernel.c || fail depth
grep -q '#define DATA_CH_PAYLOAD 32u' src/kernel.c || fail payload
grep -q 'DATA_READ_OVERRUN' src/kernel.c || fail overrun
grep -q 'dr->generation!=dc->generation' src/kernel.c || fail generation
grep -q 'timestamp_us=rt_time_ticks\*10000u' src/kernel.c || fail timestamp
grep -q 'EXPECT DATA_CHANNEL' TST/TESTDATA.TST || fail test
grep -q 'build/TST/TESTDATA.TST=TST/TESTDATA.TST' build.sh || fail pack
grep -q 'case SYS_PORT_OUT8' src/kernel.c || fail io13
grep -q 'case SYS_PORT_IN8' src/kernel.c || fail io14
echo 'FIX56-DATA CHECK: PASS'
