#!/bin/sh
set -eu
grep -q 'START MT N' PROJECT_BASELINE_V67_11_B_FIX40A.md
grep -q 'test_start_mt' src/user_shell.c
grep -q 'test_start_rt' src/user_shell.c
grep -q 'KEY F10' src/user_shell.c
grep -q 'KEY ENTER' src/user_shell.c
grep -q 'START MT 25' TST/TESTLOAD.TST
grep -q 'START RT 8' TST/TESTLOAD.TST
grep -q 'KEY F10' TST/TESTKEY.TST
grep -q 'TST/TESTLOAD.TST=TST/TESTLOAD.TST' build.sh
grep -q 'TST/TESTKEY.TST=TST/TESTKEY.TST' build.sh
test ! -d build
echo 'FIX40A load-test static check: OK'
