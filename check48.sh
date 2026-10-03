#!/bin/sh
set -eu
K=src/kernel.c
T=src/execmt_task.c
# Scheduler-created Ring-3 contexts must restore user data segments.
grep -q 't->words\[8\]=0x23u' "$K"
grep -q 't->words\[9\]=0x23u' "$K"
grep -q 't->words\[10\]=0x23u' "$K"
grep -q 't->words\[11\]=0x23u' "$K"
# EXECMT task still emits its line through exactly one console syscall.
grep -q 'sc(SYS_CONSOLE_WRITE,(uint32_t)task_line,8u,0)' "$T"
# Keep the task ABI and command surface intact.
grep -q 'SYS_EXECMT' src/user_shell.c
grep -q 'execmt ' src/user_shell.c
grep -q 'SYS_LS' src/user_shell.c
printf '%s\n' 'v48 static checks: PASS'
