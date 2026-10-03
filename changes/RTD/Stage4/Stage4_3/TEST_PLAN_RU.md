# Тест-план Stage 4.3

## Шаг 1. Focused test

    sh check_rtd_stage4_3.sh

Ожидается:

    RTD Stage 4.3 diagnostic SENSOR1/SENSOR2 checks passed

## Шаг 2. Сборка

Запустить штатный:

    ./build.sh

В FAT16 root должны появиться `SENSOR1.EXE` и `SENSOR2.EXE`.

## Шаг 3. Подготовить две задачи

    exec RTD.EXE SENSOR1.EXE 20 20 3
    exec RTD.EXE SENSOR2.EXE 20 20 7

Обе команды только подготавливают задачи. После второй команды нажать:

    F10

## Шаг 4. Наблюдение

Ожидаются различимые строки:

    SENSOR1: tick
    SENSOR2: tick

При одинаковом period/deadline и `priority=7` против `priority=3` первый выбор
READY-задачи должен учитывать большее значение priority.

## Шаг 5. Остановка

ESC должен завершать выбранную RT-задачу. Соответствующая строка должна иметь
тот же префикс (`SENSOR1:` или `SENSOR2:`). Оставшаяся RT-задача должна продолжать
работать до своего ESC.
