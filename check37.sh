#!/bin/sh
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }
grep -q '#define SCHED_TASKS      25u' src/kernel.c || fail 'scheduler task count missing'
grep -q '#define TASK_CREATE  0u' src/kernel.c || fail 'CREATE state missing'
grep -q '#define TASK_READY   1u' src/kernel.c || fail 'READY state missing'
grep -q '#define TASK_RUNNING 2u' src/kernel.c || fail 'RUNNING state missing'
grep -q '#define TASK_BLOCKED 3u' src/kernel.c || fail 'BLOCKED state missing'
grep -q '#define TASK_STOPPED 4u' src/kernel.c || fail 'STOPPED state missing'
grep -q '#define TASK_EXIT    5u' src/kernel.c || fail 'EXIT state missing'
grep -q 'struct sched_task {' src/kernel.c || fail 'scheduler task structure missing'
grep -q 'uint32_t cr3;' src/kernel.c || fail 'per-task CR3 missing'
grep -q 'uint32_t state;' src/kernel.c || fail 'per-task state missing'
grep -q 'uint32_t switches;' src/kernel.c || fail 'per-task switch counter missing'
grep -q 'static int sched_pick_next(void)' src/kernel.c || fail 'runnable-task selection missing'
grep -q 'sched_tasks\[sched_current\].state=TASK_READY' src/kernel.c || fail 'preempted task is not returned to READY'
grep -q 'sched_tasks\[sched_current\].state=TASK_BLOCKED' src/kernel.c || fail 'BLOCKED state transition missing'
grep -q 'sched_tasks\[sched_current\].state=TASK_EXIT' src/kernel.c || fail 'EXIT state transition missing'
grep -q 'sched_tasks\[sched_current\].state=TASK_RUNNING' src/kernel.c || fail 'selected task is not RUNNING'
grep -q 'load_cr3(sched_tasks\[sched_current\].cr3)' src/kernel.c || fail 'scheduler CR3 load missing'
grep -q 'load_cr3(sched_saved_shell_cr3)' src/kernel.c || fail 'shell CR3 restore missing'
grep -q 'sched_saved_shell_cr3=read_cr3()' src/kernel.c || fail 'shell CR3 save missing'
grep -q 'sched_irq_tick(f)' src/kernel.c || fail 'IRQ0 scheduler hook missing'
grep -q 'struct frame{uint32_t edi,esi,ebp,oes,ebx,edx,ecx,eax;uint32_t gs,fs,es,ds;uint32_t int_no,error;uint32_t eip,cs,eflags;}' src/kernel.c || fail 'architectural frame changed'
grep -q '4 Ring-3 tasks, separate CR3, round-robin IRQ0 scheduler.' src/user_shell.c || fail 'v38 scheduler test banner missing'
python3 - <<'PY'
from pathlib import Path
k=Path('src/kernel.c').read_text()
assert 'struct sched_task {' in k
assert 'uint32_t words[MT_CONTEXT_WORDS];' in k
assert 'uint32_t cr3;' in k
assert 'uint32_t state;' in k
assert 'uint32_t id;' in k
assert 'uint32_t switches;' in k
assert 'for(i=0;i<count;i++)sched_tasks[i].state=TASK_READY;' in k
assert 'sched_tasks[0].state=TASK_RUNNING;' in k
assert 'uint32_t n=(sched_current+i)%SCHED_TASKS;' in k
assert 'sched_tasks[sched_current].state=TASK_READY;' in k
assert 'sched_tasks[sched_current].state=TASK_BLOCKED;' in k
assert 'sched_tasks[sched_current].state=TASK_EXIT;' in k
assert 'sched_tasks[id].state=TASK_READY;' in k
assert 'sched_tasks[sched_current].state=TASK_RUNNING;' in k
assert 'sched_tasks[sched_current].switches++;' in k
assert 'sched_switches++;' in k
assert 'load_cr3(sched_tasks[sched_current].cr3)' in k
assert 'sched_saved_shell_cr3=read_cr3();' in k
assert 'load_cr3(sched_saved_shell_cr3)' in k
print('PASS: v37 scheduler state/context/CR3 structure verified')
PY
echo '=== CHECK37 PASS ==='
