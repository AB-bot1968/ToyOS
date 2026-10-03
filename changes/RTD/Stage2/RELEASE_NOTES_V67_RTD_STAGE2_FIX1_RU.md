# ToyOS v67 RTD Stage 2 FIX1

## Исправление EXC 14 после первого SYS_RT_WAIT

Исправлена потеря контекста RT-задачи при системном вызове `SYS_RT_WAIT`.

### Причина

`SYS_RT_WAIT` переводил RT-задачу в `TASK_BLOCKED` и заменял текущий interrupt frame на сохранённый frame shell, но не сохранял текущий frame RT-задачи в `sched_tasks[rt_task_id].words`.

После следующего release планировщик восстанавливал устаревший контекст RT-задачи. Это могло привести к повторному выполнению старого участка кода и к page fault (`EXC 14`).

### Исправление

Перед переключением на shell текущий `struct frame` теперь сохраняется в контекст detached RT-задачи. Это сохраняет точные `EIP`, `ESP`, сегментные регистры и `EFLAGS` для продолжения после следующего release.

Механизм ESC/завершения SENSOR и вывод `toy0>` не изменялись.

## Проверка

- `check_rtd_stage2_fix1.sh` — PASS
- 32-bit freestanding compile `kernel.c` — PASS
- 32-bit freestanding compile `rt_sensor.c` — PASS

Реальный boot/runtime в данном окружении не выполнялся: `qemu-system-i386` отсутствует.
