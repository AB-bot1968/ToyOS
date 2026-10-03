#!/bin/sh
set -eu
K=src/kernel.c
T=src/execmt_task.c
L=program.ld
# EXECMT task text is explicitly placed in the raw EXE image output section,
# avoiding any dependency on a separate .rodata output being copied.
grep -q 'section(".usercode_data")' "$T"
grep -q '\*(.usercode_data\*)' "$L"
# The task line remains a single syscall from the executable.
grep -q 'sc(SYS_CONSOLE_WRITE,(uint32_t)task_line,8u,0)' "$T"
# Scheduler user data segments remain valid.
grep -q 't->words\[8\]=0x23u' "$K"
grep -q 't->words\[11\]=0x23u' "$K"
# Existing command surface must remain intact.
grep -q 'execmt ' src/user_shell.c
grep -q 'execq ' src/user_shell.c
grep -q 'SYS_LS' src/user_shell.c
printf '%s\n' 'v49 static checks: PASS'
