# ToyOS v67 RTD Stage 5.3 — release/deadline после завершения job

## База

Этап выполнен от зафиксированной рабочей базы `v67.4.4` и накопленных исправлений Stage 5.1/5.2. Механизмы `F10`, `ESC`, `toy0>`, `SYS_RT_WAIT`, `SENSOR.EXE`, `priority` и soft-deadline Stage 5.2 не заменялись.

## Формальная политика

Stage 5.3 фиксирует для периодической задачи политику **no-backlog / skip-ahead**:

1. Текущая job сохраняет неизменные `release_tick` и `deadline_tick` до `SYS_RT_WAIT`.
2. После завершения job следующий номинальный release равен `previous_release + period`.
3. Если к моменту завершения этот release уже находится в прошлом, все такие просроченные номинальные release пропускаются.
4. `rt_skipped_releases` считает именно пропущенные активации, которые не создавались как отдельные job.
5. Если следующий release совпадает с текущим `now`, новая job может стать READY немедленно. Граница `now == release` не пропускается.
6. Следующая job получает новый абсолютный `release_tick`, а `deadline_tick = release_tick + deadline_ticks`.
7. Уже зафиксированный `deadline miss` текущей job не изменяется. Пропущенные release учитываются отдельным счётчиком и не превращаются задним числом в исполненные job.

## Изменения исходного кода

### `src/rt_deadline.h`

Добавлена wrap-safe функция `rt_next_release_after()`. Она вычисляет первый release, который не находится строго в прошлом, и количество пропущенных release без потенциально длинного цикла по каждому периоду.

### `src/kernel.c`

В `struct sched_task` добавлены:

- `rt_job_sequence` — номер созданной job;
- `rt_skipped_releases` — суммарное количество пропущенных активаций.

`rt_begin_job()` теперь централизованно создаёт новую job и увеличивает её номер.

`rt_wait()` после завершения job использует no-backlog policy. При совпадении `next_release == now` новая job создаётся немедленно; иначе задача блокируется до release.

### `SYS_RT_JOB_INFO = 44`

Добавлен диагностический syscall с 9 словами:

`job_sequence`, `job_release_tick`, `deadline_tick`, `now`, `next_release_tick`, `job_active`, `job_missed`, `deadline_misses`, `skipped_releases`.

Существующий `SYS_RT_DEADLINE_INFO = 42` не изменён, чтобы сохранить старый ABI.

### `RTDSKIP.EXE`

Добавлен обычный EXE1 диагностический процесс, который намеренно держит job несколько периодов и после `SYS_RT_WAIT` показывает состояние следующей job.

## Ограничение этапа

Stage 5.3 не изменяет частоту PIT и не вводит 10-ms timebase. Он определяет только семантику периодических release после завершения текущей job.
