# ToyOS v67 RTD Stage 5.1 FIX1 — подробный план тестирования

## 1. Host regression

Из корня проекта:

```sh
sh check_rtd_stage5_1.sh
sh check_rtd_stage5_1_fix1.sh
```

Ожидается:

```text
RTD Stage 5.1 absolute deadline checks passed
RTD Stage 5.1 FIX1 absolute-deadline immutability checks passed
```

## 2. Runtime: команда пользователя

Подготовить:

```text
exec RTD.EXE DEADLINE.EXE 1000 100 3
```

Запустить:

```text
F10
```

## 3. Что наблюдать

Первая строка:

```text
DEADLINE: started release=R deadline=D now=N missed=0
```

Для 50 Hz и `deadline=100 ms`:

```text
D = R + 5
```

Пока текущая job активна, все sample-строки должны сохранять одну и ту же пару `R/D`.

Пример нормального результата:

```text
DEADLINE: started release=100 deadline=105 now=100 missed=0
DEADLINE: sample release=100 deadline=105 now=101 missed=0
DEADLINE: sample release=100 deadline=105 now=103 missed=0
DEADLINE: sample release=100 deadline=105 now=106 missed=1
```

Здесь `now=106` уже после deadline, но `release=100` и `deadline=105` не меняются.

## 4. Что считалось ошибкой в старой версии

Неправильно:

```text
release=100 deadline=105 now=101
release=100 deadline=105 now=103
release=150 deadline=155 now=151
```

Последняя строка уже показывает следующую job до явного завершения текущей job.

## 5. Проверка miss

После sample, пересёкшего deadline, `missed=` должен стать `1`, но пара `release/deadline` текущей job должна остаться неизменной.

Затем `SYS_RT_WAIT` должен вернуть `1`.

## 6. ESC

Во время диагностики нажать:

```text
ESC
```

Ожидается:

```text
DEADLINE: ESC -> stopped
toy0>
```

Этот тест подтверждает отсутствие регрессии механизма завершения.

## 7. Регрессия Stage 4.4

После проверки Stage 5.1 FIX1 повторить:

```text
exec RTD.EXE SENSOR1.EXE 20 20 3
exec RTD.EXE SENSOR2.EXE 20 20 7
F10
```

Обе задачи должны продолжать работать, а порядок dispatch должен оставаться таким же, как в Stage 4.4.
