#!/bin/sh
set -eu
[ "$(find TST -maxdepth 1 -type f -name 'TEST*.TST' | wc -l)" -eq 43 ]
! find TST -maxdepth 1 -type f -name 'B*.TST' | grep -q .
! grep -q 'TST.LOG' src/user_shell.c
grep -q 'START RT 1' TST/TESTACC.TST
grep -q 'START RT 4' TST/TESTACC.TST
grep -q 'ASSERT MT_HEARTBEAT_WAIT 1 1000 100' TST/TESTSONP.TST
echo 'FIX60ZA TEST SEMANTICS: PASS'
