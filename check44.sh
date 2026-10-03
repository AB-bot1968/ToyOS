#!/bin/sh
set -eu
fail(){ echo "CHECK44 FAIL: $*"; exit 1; }
grep -q 'case SYS_SCHED_EXIT:if(f->cs&3u){if(sched_active)sched_exit(f);else exe_exit(f);}' src/kernel.c || fail 'SYS_SCHED_EXIT standalone fallback missing'
grep -q 'case SYS_EXIT:if(f->cs&3u){if(sched_active)sched_exit(f);else exe_exit(f);}' src/kernel.c || fail 'SYS_EXIT compatibility missing'
grep -q 'char line\[8\];' src/execmt_task.c || fail 'atomic Task line builder missing'
grep -q 'Task 02: BLOCKED' src/execmt_task.c || fail 'Task 02 lifecycle text missing'
grep -q 'Task 03: WAKE PASS' src/execmt_task.c || fail 'Task 03 wake text missing'
grep -q 'exec FILE.EXE' src/user_shell.c || fail 'exec command missing'
grep -q 'execq REPEAT FILE1' src/user_shell.c || fail 'execq help missing'
grep -q 'execmt FILE1.EXE' src/user_shell.c || fail 'execmt help missing'
[ -f src/execmt_task.c ] || fail 'execmt task source missing'
[ -f src/queue_tests/qpass.c ] || fail 'qpass source missing'
echo 'CHECK44 PASS'
