# ToyOS v67 RTD Stage 4.3 — SENSOR1/SENSOR2 для наблюдения параллельного RT

## Цель

Stage 4.3 добавляет два диагностических бинарника `SENSOR1.EXE` и `SENSOR2.EXE`.
Они нужны для наблюдения нескольких одновременно работающих RT-задач и для
проверки ранее реализованного priority arbitration.

## Пошаговые изменения

1. Добавлен единый исходник `src/rt_sensor_diag.c`.
2. Исходник компилируется два раза с `RT_SENSOR_DIAG_ID=1` и `=2`.
3. Получаются два независимых обычных EXE1: `SENSOR1.EXE` и `SENSOR2.EXE`.
4. `SENSOR1.EXE` выводит строки с префиксом `SENSOR1:`.
5. `SENSOR2.EXE` выводит строки с префиксом `SENSOR2:`.
6. Оба бинарника используют только существующие syscall `SYS_RT_INFO`,
   `SYS_CONSOLE_POLL`, `SYS_RT_WAIT` и `SYS_EXIT`.
7. Добавлены отдельные записи сборки и FAT16 для обоих файлов.
8. Добавлен `check_rtd_stage4_3.sh` и host-test
   `tools/test_rtd_stage4_3.c`.

## Что не меняется

`rt_priority_compare()` и `rt_pick_ready()` остаются реализацией Stage 4.2.
Stage 4.3 не меняет scheduler, F10, ESC, shell context, `SYS_RT_WAIT`,
period/deadline или EXE1 ABI.
