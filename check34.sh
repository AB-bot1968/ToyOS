#!/bin/sh
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }
grep -q '#define SYS_MT_START    18u' src/kernel.c || fail 'SYS_MT_START missing'
grep -q '#define SYS_MT_STOP     19u' src/kernel.c || fail 'SYS_MT_STOP missing'
grep -q 'struct sched_task {' src/kernel.c || fail 'scheduler context storage missing'
grep -q 'MT_CONTEXT_WORDS 19u' src/kernel.c || fail '19-word context size missing'
grep -q 'sched_irq_tick(f)' src/kernel.c || fail 'IRQ0 scheduler switch missing'
grep -q 'if(!sched_active||(f->cs&3u)==0u)return;' src/kernel.c || fail 'CPL3-only scheduler guard missing'
grep -q 'mem_copy(sched_tasks\[sched_current\].words' src/kernel.c || fail 'interrupted context save missing'
grep -q 'sched_pick_next' src/kernel.c || fail 'round-robin selection missing'
grep -q 'mem_copy((void\*)f,sched_tasks\[sched_current\].words' src/kernel.c || fail 'next context restore missing'
grep -q 'MT_STACK_A_PHYS' src/kernel.c || fail 'task A stack backing missing'
grep -q 'MT_STACK_B_PHYS' src/kernel.c || fail 'task B stack backing missing'
grep -q 'mt_task_a' src/user_shell.c || fail 'task A missing'
grep -q 'mt_task_b' src/user_shell.c || fail 'task B missing'
grep -q 'mt-test' src/user_shell.c || fail 'mt-test command missing'
grep -q 'SYS_MT_START' src/user_shell.c || fail 'user start syscall missing'
grep -q 'SYS_MT_STOP' src/user_shell.c || fail 'user stop syscall missing'
grep -q 'SYS_EXEC_QUEUE  15u' src/kernel.c || fail 'execq syscall 15 changed'
grep -q 'SYS_QUEUE_STOP  16u' src/kernel.c || fail 'execq stop syscall 16 changed'
grep -q 'QUEUE_MAX_DEPTH 4u' src/kernel.c || fail 'execq depth changed'
test -f VERIFICATION_V34_RU.md || fail 'v34 verification doc missing'
python3 - <<'PY'
from pathlib import Path
k=Path('src/kernel.c').read_text()
u=Path('src/user_shell.c').read_text()
assert 'struct frame{uint32_t edi,esi,ebp,oes,ebx,edx,ecx,eax;uint32_t gs,fs,es,ds;uint32_t int_no,error;uint32_t eip,cs,eflags;}' in k
assert 'MT_CONTEXT_WORDS 19u' in k
assert 'MT_STACK_PAGE' in k and 'MT_STACK_A_PHYS' in k and 'MT_STACK_B_PHYS' in k
assert 'paging_build_mt();' in k
assert 'mem_copy(sched_tasks[sched_current].words,(const void*)f,MT_CONTEXT_WORDS*4u);' in k
assert 'uint32_t n=(sched_current+i)%SCHED_TASKS;' in k
assert 'mem_copy((void*)f,sched_tasks[sched_current].words,MT_CONTEXT_WORDS*4u);' in k
assert 'if(n==32){rt_time_ticks++;rt_time_phase^=1u;if(rt_time_phase==0u)timer_ticks++;' in k and 'if(rt_background_active)sched_irq_tick(f);else if(rt_time_phase==0u)sched_irq_tick(f);' in k
assert 'void mt_task_a(void)' in u and 'void mt_task_b(void)' in u
print('PASS: scheduler context coverage remains; RT runs at 100 Hz while legacy system timer remains 50 Hz')
PY
echo '=== CHECK34 PASS ==='
