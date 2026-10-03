#!/bin/sh
set -eu
[ -f TST/TESTVWR.TST ]
grep -q '^TESTVWR.TST$' TST/ACCEPT.TXT
grep -q 'SONARVIEW_MAX_ROWS 8u' src/sonarview.c
grep -q 'build/TST/TESTVWR.TST=TST/TESTVWR.TST' build.sh
# No experimental foreground MT-yield mechanism from FIX60ZHC.
! grep -q 'FIX60ZHC: foreground EXE' src/kernel.c
! grep -q 'SYS_MT_YIELD' src/sonarview.c
count=$(grep -E '^TEST[A-Z0-9]+\.TST$' TST/ACCEPT.TXT | sort -u | wc -l)
[ "$count" -eq 45 ]
for t in $(grep -E '^TEST[A-Z0-9]+\.TST$' TST/ACCEPT.TXT); do [ -f "TST/$t" ]; done
echo 'FIX60ZHD CHECK PASS: stable FIX60ZG scheduler + bounded viewer + 45 tests'
