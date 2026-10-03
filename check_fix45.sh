#!/usr/bin/env bash
set -euo pipefail
grep -q 'SYS_SAFE_MODE 63u' src/supervis.c
grep -q 'SUP_REASON_FAULT_DEGRADED 4501u' src/supervis.c
grep -q 'SUP_REASON_WATCHDOG_DEGRADED 4502u' src/supervis.c
grep -q 'SUP_REASON_FAULT_SAFE 4591u' src/supervis.c
grep -q 'SUP_REASON_WATCHDOG_SAFE 4592u' src/supervis.c
grep -q 'ASSERT SAFE_REASON 4591' TST/TESTPOL.TST
grep -q 'TST/TESTPOL.TST=TST/TESTPOL.TST' build.sh
grep -q 'for t in TST/\*.TST TST/ACCEPT.TXT' build.sh
echo 'FIX45 supervisor policy static check: OK'
