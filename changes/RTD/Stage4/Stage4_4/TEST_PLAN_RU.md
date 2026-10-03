# ToyOS v67 RTD Stage 4.4 — Test Plan

## A. Static/host tests
1. `sh check_rtd_stage4_4.sh`.
2. Проверить `Stage 4.4 priority-preemption model passed`.

## B. Runtime test — common release, different priority
Подготовить:
```text
exec RTD.EXE SENSOR1.EXE 1000 1000 3
exec RTD.EXE SENSOR2.EXE 1000 1000 7
F10
```
Ожидается, что первая наблюдаемая выборка `dispatch=` после общего старта относится к `priority=7`.

## C. Runtime test — direct preemption
1. Подготовить и запустить только SENSOR1:
```text
exec RTD.EXE SENSOR1.EXE 1000 1000 3
F10
```
2. Дождаться `SENSOR1: [PRIORITY-PROBE]` и хотя бы одного `SENSOR1: dispatch=`.
3. Пока SENSOR1 продолжает probe, подготовить:
```text
exec RTD.EXE SENSOR2.EXE 1000 1000 7
F10
```
4. Сравнивать `dispatch=`. Следующий scheduler dispatch при совместной готовности должен быть у `SENSOR2 priority=7`.

## D. Regression
После завершения задач ESC должен по-прежнему выводить `SENSORx: ESC -> stopped`, а shell — `toy0>`.
