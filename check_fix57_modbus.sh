#!/bin/sh
set -eu
f=src/modbus_rtu.c
[ -f "$f" ] && [ -f TST/TESTMB.TST ]
grep -q 'mb_crc16' "$f"
grep -q 'mb_build_read_input' "$f"
grep -q 'mb_parse_read_input' "$f"
grep -q 'mb_master_poll' "$f"
grep -q 'EXPECT MODBUS_RTU' TST/TESTMB.TST
grep -q 'src/modbus_rtu.c' build.sh
grep -q 'TST/TESTMB.TST=TST/TESTMB.TST' build.sh
grep -q '#define SYS_PORT_OUT8    13u' src/user_shell.c
grep -q '#define SYS_PORT_IN8     14u' src/user_shell.c
! grep -q 'modbus' src/kernel.c
echo 'FIX57-MODBUS CHECK: PASS'
