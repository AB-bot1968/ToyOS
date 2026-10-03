# ToyOS RTD Stage 5.2 FIX1 — подробный план тестирования

## Тест A — одиночная deadline miss

Подготовить:

`exec RTD.EXE RTDMISS.EXE 1000 40 7`

`F10`

Ожидается:

`release` и `deadline` фиксированы для текущей job.

После превышения срока появляется:

`RTDMISS: job missed -> best-effort`

## Тест B — проверка приоритета после miss

Подготовить:

`exec RTD.EXE RTDMISS.EXE 1000 40 7`

`exec RTD.EXE SENSOR1.EXE 20 20 3`

`F10`

После появления `RTDMISS: job missed -> best-effort` должна наблюдаться dispatch-активность `SENSOR1` при его готовности, несмотря на `7 > 3`.

Это главный runtime-тест политики Stage 5.2.

## Тест C — best-effort fallback

Когда `SENSOR1` находится в `SYS_RT_WAIT` и не является READY, `RTDMISS` может снова получить CPU. Это не ошибка: missed job не удаляется и продолжает работу как best-effort.

## Тест D — завершение

После выполнения тестовой job ожидается обычный `SYS_RT_WAIT`, затем `SYS_EXIT` и возврат `toy0>`.

## Host regression

В корне проекта:

`sh check_rtd_stage5_2_fix1.sh`

Должно быть:

`RTD Stage 5.2 FIX1 deadline-miss policy checks passed`

## Интерпретация наблюдаемой последовательности

Для сценария `RTDMISS priority=7` + `SENSOR1 priority=3` последовательность `RTDMISS → SENSOR1` после старта является нормальной: до пропуска deadline обе job on-time, поэтому действует priority.

После строки `RTDMISS: job missed -> best-effort` следующая READY on-time job (`SENSOR1`) обязана получить преимущество над RTDMISS. Если `SENSOR1` уходит в `SYS_RT_WAIT`, просроченная RTDMISS снова может получить CPU как best-effort fallback.

Именно эту последовательность моделирует `tools/test_rt_deadline_miss_integration.c`.
