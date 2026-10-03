#!/bin/sh
set -eu
K=src/kernel.c
grep -q 'FIX25: SYS_CONSOLE_READ is a direct RT dispatch path' "$K"
grep -q 'rt_shell_waiting=1u;rt_return_cr3=read_cr3();mem_copy(rt_shell_frame' "$K"
grep -Eq '#define[[:space:]]+RT_MAX_TASKS[[:space:]]+8u' "$K"
grep -Eq '#define[[:space:]]+MT_QUANTUM_TICKS[[:space:]]+2u' "$K"
echo 'PASS: FIX25 console return-context static checks'
