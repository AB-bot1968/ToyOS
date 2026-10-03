# Test plan — RTD Stage 1 FIX5

## Focused static tests

`check_rtd_stage1_fix5.sh` проверяет:
- SENSOR сравнивает результат `SYS_CONSOLE_POLL` с ASCII ESC (`0x1B`);
- старой проверки `return == 1` для ESC нет;
- SENSOR получает `period_ms` через `SYS_RT_INFO`;
- RTD содержит отдельный `delay_ms` и расчёт задержки.

## Manual runtime test

1. `exec RTD.EXE SENSOR.EXE 20 20 3`
2. Убедиться, что идут `SENSOR: tick`.
3. Нажать ESC.
4. Ожидать `SENSOR: ESC -> stopped` и возврат управления shell.
5. Повторить с `exec RTD.EXE SENSOR.EXE 10 10 3 3000`.
6. Проверить, что сообщение `RTD: started ...` появляется примерно через 3 секунды,
   а после старта скорость SENSOR определяется `PERIOD_MS`, а не `DELAY_MS`.

QEMU runtime test в среде разработки не выполнялся, если `qemu-system-i386` отсутствует.
