# ToyOS v67.11-B FIX60I — SONAR repeat / quiet background

Основа: FIX60H UART FULL POLL.

Исправлено два дефекта, обнаруженных при физическом циклическом тесте SONARDRV.EXE:

1. EXECMT является detached/background механизмом. Kernel scheduler больше не печатает асинхронную строку `[MTxx] RUNNING ...` из `sched_announce()`. Состояние MT остаётся доступно через MTSTATUS/MTSTAT/PS. Сам SONARDRV.EXE и до исправления не вызывал SYS_CONSOLE_WRITE.
2. UART transport op8 перед каждой Modbus-попыткой теперь очищает не только software RX ring, но и уже принятые байты physical 16550 RX FIFO. Drain ограничен UART_RX_CAP и не включает IRQ4/IER. Это не позволяет позднему хвосту предыдущего ответа стать началом следующего Modbus-кадра.

Не изменены: syscall ABI, Modbus RTU формат, Data Channel ABI, MT quantum 20 ms, RT priority, UART full-poll policy (IER=0).
