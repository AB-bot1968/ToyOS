#!/bin/sh
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }
grep -q 'static uint32_t mt_page_directory\[SCHED_TASKS\]\[1024\]' src/kernel.c || fail 'per-task page directories missing'
grep -q 'static uint32_t mt_page_table\[SCHED_TASKS\]\[1024\]' src/kernel.c || fail 'per-task page tables missing'
grep -q 'static void paging_build_mt(void)' src/kernel.c || fail 'MT page-space builder missing'
grep -q 'mt_page_directory\[t\]\[0\]' src/kernel.c || fail 'MT PDE mapping missing'
grep -q 'MT_STACK_A_PHYS' src/kernel.c || fail 'A physical stack missing'
grep -q 'MT_STACK_B_PHYS' src/kernel.c || fail 'B physical stack missing'
grep -q 'load_cr3(sched_tasks\[0\].cr3)' src/kernel.c || fail 'initial CR3 load missing'
grep -q 'load_cr3(sched_tasks\[sched_current\].cr3)' src/kernel.c || fail 'IRQ CR3 switch missing'
grep -q 'load_cr3(sched_saved_shell_cr3)' src/kernel.c || fail 'shell CR3 restore missing'
grep -q 'sched_saved_shell_cr3=read_cr3()' src/kernel.c || fail 'shell CR3 save missing'
grep -q 'All tasks share the same virtual stack address' src/kernel.c || fail 'same virtual stack design missing'
grep -q '4 Ring-3 tasks, separate CR3, round-robin IRQ0 scheduler.' src/user_shell.c || fail 'v38 scheduler test text missing'
grep -q 'round-robin IRQ0 scheduler' src/user_shell.c || fail 'scheduler test text missing'
grep -q 'SYS_EXEC_QUEUE  15u' src/kernel.c || fail 'execq syscall 15 changed'
grep -q 'SYS_QUEUE_STOP  16u' src/kernel.c || fail 'execq stop syscall 16 changed'
grep -q 'QUEUE_MAX_DEPTH 4u' src/kernel.c || fail 'execq depth changed'
python3 - <<'PY'
from pathlib import Path
k=Path('src/kernel.c').read_text()
assert 'struct frame{uint32_t edi,esi,ebp,oes,ebx,edx,ecx,eax;uint32_t gs,fs,es,ds;uint32_t int_no,error;uint32_t eip,cs,eflags;}' in k
assert 'struct sched_task {' in k
assert 'MT_CONTEXT_WORDS 19u' in k
assert 'mt_page_table[t][MT_STACK_PAGE>>12]=sched_stack_phys(t)|PAGE_P|PAGE_RW|PAGE_US;' in k
assert 'sched_saved_shell_cr3=read_cr3();' in k
assert 'load_cr3(sched_tasks[0].cr3);' in k
assert 'load_cr3(sched_tasks[sched_current].cr3);' in k
assert 'load_cr3(sched_saved_shell_cr3);' in k
print('PASS: v35/v37 use separate CR3/page tables with identical user VA layout; v37 adds scheduler state')
PY
echo '=== CHECK35 PASS ==='
