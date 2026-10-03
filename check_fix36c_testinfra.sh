#!/bin/sh
set -eu
grep -Fq 'test [F.TST]' src/user_shell.c
grep -q 'ASSERT RT_ACTIVE' PROJECT_BASELINE_V67_11_B_FIX36C.md
grep -q 'for t in TST/\*.TST TST/ACCEPT.TXT' build.sh
grep -q 'build/TST/TESTCORE.TST=TST/TESTCORE.TST' build.sh
grep -q 'build/TST/TESTPROC.TST=TST/TESTPROC.TST' build.sh
test -f TST/TESTCORE.TST
test -f TST/TESTPROC.TST
! grep -q '#define SYS_PROCESS_WAIT     57' src/user_shell.c
printf '%s\n' 'FIX36C test infrastructure static check: OK'
