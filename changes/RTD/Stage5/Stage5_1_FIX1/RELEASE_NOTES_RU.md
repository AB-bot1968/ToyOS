# ToyOS v67 RTD Stage 5.1 FIX1 — исправление неизменности абсолютного deadline

## База

Изменение сделано от `ToyOS v67 RTD Stage 5.1` на базе принятой архитектуры `v67.4.4 / Stage 4.4`.

## Обнаруженная проблема

При достижении `rt_deadline_tick` предыдущая реализация сразу выполняла перенос текущей RT job:

- `rt_job_active = 0`;
- `rt_advance_release()`;
- пересчёт `rt_deadline_tick` для следующего release;
- перевод задачи в `TASK_BLOCKED`.

Это означало, что диагностический вызов `SYS_RT_DEADLINE_INFO` мог увидеть уже следующую пару `release/deadline`, хотя код пользовательской программы ещё не завершил текущую job через `SYS_RT_WAIT`.

Для абсолютной модели это смешивало две разные сущности: **deadline текущей job** и **release следующей job**.

## Исправленная модель

Для активной job:

```text
release = rt_job_release_tick
 deadline = rt_deadline_tick
```

остаются неизменными до завершения job через `SYS_RT_WAIT`.

Если `now >= deadline`:

```text
rt_job_missed = 1
rt_missed_deadlines++
```

и больше ничего в текущем `release/deadline` не изменяется.

Повторное увеличение счётчика подавляется флагом `rt_job_missed`.

## Завершение job

`SYS_RT_WAIT`:

1. определяет, был ли deadline пропущен;
2. возвращает `1`, если был пропуск;
3. вычисляет следующий `rt_next_release_tick`;
4. формирует deadline следующей job;
5. переводит текущую job в состояние ожидания.

Таким образом граница между двумя jobs возникает только в точке завершения текущей job.

## Диагностика

`DEADLINE.EXE` теперь дополнительно печатает `missed=`.

Пример:

```text
DEADLINE: started release=100 deadline=105 now=100 missed=0
DEADLINE: sample release=100 deadline=105 now=101 missed=0
DEADLINE: sample release=100 deadline=105 now=103 missed=0
DEADLINE: sample release=100 deadline=105 now=106 missed=1
```

Ключевой инвариант: `release=100 deadline=105` сохраняется даже при `now=106`.

## Что не изменялось

- priority arbitration;
- F10 queue;
- ESC и `toy0>`;
- `SYS_RT_WAIT` как точка завершения job;
- EXE1 loader;
- `SENSOR.EXE`, `SENSOR1.EXE`, `SENSOR2.EXE`;
- PIT 50 Hz;
- VGA/text;
- EXECMT.
