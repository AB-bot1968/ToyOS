#!/bin/sh
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }
grep -q '#define EXECMT_MAX_TASKS 25u' src/kernel.c || fail 'EXECMT_MAX_TASKS=25 missing'
grep -q '#define SCHED_TASKS      25u' src/kernel.c || fail '25-task scheduler missing'
grep -q '#define SYS_EXECMT      23u' src/kernel.c || fail 'SYS_EXECMT 23 missing'
grep -q 'static int execmt_prepare(struct frame\*f\|static int execmt_prepare(uint32_t p,uint32_t count,struct frame\*f)' src/kernel.c || fail 'execmt prepare missing'
grep -q 'static int execmt_load_one' src/kernel.c || fail 'real EXE1 task loader missing'
grep -q 'EXECMT_IMAGE_BASE_PHYS' src/kernel.c || fail 'private physical image slots missing'
grep -q 'EXECMT_IMAGE_SLOT_SIZE' src/kernel.c || fail 'per-task image slot size missing'
grep -q 'paging_prepare_execmt_task' src/kernel.c || fail 'private image mapping preparation missing'
grep -q 'paging_set_task_image_user' src/kernel.c || fail 'task user image mapping missing'
grep -q 'boot_memory_size_value' src/kernel.c || fail 'memory-size guard missing'
grep -q '0xB8FF0u' src/kernel.c || fail 'kernel must read RAM size from protected VGA-adjacent slot 0xB8FF0'
grep -q '0x0ff0' src/boot.S || fail 'bootloader must store RAM size at VGA-adjacent slot 0xB8FF0'
grep -q 'load_cr3((uint32_t)mt_page_directory\[id\])' src/kernel.c || fail 'per-task CR3 image loading missing'
grep -q 'case SYS_EXECMT' src/kernel.c || fail 'SYS_EXECMT dispatcher missing'
grep -q 'sys_execmt' src/user_shell.c || fail 'shell syscall wrapper missing'
grep -q 'execmt_command' src/user_shell.c || fail 'execmt command missing'
grep -q 'execmt FILE1.EXE' src/user_shell.c || fail 'execmt help missing'
grep -q 'case SYS_EXIT:if(f->cs&3u){if(sched_active)sched_exit(f);else exe_exit(f);}' src/kernel.c || fail 'SYS_EXIT must terminate EXECMT scheduler tasks'
test -f src/execmt_task.c || fail 'EXECMT test source missing'
grep -q 'EXECMT_ID' src/execmt_task.c || fail 'compile-time task identity missing'
grep -q 'for i in `seq 1 25`' build.sh || fail 'build does not create 25 EXECMT images'
# FIX53 preserves sector accounting but replaces the fixed slot by descriptor-driven bounds.
grep -q 'KERNEL_SECTORS=' build.sh || fail 'kernel sector accounting missing'
grep -q 'KERNEL_LOAD_LIMIT' build.sh || fail 'dynamic kernel memory guard missing'
grep -q 'KERNEL_RESERVE' build.sh || fail 'dynamic kernel reserve missing'
python3 - <<'PY'
from pathlib import Path
k=Path('src/kernel.c').read_text(); u=Path('src/user_shell.c').read_text(); t=Path('src/execmt_task.c').read_text(); b=Path('build.sh').read_text()
assert '#define EXECMT_MAX_TASKS 25u' in k
assert '#define SCHED_TASKS      25u' in k
assert 'for(t=0;t<SCHED_TASKS;t++)' in k
assert 'sched_stack_phys(t)' in k
assert 'EXECMT_IMAGE_BASE_PHYS+(id*EXECMT_IMAGE_SLOT_SIZE)' in k
assert 'if(boot_memory_size_value()<EXECMT_IMAGE_BASE_PHYS+(count*EXECMT_IMAGE_SLOT_SIZE))' in k
assert '0xB8FF0u' in k
assert 'execmt: scheduler finished; all requested tasks EXIT, returning to shell' in Path('src/user_shell.c').read_text()
assert 'paging_prepare_execmt_task(i);' in k
assert 'sched_load_initial(&sched_tasks[i],entry,MT_STACK_TOP' in k
assert 'sched_tasks[i].state=TASK_READY' in k
assert 'sched_tasks[0].state=TASK_RUNNING' in k
assert 'load_cr3(sched_tasks[0].cr3)' in k
assert 'SYS_EXECMT      23u' in u
assert 'static uint32_t sys_execmt' in u
assert 'execmt_command' in u
assert 'EXECMT_ID' in t
assert 'SYS_SCHED_BLOCK' not in t and 'SYS_SCHED_WAKE' not in t and 'SYS_SCHED_EXIT' not in t
assert 'SYS_EXIT' in Path('src/queue_tests/qpass.c').read_text()
assert 'QPASS.EXE' in Path('src/queue_tests/qpass.c').read_text()
assert 'case SYS_EXIT:if(f->cs&3u){if(sched_active)sched_exit(f);else exe_exit(f);}' in k
assert 'for i in `seq 1 25`' in b
assert 'build/MT${mtid}.EXE' in b
assert 'KERNEL_SECTORS=' in b and 'KERNEL_LOAD_LIMIT' in b and 'KERNEL_RESERVE' in b
print('PASS: EXECMT 25-task lifecycle, private CR3/image slots, FIX17 minimal MT task and FIX53 dynamic kernel bounds verified')
PY
echo '=== CHECK39 PASS ==='
