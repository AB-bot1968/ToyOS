# ToyOS RTD Stage 5.2 FIX2 — результаты проверки

Проверено:

- `rt_recount_active()` считает только READY/RUNNING/BLOCKED;
- `TASK_EXIT` и `TASK_STOPPED` не учитываются как активные;
- `SYS_RT_STATUS=43` добавлен в ABI ядра;
- `RTD.EXE` выводит `active/free` для `-1`;
- Stage 5.2 FIX1 integration model проходит;
- Stage 4.4 priority-preemption checks проходят;
- изменённые `kernel.c` и `rtd.c` проходят 32-bit freestanding compile.

Ограничение: фактический boot/runtime QEMU в текущем окружении отсутствует.
