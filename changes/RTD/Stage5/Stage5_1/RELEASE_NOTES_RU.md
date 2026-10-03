# ToyOS v67 RTD Stage 5.1 — абсолютный deadline одной RT job

## 1. Базовая версия

Stage 5.1 начинается строго от зафиксированной версии `ToyOS v67 RTD Stage 4.4`.
Существующие механизмы shell, F10, ESC, `SYS_RT_WAIT`, EXE1, EXECMT и fixed-priority
arbitration не заменяются.

## 2. Цель этапа

До Stage 5.1 поле `rt_deadline_tick` уже присутствовало, но модель не была оформлена
как отдельная проверяемая сущность «release -> абсолютный deadline». На этом этапе
каждая RT job получает собственный `rt_job_release_tick`, а `rt_deadline_tick`
строится только от этого release.

Формула:

```text
absolute_deadline = release_tick + ceil(deadline_ms / 20 ms)
```

20 ms — существующий период PIT 50 Hz.

## 3. Изменённые исходники

### `src/rt_deadline.h`

Новый общий header с тремя чистыми функциями:

- `rt_deadline_ms_to_ticks()` — перевод миллисекунд в целое число системных тиков с округлением вверх;
- `rt_deadline_reached()` — wrap-safe проверка `now >= target` в кольцевом пространстве `uint32_t`;
- `rt_deadline_absolute()` — формирование абсолютного deadline из release tick.

Header используется и kernel-кодом, и host unit-test, чтобы тест проверял ту же реализацию,
которая входит в kernel.

### `src/kernel.c`

Добавлено поле:

```c
uint32_t rt_job_release_tick;
```

Добавлены внутренние операции:

```c
rt_begin_job()
rt_advance_release()
```

`rt_begin_job()` фиксирует release текущей job и вычисляет deadline от него.

Проверки deadline переведены на `rt_deadline_reached()`, поэтому переход таймера через
`0xffffffff -> 0` не ломает сравнение.

Добавлен диагностический syscall:

```text
SYS_RT_DEADLINE_INFO = 42
```

Возвращает 5 слов:

```text
[0] release_tick
[1] absolute deadline_tick
[2] current timer_ticks
[3] period_ticks
[4] missed_deadlines
```

Существующий `SYS_RT_INFO=39` и его 32-байтовый ABI не менялись.

### `src/rt_deadline_diag.c`

Новый обычный EXE1 `DEADLINE.EXE` для runtime-проверки.
Он не имеет специального RT-формата: его загружает существующий `RTD.EXE` так же,
как `SENSOR.EXE`.

Программа печатает исходный release/deadline и несколько снимков состояния.
Это позволяет визуально проверить, что `deadline` остаётся неизменным, пока `now`
продвигается вперёд.

### `tools/test_rt_deadline.c`

Host unit-test общего timing header.
Проверяет округление 50 Hz, абсолютную формулу и timer wrap.

### `check_rtd_stage5_1.sh`

Focused regression для Stage 5.1: наличие API, привязка deadline к release,
новый EXE1, build-интеграция и компиляция.

### `build.sh`

Добавлены компиляция/линковка/установка `DEADLINE.EXE`, FAT16-проверка и запуск
`check_rtd_stage5_1.sh`.

### `README.md`

Добавлено описание Stage 5.1 и практический runtime-тест.

## 4. Что намеренно не изменено

- PIT остаётся 50 Hz;
- `period/deadline/priority` command ABI не изменился;
- F10 queue не изменена;
- ESC и `toy0>` не изменены;
- `SYS_RT_WAIT` не изменён по смыслу перехода job -> blocked;
- `rt_pick_ready()` и priority arbitration не изменены;
- `SENSOR.EXE`, `SENSOR1.EXE`, `SENSOR2.EXE` остаются обычными EXE1;
- число RT slots остаётся 4.

## 5. Инвариант Stage 5.1

Для активной job выполняется:

```text
rt_job_release_tick = release конкретной job
rt_deadline_tick    = rt_job_release_tick + deadline_interval
```

`rt_deadline_tick` не пересчитывается от момента фактического dispatch.

## 6. Ограничение

Текущий PIT даёт шаг 20 ms. Stage 5.1 не делает 10-ms RT timebase.
Реальный 10-ms период будет отдельным Stage после завершения deadline-модели.
