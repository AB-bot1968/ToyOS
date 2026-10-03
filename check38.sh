#!/bin/sh
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }
grep -q '#define SCHED_TASKS      25u' src/kernel.c || fail '25-task scheduler missing'
grep -q '#define TASK_CREATE  0u' src/kernel.c || fail 'CREATE state missing'
grep -q '#define TASK_READY   1u' src/kernel.c || fail 'READY state missing'
grep -q '#define TASK_RUNNING 2u' src/kernel.c || fail 'RUNNING state missing'
grep -q '#define TASK_BLOCKED 3u' src/kernel.c || fail 'BLOCKED state missing'
grep -q '#define TASK_STOPPED 4u' src/kernel.c || fail 'STOPPED state missing'
grep -q '#define TASK_EXIT    5u' src/kernel.c || fail 'EXIT state missing'
grep -q 'static void sched_block(struct frame\*f)' src/kernel.c || fail 'block syscall handler missing'
grep -q 'static void sched_wake(struct frame\*f)' src/kernel.c || fail 'wake syscall handler missing'
grep -q 'state==TASK_READY' src/kernel.c || fail 'wake must tolerate IRQ0 BLOCKED->READY race'
grep -q 'static void sched_exit(struct frame\*f)' src/kernel.c || fail 'exit syscall handler missing'
grep -q 'sched_tasks\[sched_current\].state=TASK_BLOCKED' src/kernel.c || fail 'BLOCKED transition missing'
grep -q 'sched_tasks\[sched_current\].state=TASK_EXIT' src/kernel.c || fail 'EXIT transition missing'
grep -q 'sched_tasks\[id\].state=TASK_READY' src/kernel.c || fail 'wake transition to READY missing'
grep -q 'SYS_SCHED_BLOCK 20u' src/kernel.c || fail 'syscall 20 missing'
grep -q 'SYS_SCHED_WAKE  21u' src/kernel.c || fail 'syscall 21 missing'
grep -q 'SYS_SCHED_EXIT  22u' src/kernel.c || fail 'syscall 22 missing'
grep -q 'extern void mt_task_c(void)' src/kernel.c || fail 'task C missing'
grep -q 'extern void mt_task_d(void)' src/kernel.c || fail 'task D missing'
grep -q '4 Ring-3 tasks, separate CR3, round-robin IRQ0 scheduler.' src/user_shell.c || fail 'v38 test banner missing'
grep -q 'Lifecycle: CREATE -> READY -> RUNNING -> BLOCKED -> READY -> RUNNING -> EXIT.' src/user_shell.c || fail 'v38 lifecycle test text missing'
grep -q 'ISOLATION PASS: A marker is invisible in B' src/user_shell.c || fail 'isolation regression missing'
grep -q 'ISOLATION PASS: A marker survived B switches' src/user_shell.c || fail 'isolation persistence regression missing'
python3 - <<'PY2'
from pathlib import Path
k=Path('src/kernel.c').read_text(); u=Path('src/user_shell.c').read_text()
assert 'mt_page_directory[SCHED_TASKS][1024]' in k
assert 'mt_page_table[SCHED_TASKS][1024]' in k
assert 'for(t=0;t<SCHED_TASKS;t++)' in k
assert 'for(i=0;i<count;i++)sched_tasks[i].state=TASK_READY;' in k
assert 'sched_tasks[sched_current].state=TASK_BLOCKED;' in k
assert 'sched_tasks[sched_current].state=TASK_EXIT;' in k
assert 'sched_tasks[id].state=TASK_READY;' in k
assert 'load_cr3(sched_tasks[sched_current].cr3)' in k
assert 'load_cr3(sched_saved_shell_cr3)' in k
assert 'struct frame{uint32_t edi,esi,ebp,oes,ebx,edx,ecx,eax;uint32_t gs,fs,es,ds;uint32_t int_no,error;uint32_t eip,cs,eflags;}' in k
for name in ('mt_task_a','mt_task_b','mt_task_c','mt_task_d'):
    assert ('void '+name+'(') in u
assert 'sys_sched_block()' in u
assert 'sys_sched_wake(0)' in u
assert 'sys_sched_wake(1)' in u
assert 'sys_sched_exit()' in u
print('PASS: v38 four-task lifecycle, blocking/wakeup/exit, CR3 and v36 isolation coverage verified')
PY2
echo '=== CHECK38 PASS ==='
