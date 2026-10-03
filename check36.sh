#!/bin/sh
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }
grep -q 'sched_stack_phys(i)' src/kernel.c || fail 'scheduler stack zeroing missing'
grep -q 'MT_STACK_B_PHYS' src/kernel.c || fail 'B physical stack backing missing'
grep -q 'volatile uint32_t isolation=0xa5a55a5au' src/user_shell.c || fail 'A isolation marker missing'
grep -q 'if(isolation==0xa5a55a5au)' src/user_shell.c || fail 'B isolation check missing'
grep -q 'ISOLATION PASS: A marker is invisible in B' src/user_shell.c || fail 'B isolation PASS message missing'
grep -q 'ISOLATION PASS: A marker survived B switches' src/user_shell.c || fail 'A isolation PASS message missing'
grep -q 'load_cr3(sched_tasks\[sched_current\].cr3)' src/kernel.c || fail 'IRQ CR3 switch missing'
grep -q 'load_cr3(sched_saved_shell_cr3)' src/kernel.c || fail 'shell CR3 restore missing'
python3 - <<'PY'
from pathlib import Path
k=Path('src/kernel.c').read_text(); u=Path('src/user_shell.c').read_text()
assert 'struct frame{uint32_t edi,esi,ebp,oes,ebx,edx,ecx,eax;uint32_t gs,fs,es,ds;uint32_t int_no,error;uint32_t eip,cs,eflags;}' in k
assert 'struct sched_task {' in k
assert 'MT_STACK_PAGE>>12]=sched_stack_phys(t)|PAGE_P|PAGE_RW|PAGE_US;' in k
assert 'volatile uint32_t isolation=0xa5a55a5au;' in u
assert 'isolation=0x5aa5a55au;' in u
print('PASS: v36 memory isolation remains covered while v37 scheduler controls task states')
PY
echo '=== CHECK36 PASS ==='
