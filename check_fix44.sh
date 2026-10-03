#!/usr/bin/env bash
set -euo pipefail
grep -q 'SYS_SAFE_MODE.*63u' src/kernel.c
grep -q 'case SYS_SAFE_MODE' src/kernel.c
grep -q 'safemode degraded 41' TST/TESTSAFE.TST
grep -q 'ASSERT SAFE_MODE 2' TST/TESTSAFE.TST
grep -q 'ASSERT SAFE_REASON 77' TST/TESTSAFE.TST
grep -q 'TST/TESTSAFE.TST=TST/TESTSAFE.TST' build.sh
echo 'FIX44 safe-mode static check: OK'
