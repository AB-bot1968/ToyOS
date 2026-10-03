# ToyOS v67 RTD Stage 2 FIX2

## Исправление EXC 14 после ESC

Исправлена ошибка контекста shell при завершении detached RT-задачи.

### Причина

IRQ0 мог приходить в тот момент, когда shell находился внутри `SYS_CONSOLE_READ` в Ring 0. Ветка RT-планировщика ошибочно копировала такой interrupt frame в `rt_shell_frame`. У такого frame `CS=0x08`, поэтому после `SENSOR.EXE -> SYS_EXIT` ядро могло восстановить kernel-mode frame и получить `EXC 14` при `IRET`.

### Исправление

IRQ0 больше не перезаписывает `rt_shell_frame`. Корректный shell frame сохраняется непосредственно в `SYS_CONSOLE_READ` перед передачей CPU RT-задаче и используется до завершения RT-задачи.

Механизм ESC, `toy0>` и обычный EXE1 не изменены.
