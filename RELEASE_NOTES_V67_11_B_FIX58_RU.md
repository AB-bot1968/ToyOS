# ToyOS v67.11-B FIX58 — SONARDRV

Основа: FIX57 MODBUS RTU RING3 CURRENT STABLE.

Добавлен `SONARDRV.EXE` — Ring3-служба эхолота. Производственный тракт: FIX55 UART transport -> FIX57 Modbus RTU Function 04 -> 6 input registers -> X/Y/Z signed int32 mm -> FIX56 Sensor/Data Channel 0.

Формат координаты: два 16-битных регистра, старшее слово первым. Плавающая точка не используется.

Heartbeat публикуется только после полностью успешного цикла: корректный Modbus response/CRC, decode XYZ и публикация sample. Поэтому запуск процесса без связи с датчиком не подтверждает recovery.

Добавлен общий `sonar_core.c` и автоматический `TESTSON.TST`. Тест использует тот же production decoder и Modbus parser, проверяя положительные/отрицательные XYZ, partial response, bad CRC, Modbus exception и неверное число регистров без внешнего COM.

Syscall 13/14 и политика доступа Ring3 к I/O-портам не изменены. Новых syscall FIX58 не добавляет.
