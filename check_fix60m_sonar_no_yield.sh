#!/bin/sh
set -eu
S=src/sonardrv.c
fail(){ echo "FIX60M SONAR no-yield regression: FAIL: $1"; exit 1; }
test -f TST/TESTSONM.TST || fail test
# Match an actual syscall expression, not comments mentioning the ABI name.
if grep -q 'sc(SYS_MT_YIELD' "$S"; then fail active_yield_call; fi
grep -q 'FIX60M' "$S" || fail marker
grep -q 'RUN execmt SONARDRV.EXE' TST/TESTSONM.TST || fail launch
echo 'FIX60M SONAR no-yield regression: PASS'
