#!/bin/sh
set -eu
K=src/kernel.c
# v50 root cause regression: execmt_load_one must return image_end because
# sched_load_initial() clears the whole task record, including image_end.
grep -q 'execmt_load_one(const char\*name,uint32_t id,uint32_t\*entry_out,uint32_t\*image_end_out)' "$K"
grep -q 'execmt_load_one(name,i,&entry,&image_end)' "$K"
grep -q 'sched_tasks\[i\]\.image_end=image_end' "$K"
# The returned boundary must be assigned AFTER sched_load_initial().
python - <<'PY'
s=open("src/kernel.c",encoding="utf-8").read()
a=s.index("execmt_load_one(name,i,&entry,&image_end)")
b=s.index("sched_tasks[i].image_end=image_end",a)
c=s.index("sched_load_initial(&sched_tasks[i]",a)
assert c < b
print("v50 ordering check: PASS")
PY
# Keep the Ring-3 INT 80h syscall path and existing command surface.
grep -q 'idt_set(0x80,isr128,1)' "$K"
grep -q 'case SYS_CONSOLE_WRITE' "$K"
grep -q 'case SYS_EXECMT' "$K"
grep -q 'SYS_LS' src/user_shell.c
grep -q 'execq ' src/user_shell.c
grep -q 'execmt ' src/user_shell.c
printf '%s\n' 'v50 static checks: PASS'
