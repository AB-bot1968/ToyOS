# ToyOS v67 RTD Stage 5.1 — подробный план тестирования

## A. Host unit-test

Из корня проекта:

```sh
sh check_rtd_stage5_1.sh
```

Ожидается:

```text
RTD Stage 5.1 absolute deadline checks passed
```

Тест дополнительно проверяет:

```text
1 ms   -> 1 tick
20 ms  -> 1 tick
21 ms  -> 2 ticks
100 ms -> 5 ticks
```

и абсолютную модель:

```text
release=100, deadline_interval=5 -> deadline=105
```

После `now=104` deadline ещё не достигнут.
При `now=105` и `now=106` deadline считается достигнутым.

Отдельно проверяется wrap:

```text
release=0xfffffff0
+32 ticks
=0x00000010
```

## B. Сборка EXE1

После штатной сборки должен появиться:

```text
build/DEADLINE.EXE
```

Проверка FAT16:

```sh
./build/fat16check.exe build/disk.img DEADLINE.EXE
```

Ожидается `FAT16 CHECK OK`.

EXE1 должен иметь тот же формат, что и остальные пользовательские программы:

```text
magic = EXE1
entry = 0x00100000
```

## C. Runtime — нормальная job до deadline

Подготовить задачу:

```text
exec RTD.EXE DEADLINE.EXE 1000 100 3
```

Ожидается:

```text
RTD: task prepared: DEADLINE.EXE 1000 100 3
```

Запустить:

```text
F10
```

Ожидаемый первый вывод:

```text
DEADLINE: started release=R deadline=D now=N
```

где для данной job:

```text
D = R + 5 ticks
```

При последующих строках:

```text
DEADLINE: sample release=R deadline=D now=N1
DEADLINE: sample release=R deadline=D now=N2
DEADLINE: sample release=R deadline=D now=N3
```

`R` и `D` должны оставаться прежними, а `N1 < N2 < N3`.

Затем программа выполняет `SYS_RT_WAIT` до deadline.
Ожидается:

```text
DEADLINE: job completed before deadline
```

## D. Проверка, что deadline не сдвигается из-за dispatch

Запустить:

```text
exec RTD.EXE DEADLINE.EXE 1000 100 3
F10
```

Сравнить три sample-строки.

Неправильный результат выглядел бы как:

```text
release=R deadline=R+5
release=R deadline=N1+5
release=R deadline=N2+5
```

Такого быть не должно.

Правильный результат:

```text
release=R deadline=R+5
release=R deadline=R+5
release=R deadline=R+5
```

## E. Проверка timer wrap

Host unit-test покрывает арифметический wrap. В runtime-контрольном boot-тесте
ручное ожидание полного `uint32_t` timer wrap не требуется: это занимает слишком
много времени при 50 Hz. Поэтому wrap является unit-test уровнем Stage 5.1.

## F. Регрессия RTD

После сборки должны пройти все focused-тесты Stage 1–4.4 и Stage 5.1:

```sh
for f in \
  check_rtd_stage1.sh \
  check_rtd_stage1_fix3.sh \
  check_rtd_stage1_fix6.sh \
  check_rtd_stage2.sh \
  check_rtd_stage3.sh \
  check_rtd_stage3_fix1.sh \
  check_rtd_stage3_fix2.sh \
  check_rtd_stage3_fix3.sh \
  check_rtd_stage3_fix4.sh \
  check_rtd_stage3_f10.sh \
  check_rtd_stage4_1.sh \
  check_rtd_stage4_2.sh \
  check_rtd_stage4_3.sh \
  check_rtd_stage4_4.sh \
  check_rtd_stage5_1.sh; do
    sh "$f" || exit 1
done
```

## G. Регрессия стабильной функции SENSOR

Проверить отдельно:

```text
exec RTD.EXE SENSOR.EXE 20 20 3
```

Затем несколько `tick`, после чего:

```text
ESC
```

Ожидается уже подтверждённое поведение:

```text
SENSOR: ESC -> stopped
toy0>
```

Этот тест является обязательным регрессом, но код `SENSOR.EXE` для Stage 5.1
не менялся.

## H. Что не является критерием Stage 5.1

`period=10 ms` не тестируется как настоящий 10-ms период: PIT остаётся 50 Hz.
Настройка 10 ms будет отдельной задачей с отдельным test plan.
