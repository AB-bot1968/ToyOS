# Тест-план ToyOS v67 RTD Stage 6.1

## 1. Host focused test

Из корня проекта:

```sh
sh ./check_rtd_stage6_1.sh
```

Ожидается:

```text
RTD Stage 6.1 dual-rate timebase checks passed
```

Проверяются преобразования 1/10/11/20/40/100 ms, 32-bit boundary и модель «10 RT ticks = 5 system ticks».

## 2. Проверка старого 50 Hz системного таймера

Загрузить ToyOS и выполнить существующую проверку `pit-test`. Её критерий не изменён: `SYS_TIMER_GET` должен увеличиваться раз в 20 ms логического времени.

Можно также выполнить:

```text
syscall 3
```

и убедиться по выводу, что используется `50 Hz system`.

## 3. Проверка нового RT времени (100 Hz)

Запустить:

```text
exec RTD.EXE RTTIME.EXE 1000 1000 255
```

затем:

```text
F10
```

Ожидается:

```text
RTTIME: started ordinary EXE1 (ESC to stop) RT=100Hz/10ms SYSTEM=50Hz/20ms
RTTIME: rt_delta=1 system_delta=0 ...
RTTIME: rt_delta=1 system_delta=1 ...
RTTIME: rt_delta=1 system_delta=0 ...
...
RTTIME: 12 RT ticks observed; expected 12 RT ticks and about 6 system ticks
```

Наблюдаемый критерий: `rt_delta` должен быть 1 для каждого следующего аппаратного RT tick; `system_delta` появляется примерно на каждом втором RT tick.

## 4. Реальный 10 ms период

Подготовить:

```text
exec RTD.EXE SENSOR1.EXE 10 10 3
F10
```

У `SENSOR1` одна RT release должна соответствовать одному RT tick. Важно: `SYS_TIMER_GET` здесь не используется как источник периода; RT scheduler работает по `rt_time_ticks`.

## 5. Регрессия 20/40/100 ms

Повторить уже подтверждённые тесты:

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

Ожидается соответственно 2, 4 и 10 RT ticks между releases.

## 6. Проверка delay

Команда:

```text
exec RTD.EXE SENSOR1.EXE 10 10 3 30
```

не должна использовать старую 20 ms сетку: минимальный шаг задержки теперь 10 ms.

## 7. ESC

После запуска `SENSOR1` или `RTTIME` нажать ESC. Ожидается обычный уже проверенный путь:

```text
...: ESC -> stopped
toy0>
```

## 8. Регрессия EXECMT

Без активных RT-задач выполнить существующий `mt-test`. Его logical scheduler cadence остаётся 50 Hz.

## 9. Критерий принятия

Stage 6.1 считается успешным только если одновременно подтверждены:

- `SYS_TIMER_GET` остаётся 50 Hz;
- `SYS_RT_TIME_GET` даёт 10 ms RT ticks;
- 10 ms RT period реально достижим;
- 20/40/100 ms сохраняют ожидаемую кратность;
- F10/ESC/`toy0>` не регрессировали;
- EXECMT продолжает работать на 50 Hz cadence.
