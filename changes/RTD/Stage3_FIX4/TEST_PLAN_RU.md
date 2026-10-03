# ToyOS v67 RTD Stage 3 FIX4 — план проверки

## Цель

Проверить две независимые характеристики:

1. shell остаётся интерактивным, пока RT-задача работает;
2. RT-программа не потребляет обычные клавиши shell.

## Тест 1 — запуск одной RT-задачи

Команда:

```text
exec RTD.EXE SENSOR.EXE 20 20 3
```

Ожидается:

```text
RTD: started SENSOR.EXE ...
toy0>
SENSOR: tick
...
```

Критерий: система не должна зависать.

## Тест 2 — ввод символов shell при работающем SENSOR

Пока `SENSOR: tick` продолжает выводиться, ввести:

```text
help
```

Ожидается, что символы `h`, `e`, `l`, `p`, `Enter` поступят в shell, а не потеряются в `SENSOR.EXE`.

## Тест 3 — запуск второй RT-задачи

Без остановки первой задачи выполнить:

```text
exec RTD.EXE SENSOR.EXE 40 40 3
```

Критерий: первая RT-задача остаётся активной, а вторая запускается без `start FAIL`.

## Тест 4 — ESC

Для работающей RT-сессии нажать `ESC`.

Ожидается:

```text
SENSOR: ESC -> stopped
toy0>
```

и shell должен продолжить принимать команды.

## Тест 5 — компиляция

Обязательные компоненты:

- `src/kernel.c`
- `src/user_shell.c`
- `src/rtd.c`
- `src/rt_sensor.c`

Компиляция: 32-bit freestanding.

## Тест 6 — regression

Обязательны:

```text
check_rtd_stage1.sh
check_rtd_stage1_fix3.sh
check_rtd_stage1_fix6.sh
check_rtd_stage2.sh
check_rtd_stage3.sh
check_rtd_stage3_fix1.sh
check_rtd_stage3_fix2.sh
check_rtd_stage3_fix3.sh
check_rtd_stage3_fix4.sh
```
