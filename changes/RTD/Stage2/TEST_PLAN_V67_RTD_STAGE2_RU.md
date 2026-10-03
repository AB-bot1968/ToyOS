# ToyOS v67 — тест-план RT Stage 2

## Статические тесты

`check_rtd_stage2.sh` проверяет наличие RT_WAIT, release/deadline state, deadline accounting, priority budget и overflow-safe расчёт.

## Сборка

Обязательно компилируются `kernel.c`, `user_shell.c`, `rtd.c`, `rt_sensor.c` в 32-bit freestanding режиме.

## Runtime acceptance

1. `exec RTD.EXE SENSOR.EXE 20 20 3` — SENSOR печатает один job/tick на период и после каждого job вызывает RT_WAIT.
2. `exec RTD.EXE SENSOR.EXE 40 40 3` — интервалы job примерно 40 ms (2 PIT ticks).
3. `exec RTD.EXE SENSOR.EXE 100 100 3` — примерно 100 ms (5 PIT ticks).
4. `exec RTD.EXE SENSOR.EXE 20 20 255` — высокий priority получает максимально разрешённый budget; для SENSOR, завершающего job через RT_WAIT, результат остаётся периодическим.
5. `exec RTD.EXE SENSOR.EXE 20 1 3` — команда должна быть отвергнута существующей валидацией `deadline <= period`.
6. Нагрузка/искусственная job, не вызывающая RT_WAIT до deadline, должна привести к `deadline miss` и переходу к следующему release.
7. ESC во время ожидания SENSOR не должен ломать существующий механизм завершения и shell prompt.

PIT 50 Hz сохраняется; hard 10-ms scheduling на этом этапе не заявляется.
