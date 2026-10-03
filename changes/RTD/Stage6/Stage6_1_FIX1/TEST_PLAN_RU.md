# Тест-план Stage 6.1 FIX1

## 1. Автоматическая проверка

```sh
sh ./check_rtd_stage6_1_fix1.sh
```

Ожидается:

```text
Stage 6.1 FIX1 focused checks passed
```

Проверяется наличие Ring-0 guard до `rt_release_jobs()`, 32-bit freestanding compile и все RTD-регрессии Stage 4.1–5.3 + Stage 6.1.

## 2. Основной runtime-тест 10 ms

```text
exec RTD.EXE SENSOR1.EXE 10 10 3
```

Нажать `F10`.

Ожидается:

```text
SENSOR1: started ordinary EXE1 period=10ms deadline=10ms priority=3
SENSOR1: tick
SENSOR1: tick
SENSOR1: tick
...
```

Задача не должна завершаться после первого `tick` и не должна зависать.

## 3. Проверка ESC

Во время работы нажать `ESC`.

Ожидается:

```text
SENSOR1: ESC -> stopped
toy0>
```

## 4. Регрессия 20/40/100 ms

Повторить:

```text
exec RTD.EXE SENSOR1.EXE 20 20 3
F10
```

```text
exec RTD.EXE SENSOR1.EXE 40 40 3
F10
```

```text
exec RTD.EXE SENSOR1.EXE 100 100 3
F10
```

## 5. Две задачи

```text
exec RTD.EXE SENSOR1.EXE 10 10 3
exec RTD.EXE SENSOR2.EXE 20 20 7
F10
```

Обе задачи должны выдавать диагностический вывод; завершение одной через ESC не должно повреждать вторую.
