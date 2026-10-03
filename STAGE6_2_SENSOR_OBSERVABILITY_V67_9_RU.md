# ToyOS v67.9 — Stage 6.2: достоверная наблюдаемость RT-задач

## Цель

v67.9 продолжает v67.8 и делает вывод SENSOR1..SENSOR4 максимально информативным
без изменения существующих RT syscall ABI 1..46 и без изменения политики планировщика.

## Что измеряет ядро

Для каждой текущей RT-задачи ядро сохраняет:

- `release` — номинальный RT tick выпуска текущего job;
- `observed` — RT tick, на котором IRQ0 реально обработал release;
- `interval` — фактический интервал между двумя обработанными releases;
- `jitter` — `interval - requested_period`, со знаком;
- `late` — задержка обслуживания release относительно номинального release;
- `dispatch` — первый RT tick фактического запуска текущего job;
- `latency` — `dispatch - release`;
- `cpu` — накопленные RT ticks, в которых текущий job действительно был RUNNING;
- `wall` — время от первого dispatch до текущего RT tick; это wall/response span, а не CPU time;
- `deadline` и `miss` — абсолютный deadline и состояние miss;
- `skips` и `total_miss` — пропущенные releases и накопленные deadline misses.

Все значения используют единую RT-базу `rt_time_ticks` 100 Hz: 1 tick = 10 ms.
Таким образом, диагностический вывод не смешивает legacy 50 Hz `SYS_TIMER_GET` с RT clock.

## Новый syscall

Добавлен **SYS_RT_EXEC_INFO = 47**.

Это только диагностический read-only интерфейс. Существующие syscall 1..46 не меняются.
Возвращаются 14 `uint32_t`:

| Индекс | Поле |
|---:|---|
| 0 | job sequence |
| 1 | period ticks |
| 2 | nominal release |
| 3 | observed release |
| 4 | observed interval |
| 5 | release lateness |
| 6 | first dispatch |
| 7 | dispatch latency |
| 8 | CPU ticks consumed by current job |
| 9 | current RT tick |
| 10 | absolute deadline |
| 11 | current job missed flag |
| 12 | skipped releases |
| 13 | total missed deadlines |

## SENSOR1..SENSOR4

`src/rt_sensor_diag.c` теперь собирается четырьмя вариантами `RT_SENSOR_DIAG_ID=1..4`.
Все четыре программы идентичны по логике; отличается только имя в консоли.

Пример строки:

`SENSOR3 #17 release=170 observed=170 interval=1t/10ms jitter=0t/0ms late=0t dispatch=170 latency=0t cpu=1t/10ms wall=1t/10ms deadline=171 miss=0 skips=0 total_miss=0`

Важное различие:

- `cpu` — фактическое CPU-running время по RT scheduler accounting;
- `wall` — elapsed span после первого dispatch;
- поэтому `wall >= cpu` при вытеснениях, а равенство обычно означает отсутствие заметного вытеснения в данном job.

Точность ограничена текущей RT timebase: 10 ms. Это честное ограничение реализации,
а не псевдо-точность от пользовательского таймера.

## Максимальный тест четырёх RT задач

Рекомендуемая конфигурация с одной и той же периодикой и четырьмя уровнями priority:

```text
exec rtd.exe sensor1.exe 10 10 1
exec rtd.exe sensor2.exe 10 10 3
exec rtd.exe sensor3.exe 10 10 5
exec rtd.exe sensor4.exe 10 10 7
```

Это заполняет все четыре текущих RT slots и одновременно проверяет priority arbitration,
release interval, dispatch latency, CPU accounting, deadline misses и skipped releases.

Для повторяемого stress-теста можно запускать их с задержками или одинаковыми release windows,
а затем сопоставлять `release`, `observed`, `dispatch`, `cpu` и `wall` между четырьмя SENSOR.

## Case-insensitive invariant

Требование остаётся постоянным: команды shell и параметры команд, которые являются
идентификаторами/ключевыми словами, принимаются независимо от регистра ASCII.
Числовые значения и чувствительные данные пользователя не преобразуются.

## Проверка

Добавлен `check135.sh`.

Проверяется:

- наличие syscall 47 и kernel CPU accounting;
- сборка SENSOR1..SENSOR4;
- упаковка SENSOR3/SENSOR4 в FAT16;
- наличие всех диагностических полей;
- сохранение case-insensitive shell parser;
- freestanding i386 `-Werror` compilation kernel + четырёх SENSOR + RTD + JITTER;
- существующий hosted jitter arithmetic test.

Полная `build.sh` зависит от требуемого x86 W64DevKit/i686 toolchain и в этой среде
не считается выполненной только по факту успешной частичной компиляции.
