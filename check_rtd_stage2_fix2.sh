#!/bin/sh
set -eu
grep -Rq 'IRQ0 never copies its Ring-0 frame into' changes/RTD || { echo 'FAIL: FIX2 shell-frame invariant documentation missing'; exit 1; }
awk '/else if\(read_cr3\(\)==sched_saved_shell_cr3&&rt_shell_waiting\)/{seen=1; n=0; next} seen && n++<8 { if ($0 ~ /mem_copy\(rt_shell_frame/) exit 2 } END { if (seen!=1) exit 3 }' src/kernel.c || { echo 'FAIL: IRQ0 writes RT shell frame'; exit 1; }
grep -q 'rt_shell_frame,MT_CONTEXT_WORDS\*4u' src/kernel.c || { echo 'FAIL: shell frame restore missing'; exit 1; }
echo 'RTD stage2 FIX2 checks passed'
