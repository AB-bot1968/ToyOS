#!/bin/sh
set -eu
[ "$(grep -c '^ASSERT RT_SLOT_PROGRESS ' TST/TESTACC.TST)" -eq 1 ]
[ "$(grep -c '^ASSERT RT_DUMP_NONEMPTY ' TST/TESTACC.TST)" -eq 1 ]
grep -q '^RUN rTsTaT sToP sLoT4$' TST/TESTACC.TST
grep -q '^ASSERT RT_ACTIVE 3$' TST/TESTACC.TST
grep -q '^ASSERT RT_ACTIVE 0$' TST/TESTACC.TST
grep -q '^ASSERT RT_ACTIVE 1$' TST/TESTKEY.TST
! grep -q '^ASSERT RT_ACTIVE 8$' TST/TESTKEY.TST
grep -q 'if(f->eax==SYS_RT_YIELD)f->eax=0u;' src/kernel.c
! grep -R -q 'TST.LOG' src/user_shell.c
! find TST -name 'B*.TST' | grep -q .
echo 'FIX60ZE RT YIELD/TEST SCOPE: PASS'
