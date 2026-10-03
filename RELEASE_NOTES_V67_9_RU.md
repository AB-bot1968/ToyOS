# ToyOS v67.9 — Release Notes

Основа: **ToyOS v67.8 — Stage 6.2 RT Jitter**.

## Изменения

1. Добавлен read-only `SYS_RT_EXEC_INFO = 47` для расширенной RT-диагностики.
2. Добавлен отдельный kernel accounting `rt_job_cpu_ticks` для текущего RT job.
3. SENSOR1..SENSOR2 расширены до полного timing output.
4. Добавлены **SENSOR3.EXE** и **SENSOR4.EXE**.
5. Сборка и FAT16 image теперь включают все четыре SENSOR.
6. Добавлен `check135.sh`.
7. Существующие syscall 1..46 и scheduler policy не изменены.
8. Постоянное требование case-insensitive command input сохранено.

## Ограничение точности

Текущая RT clock = 100 Hz, поэтому диагностическая точность составляет 10 ms.
`cpu` — scheduler CPU-running ticks; `wall` — elapsed dispatch-to-now span.
Не следует трактовать `wall` как чистое CPU time.

## Проверка

Targeted freestanding i386 compilation выполнена с `-Werror` для kernel,
SENSOR1..4, RTD и JITTER. `tools/test_rt_jitter.c` также проходит.
Полный W64DevKit build не заявляется в Linux x86_64 среде.
