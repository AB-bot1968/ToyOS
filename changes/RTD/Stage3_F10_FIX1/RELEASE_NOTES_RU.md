# ToyOS v67 RTD Stage 3 F10 FIX1 — исправление EXC 14 после возврата в text mode

## База

Базовая версия: `ToyOS v67 RTD Stage 3 F10`.

Уже подтверждённые функции не изменялись: обычный EXE1 `SENSOR.EXE`, RT `period/deadline/priority`, `SYS_RT_WAIT`, ESC-завершение, возврат `toy0>`, очередь RT-команд и запуск по F10.

## Симптом

Сразу после возврата из `VGADRV.EXE` в текстовый режим shell получал:

`EXC 14 EIP=71016 CS=27 ERR=7 CR2=319488`

`CR2=319488 = 0x4E000`. Ошибка происходила в Ring-3 (`CS=27`) при записи в пользовательское состояние shell.

## Корневая причина

В Stage 3 F10 были добавлены глобальные изменяемые объекты shell:

- `pending_rt_args`;
- `pending_rt_count`;
- `shell_f10_event`.

Они оставались в обычной `.bss` ядра. Page table ToyOS намеренно не даёт Ring-3 доступ к обычной kernel `.bss`, поэтому запись shell в эти объекты вызывала #PF с `ERR=7`.

## Исправление шаг за шагом

1. Изменяемые глобальные данные F10/RTD помещены в отдельную секцию `.userdata`.
2. В `liker.ld` добавлена отдельная loadable output section `.udata`, выровненная на страницу.
3. В kernel добавлены границы `__user_data_start/__user_data_end`.
4. Добавлена `paging_mark_user_rw()`, которая даёт только этой области `P|RW|US`.
5. В `paging_init()` новая область мапится как user-writable.
6. `user_range()` и `user_rw_range()` знают о новой области для корректной проверки syscall-указателей.
7. `build.sh` теперь включает `.udata` в `kernel.bin`.
8. Существующая scheduler/RT page-table логика не изменена: `.userdata` относится только к foreground shell.

## Результат

Адреса F10/RTD state теперь находятся в явно user-доступной RW-области, а обычная kernel `.bss` остаётся supervisor-only.
