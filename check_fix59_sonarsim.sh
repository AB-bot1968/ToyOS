#!/bin/sh
set -eu
fail(){ echo "FIX59 SONARSIM CHECK: FAIL: $1"; exit 1; }
test -f tools/sonarsim/sonarsim.c || fail source
test -f tools/sonarsim/build_w64devkit.bat || fail windows-build
test -f tools/sonarsim/README_RU.md || fail readme
grep -q 'FM_NO_RESPONSE' tools/sonarsim/sonarsim.c || fail no-response
grep -q 'FM_BAD_CRC' tools/sonarsim/sonarsim.c || fail bad-crc
grep -q 'FM_DELAY' tools/sonarsim/sonarsim.c || fail delay
grep -q 'FM_PARTIAL' tools/sonarsim/sonarsim.c || fail partial
grep -q 'FM_EXCEPTION' tools/sonarsim/sonarsim.c
grep -q 'memmove(q,q+1,7)' tools/sonarsim/sonarsim.c
grep -q 'READY' tools/sonarsim/sonarsim.c || fail exception
grep -q 'req\[1\]!=4' tools/sonarsim/sonarsim.c || fail function04
grep -q 'req\[5\]!=6' tools/sonarsim/sonarsim.c || fail six-registers
grep -q 'put_i32_regs' tools/sonarsim/sonarsim.c || fail xyz-format
grep -q 'TST/TESTSIM.TST' build.sh || fail testsim-pack
grep -q 'EXPECT MODBUS_RTU' TST/TESTSIM.TST || fail modbus-acceptance
grep -q 'EXPECT SONAR_DRIVER' TST/TESTSIM.TST || fail sonar-acceptance
grep -q 'SYS_PORT_OUT8.*13u' src/kernel.c || fail port13
grep -q 'SYS_PORT_IN8.*14u' src/kernel.c || fail port14
echo 'FIX59 SONARSIM CHECK: PASS'
