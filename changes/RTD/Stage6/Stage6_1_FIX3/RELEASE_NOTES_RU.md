# ToyOS v67 RTD — Stage 6.1 FIX3

## Цель

Исправить зависание периодической RT-задачи с минимальным периодом 10 ms, не возвращая опасное переключение контекста из IRQ0, когда interrupt frame относится к Ring 0.

## Наблюдаемая проблема

Команда:

`exec RTD.EXE SENSOR1.EXE 10 10 3`

после `F10` запускала `SENSOR1.EXE`, но в runtime был виден только первый `tick`, после чего задача переставала выводить следующие активации. Контекст задачи при этом внешне не обязательно был уничтожен.

## Точная причина

В Stage 6.1 FIX1 был введён правильный запрет context switch из IRQ0, если `f->cs & 3 == 0`, потому что такой frame является transient Ring-0 frame и его нельзя копировать как пользовательский контекст.

Но ветка делала ранний `return` до вызова `rt_release_jobs()`.

При этом `SYS_RT_WAIT` переводит задачу в `TASK_BLOCKED` до следующего абсолютного `rt_next_release_tick`. Следующий 10-ms IRQ0 обычно приходит, пока shell находится внутри `SYS_CONSOLE_READ`, то есть interrupt frame имеет Ring-0 CS. Из-за раннего return RT release не продвигался. Задача оставалась BLOCKED до следующего события, которое могло снова не дать безопасного user-mode dispatch.

## Исправление

В `sched_irq_tick()` при Ring-0 IRQ0 теперь:

1. считывается текущий `rt_time_ticks`;
2. вызывается `rt_release_jobs(now)`;
3. выполняется немедленный `return`;
4. context switch не выполняется;
5. transient Ring-0 frame не копируется в RT task context.

Таким образом временное состояние RT продолжает обновляться на каждом 10-ms IRQ0, но переключение остаётся разрешено только из безопасного Ring-3 пути (`SYS_CONSOLE_READ`, `SYS_RT_WAIT` или Ring-3 IRQ0).

## Что не изменено

- 50-Hz логические часы `SYS_TIMER_GET`;
- 100-Hz аппаратный RT timebase;
- `period/deadline/priority`;
- `SYS_RT_WAIT` ABI;
- F10-подготовка задач;
- ESC-завершение;
- shell prompt `toy0>`;
- обычный формат EXE1;
- EXECMT;
- существующие CR3/image/stack механизмы.

## Ожидаемый результат

`exec RTD.EXE SENSOR1.EXE 10 10 3` + `F10` должен выдавать непрерывные `SENSOR1: tick`, а `ESC` — штатно завершать задачу и возвращать `toy0>`.
