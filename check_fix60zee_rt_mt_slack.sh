#!/bin/sh
set -eu
ROOT=`CDPATH= cd -- "$(dirname -- "$0")" && pwd`
cd "$ROOT"
# No hidden exec->MT viewer transformation remains.
! grep -q 'exec_sonar_viewer' src/user_shell.c
grep -q 'use execmt SONARVWR.EXE' src/user_shell.c
grep -q 'active-session append' src/user_shell.c
# The old fairness mechanism must never assert the one-PIT hold.
! grep -q 'rt_shell_hold[[:space:]]*=[[:space:]]*1u' src/kernel.c
grep -q 'FIX60ZEE: do not buy MT progress' src/kernel.c
grep -q 'process_wait_mt_turn' src/kernel.c
# Live log test is monotonic/minimum, saturation remains exact elsewhere.
grep -q 'SONAR_LOG_MIN 8' TST/TESTMRT.TST
grep -q 'ASSERT SONAR_LOG_VALID 128' TST/TESTLGR.TST
grep -q 'START RT10 4' TST/TESTVWR.TST
grep -q 'RUN execmt SONARVWR.EXE' TST/TESTVWR.TST
grep -q 'WAIT LAST' TST/TESTVWR.TST
count=`grep -Ev '^[[:space:]]*(#|$)' TST/ACCEPT.TXT | wc -l | tr -d ' '`
[ "$count" = 45 ]
[ `grep -Ev '^[[:space:]]*(#|$)' TST/ACCEPT.TXT | sort -u | wc -l | tr -d ' '` = 45 ]
while IFS= read -r t; do
  case "$t" in ''|'#'*) continue;; esac
  [ -f "TST/$t" ] || { echo "missing TST/$t"; exit 1; }
  grep -q "build/TST/$t=TST/$t" build.sh || { echo "missing FAT payload $t"; exit 1; }
done < TST/ACCEPT.TXT
! find . -maxdepth 2 -type f -name 'B*.TST' | grep -q .
! find . -maxdepth 2 -type f -name 'TST.LOG' | grep -q .
[ ! -d build ]
echo 'FIX60ZEE CHECK PASS: RT post-sweep MT slack + explicit EXECMT viewer + WAIT fairness + 45 tests'
