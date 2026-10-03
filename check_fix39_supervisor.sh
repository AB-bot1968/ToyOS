#!/bin/sh
set -eu
grep -q 'SYS_PROCESS_RESULT  59u' src/kernel.c
grep -q 'case SYS_PROCESS_RESULT' src/kernel.c
grep -q 'SUPERVIS.EXE' build.sh
grep -q 'TST/TESTSUP.TST' build.sh
grep -q 'ASSERT LAST_STATUS 3' TST/TESTSUP.TST
grep -q 'restarts<3u' src/supervis.c
! test -d build
echo 'FIX39 supervisor static check: OK'
