#!/bin/sh
set -eu
[ -f TST/TESTMRT.TST ]
grep -q '^TESTMRT.TST$' TST/ACCEPT.TXT
grep -q 'if(rt_shell_waiting)' src/kernel.c
grep -q 'FIX60ZF: SYS_CONSOLE_READ is the foreground ownership boundary' src/kernel.c
grep -q 'TESTMRT.TST=TST/TESTMRT.TST' build.sh
grep -q 'TST/TESTMRT.TST' build.sh
grep -q 'FIX60ZF' TOYOS_ARCHITECTURE_VISION.md
! grep -q 'TST.LOG' src/user_shell.c
find TST -maxdepth 1 -name 'B*.TST' | grep . && exit 1 || true
printf '%s\n' 'FIX60ZF MIXED MT RT CONSOLE: PASS'
