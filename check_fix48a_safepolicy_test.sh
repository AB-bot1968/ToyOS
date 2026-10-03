#!/usr/bin/env bash
set -euo pipefail
root="$(cd "$(dirname "$0")" && pwd)"
cd "$root"
if grep -Fq 'FIX48B SAFE MODE POLICY EXPECTED-DENIAL TEST FIX' VERSION.txt; then
  exec ./check_fix48b_safepolicy_test.sh
fi
need(){ grep -Fq "$2" "$1" || { echo "FIX48A check FAIL: $1 missing: $2"; exit 1; }; }
need VERSION.txt 'FIX48A SAFE MODE POLICY TEST FIX'
need TST/TESTSFP.TST 'TYPE exec RTD.EXE SENSOR1.EXE 10 10 1'
need TST/TESTSFP.TST 'KEY ENTER'
need TST/TESTSFP.TST 'KEY F10'
need TST/TESTSFP.TST 'ASSERT SAFE_DENY_RT 1'
need TST/TESTSFP.TST 'ASSERT SAFE_DENY_TOTAL 3'
if grep -Fxq 'START RT 1' TST/TESTSFP.TST; then echo 'FIX48A check FAIL: expected denial still uses START RT'; exit 1; fi
need src/kernel.c 'SYS_SAFE_POLICY        66u'
need src/kernel.c 'safe_deny_rt++'
need build.sh 'TST/TESTSFP.TST'
echo 'FIX48A SAFE policy regression test correction: OK'
