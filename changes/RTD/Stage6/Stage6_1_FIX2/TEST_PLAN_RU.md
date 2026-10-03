# План тестирования — Stage 6.1 FIX2

## 1. Статический focused-test

Из корня проекта:

    sh ./check_rtd_stage6_1_fix2.sh

Ожидается:

    RTD Stage 6.1 FIX2 focused checks passed

Проверяется:

- наличие fallback `rt_pick_ready_missed()` внутри `SYS_CONSOLE_READ`;
- порядок выбора: ON-TIME сначала, MISSED только при отсутствии ON-TIME;
- Stage 6.1 FIX1 запрет переключения из Ring 0 сохранён;
- `rt_sensor_diag.c` использует 100-Hz RT clock;
- 32-bit freestanding-компиляция kernel и SENSOR1/SENSOR2;
- регрессия Stage 4.4 и предыдущих Stage 5/6 checks.

## 2. Запуск одной задачи 10 ms

Подготовить:

    exec RTD.EXE SENSOR1.EXE 10 10 3

Запустить:

    F10

Ожидается:

    RTD: task 1 started
    SENSOR1: started ordinary EXE1 ... period=10ms ...
    SENSOR1: tick
    SENSOR1: tick
    SENSOR1: tick

Допускается, что первый запуск уже имеет `deadline miss`, если shell успел
дойти до RT-задачи позже её 10-ms absolute deadline. Это не является отказом:
job должна быть запущена как best-effort и продолжить работу.

## 3. Проверка периода

Использовать:

    exec RTD.EXE SENSOR1.EXE 10 10 3
    F10

Счётчик RT dispatch/release должен изменяться с шагом 1 RT tick = 10 ms.
В диагностическом SENSOR1 строки `tick` теперь используют тот же RT clock.

## 4. Завершение

Во время выполнения:

    ESC

Ожидается:

    SENSOR1: ESC -> stopped
    toy0>

## 5. Регрессия 20/40/100 ms

Повторить рабочие сценарии:

    exec RTD.EXE SENSOR1.EXE 20 20 3
    F10

    exec RTD.EXE SENSOR1.EXE 40 40 3
    F10

    exec RTD.EXE SENSOR1.EXE 100 100 3
    F10

Поведение должно оставаться прежним.
