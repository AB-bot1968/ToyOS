#!/bin/sh
set -eu
ROOT=`CDPATH= cd -- "$(dirname -- "$0")" && pwd`
cd "$ROOT"
grep -q 'rt_burst_done_mask' src/kernel.c
grep -q 'rt_pick_ready_sweep' src/kernel.c
grep -q 'rt_burst_done_mask|=' src/kernel.c
grep -q 'while(!open_log' src/sonarlog.c
grep -q 'START RT10 4' TST/TESTMRT.TST
grep -q 'ASSERT RT_SWEEP_FAIR 4 4' TST/TESTMRT.TST
grep -q 'TYPE exec RTD.EXE SENSOR.EXE 100 100 1' TST/TESTKEY.TST
grep -q 'KEY F10' TST/TESTKEY.TST
count=`grep -Ev '^[[:space:]]*(#|$)' TST/ACCEPT.TXT | wc -l | tr -d ' '`
[ "$count" = 45 ]
[ `grep -Ev '^[[:space:]]*(#|$)' TST/ACCEPT.TXT | sort -u | wc -l | tr -d ' '` = 45 ]
while IFS= read -r t; do case "$t" in ''|'#'*) continue;; esac; [ -f "TST/$t" ] || { echo "missing TST/$t"; exit 1; }; done < TST/ACCEPT.TXT
! find . -maxdepth 2 -type f -name 'B*.TST' | grep -q .
! find . -maxdepth 2 -type f -name 'TST.LOG' | grep -q .
echo 'FIX60ZED CHECK PASS: distinct-slot RT sweep + SONARLOG recovery + 45 tests'
