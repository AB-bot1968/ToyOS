#!/bin/sh
set -eu
f=src/execmt_task.c
! grep -q 'SYS_CONSOLE' "$f"
! grep -q 'SYS_SCHED_BLOCK' "$f"
! grep -q 'SYS_SCHED_WAKE' "$f"
! grep -q 'SYS_SCHED_EXIT' "$f"
grep -q 'SYS_MT_DATA 53u' "$f"
grep -q ':RUN;' "$f"
grep -q 'for(;;)' "$f"
grep -q 'session active; use mtstop ALL before a new execmt' src/user_shell.c
echo 'PASS: FIX17 minimal silent MT tasks and readable active-session error'
