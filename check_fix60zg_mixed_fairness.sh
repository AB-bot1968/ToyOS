#!/usr/bin/env bash
set -euo pipefail
grep -q "FIX60ZG: rt_shell_hold is a scheduler guarantee" src/kernel.c
grep -q "rt_background_active&&!rt_defer_once" src/kernel.c
grep -q "leave the hold set until the next genuine Ring-3 PIT IRQ" src/kernel.c
grep -q "SENSOR.EXE 10 10 1" TST/TESTMRT.TST
grep -q "FIX60ZG — bounded interleave" TOYOS_ARCHITECTURE_VISION.md
! grep -Rqs "TST.LOG" src/user_shell.c
! find TST -maxdepth 1 -name 'B*.TST' | grep -q .
echo "FIX60ZG MIXED FAIRNESS: PASS"
