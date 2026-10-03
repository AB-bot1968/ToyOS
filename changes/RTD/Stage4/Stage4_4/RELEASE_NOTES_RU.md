# ToyOS v67 RTD Stage 4.4 — Release Notes

## Цель
Добавить отдельную наблюдаемость и диагностический сценарий фактического priority-preemption без изменения правил fixed-priority arbitration Stage 4.2.

## Изменения
1. Добавлен `SYS_RT_TRACE=41`. Он доступен только текущей Ring-3 RT-задаче и возвращает `task_id`, `priority`, `switches`, `dispatch_seq`, `state`.
2. В `struct sched_task` добавлен `rt_dispatch_seq`; глобальная последовательность увеличивается только в `rt_switch_to()`. Это не меняет выбор задачи.
3. `SENSOR1.EXE` и `SENSOR2.EXE` остаются обычными EXE1. Они печатают dispatch trace.
4. Для `period=deadline=1000 ms` и priority 3/7 включён контролируемый PRIORITY-PROBE. Low priority удерживает один системный тик, high priority — два; задача не вызывает `SYS_RT_WAIT` до конца burst. Это создаёт реальную ситуацию, в которой READY high-priority задача может быть выбрана вместо текущей low-priority.
5. Existing `rt_pick_ready()` не изменён в Stage 4.4.

## Совместимость
Не изменены `ESC`, `toy0>`, F10, `SYS_RT_WAIT`, обычный EXE1 loader, EXECMT и PIT 50 Hz.
