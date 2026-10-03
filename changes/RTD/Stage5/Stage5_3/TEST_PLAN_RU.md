# ToyOS v67 RTD Stage 5.3 — план тестирования

## 1. Host unit-test

Из корня проекта:

```sh
sh ./check_rtd_stage5_3.sh
```

Он проверяет:

- несколько пропущенных периодов;
- завершение точно на границе release;
- один пропущенный release;
- переход `0xffffffff -> 0`;
- freestanding-компиляцию kernel и `RTDSKIP.EXE`.

## 2. Базовый runtime-тест

```text
exec RTD.EXE RTDSKIP.EXE 20 20 3
F10
```

Ожидается диагностический вывод с `job=1`, абсолютными `release/deadline` и увеличивающимся `now`.

После нескольких системных тиков программа печатает `before-wait`, затем вызывает `SYS_RT_WAIT`.

При `period=20 ms` один период равен одному PIT tick. Поскольку диагностическая работа длится пять tick, после завершения должны быть пропущены несколько номинальных release.

Ожидаемый характер вывода:

```text
RTDSKIP: ... job=1 release=R deadline=R+1 ...
RTDSKIP: ... before-wait ... skipped=0
RTDSKIP: completed with deadline miss
RTDSKIP: ... next-job job=2 release>now ... skipped=5
```

Конкретные номера tick зависят от момента F10 и диспетчеризации.

## 3. Проверка отсутствия backlog

Новая `release` после завершения должна быть **первой в будущем**, а не старой release + один период независимо от текущего времени.

Например, если:

```text
period = 1 tick
previous release = 100
completion now = 105
```

ожидается:

```text
skipped = 5
next release = 106
```

а не последовательное создание job для release 101, 102, 103, 104, 105.

## 4. Проверка точной границы

Если job завершается ровно в момент следующего release, этот release **не пропускается**.

Пример:

```text
previous release = 100
period = 5
completion now = 105
```

ожидается:

```text
skipped = 0
next release = 105
job 2 может стать READY немедленно
```

## 5. Проверка нескольких периодов

Рекомендуется повторить:

```text
exec RTD.EXE RTDSKIP.EXE 20 20 3
F10
```

и затем аналогично:

```text
exec RTD.EXE RTDSKIP.EXE 100 100 3
F10
```

В первом тесте должно быть несколько skipped releases; во втором при пятитиковом периоде и пятитиковом/длиннее workload число пропусков будет меньше или равно, в зависимости от фактического времени завершения.

## 6. Проверка ESC

После диагностического запуска:

```text
ESC
```

должны сохраняться уже подтверждённые:

```text
RTDSKIP: ESC -> stopped
toy0>
```

Stage 5.3 не должен менять механизм завершения задачи.
