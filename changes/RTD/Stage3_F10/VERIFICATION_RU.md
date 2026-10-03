# ToyOS v67 RTD Stage 3 F10 — результаты статической проверки

## Компиляция

Проверены 32-bit freestanding:

- `src/kernel.c`
- `src/user_shell.c`
- `src/rtd.c`
- `src/rt_sensor.c`

## Focused checks

`check_rtd_stage3_f10.sh` проверяет:

- F10 Set-1 scancode `0x44`;
- внутреннее событие `KEY_EVENT_F10`;
- возврат `4` из `SYS_CONSOLE_READ`;
- обработку события в `readline()` и `shell_loop()`;
- очередь из максимум четырёх RT-заявок;
- отсутствие немедленного `SYS_EXEC_ARGS` в `exec_rtd_command()`;
- штатный запуск заявок через `rt_launch_pending()`.

## Ограничение

Фактический runtime boot не проверяется в текущем контейнере, поскольку `qemu-system-i386` отсутствует.
