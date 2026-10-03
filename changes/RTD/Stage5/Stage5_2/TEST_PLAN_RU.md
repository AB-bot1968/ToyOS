# ToyOS v67 RTD Stage 5.2 — план тестирования

## 1. Host unit test

Из корня проекта:

```sh
sh check_rtd_stage5_2.sh
```

Ожидается:

```text
RTD Stage 5.2 deadline-miss best-effort checks passed
```

Проверяются два класса READY jobs:

```text
on-time
missed
```

и правило: любая on-time job выбирается раньше missed job, даже если у missed выше priority.

## 2. Одиночная просроченная job

Подготовить:

```text
exec RTD.EXE RTDMISS.EXE 1000 40 7
```

Запустить:

```text
F10
```

Ожидаемая последовательность — примерно:

```text
RTDMISS: started release=R deadline=D now=R
RTDMISS: sample release=R deadline=D now=R+1 missed=0
RTDMISS: sample release=R deadline=D now=R+2 missed=1
RTDMISS: job missed -> best-effort
...
RTDMISS: completed with deadline miss
```

При PIT 50 Hz `deadline=40 ms` соответствует 2 ticks.

Критические признаки корректности:

- `release` не изменился после miss;
- `deadline` не изменился после miss;
- `missed` стал ненулевым только один раз;
- job продолжила выполнение после miss;
- `SYS_RT_WAIT` вернул признак miss (`1`);
- система не зависла.

## 3. Проверка demotion относительно on-time задачи

Подготовить:

```text
exec RTD.EXE RTDMISS.EXE 1000 40 7
exec RTD.EXE SENSOR1.EXE 20 20 3
```

Запустить:

```text
F10
```

Ожидается, что до пропуска deadline задачи участвуют согласно существующему priority arbitration. После того как `RTDMISS` получает `missed=1`, следующая on-time READY job `SENSOR1` должна иметь преимущество над missed `RTDMISS`, несмотря на priority 7 против priority 3.

При этом `RTDMISS` не уничтожается и может продолжить работу в периоды, когда on-time RT job не готова.

## 4. Проверка нескольких missed jobs

Подготовить две диагностические задачи с одинаковым коротким deadline и разными priority:

```text
exec RTD.EXE RTDMISS.EXE 1000 40 3
exec RTD.EXE RTDMISS.EXE 1000 40 7
F10
```

После miss обе относятся к best-effort классу. Внутри этого класса должна действовать обычная сортировка Stage 4.2: `priority`, затем абсолютный `deadline`, затем slot.

## 5. ESC regression

В любой момент:

```text
ESC
```

Проверить сохранённый механизм:

```text
RTDMISS: ESC -> stopped
toy0>
```

Запрос deadline miss не должен менять механизм завершения задачи.

## 6. Ограничение текущего timebase

PIT остаётся 50 Hz. Поэтому минимальная аппаратная scheduler-сетка — 20 ms. Stage 5.2 не пытается создать 10-ms timer и не меняет значение `SYS_TIMER_GET`.
