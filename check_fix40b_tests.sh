#!/usr/bin/env bash
set -euo pipefail
grep -q 'LAST_MTDATA ' src/user_shell.c
grep -q 'ARGV:ARGVDIAG.EXE|ONE|TWO|THREE' TST/TESTKEY.TST
grep -q 'TYPE spawn FAULTUD.EXE' TST/TESTKEY.TST
grep -q 'TYPE spawn FAULTGP.EXE' TST/TESTKEY.TST
grep -q 'TYPE spawn FAULTPF.EXE' TST/TESTKEY.TST
! test -d build
echo 'FIX40B test static check: OK'
