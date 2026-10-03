# ToyOS v67 RTD Stage 5.2 — обработка deadline miss

## База

Изменение выполняется от зафиксированной версии `ToyOS v67 RTD Stage 4.4` с применёнными и проверенными Stage 5.1 FIX1.

## Цель шага

Stage 5.2 добавляет реальное планировочное следствие пропуска абсолютного deadline, не уничтожая текущий пользовательский контекст и не меняя механизмы F10, ESC, `SYS_RT_WAIT`, EXE1 loader, VGA/text и PIT 50 Hz.

## Выбранная политика

Используется мягкая (soft real-time) политика:

1. До deadline job считается on-time.
2. При `now >= rt_deadline_tick` устанавливается `rt_job_missed=1` и увеличивается `rt_missed_deadlines` один раз для текущей job.
3. `release_tick` и `deadline_tick` текущей job не изменяются.
4. Просроченная job остаётся живой и сохраняет свой Ring-3 context.
5. При выборе следующей READY job любая on-time job имеет преимущество перед просроченной независимо от numeric priority.
6. Если on-time READY job отсутствует, просроченная job может продолжить работу как best-effort. Среди нескольких просроченных задач сохраняется прежнее правило `priority -> absolute deadline -> slot`.
7. `SYS_RT_WAIT` по-прежнему является границей завершения текущей job; после него создаётся следующий release/deadline.

## Почему job не завершается автоматически

Автоматическое уничтожение контекста после deadline означало бы потерю состояния обычного EXE1 и требовало бы отдельной семантики перезапуска программы. Для текущего малого RT-слоя безопаснее сохранить контекст и явно понизить класс планирования.

## Изменение планировщика

Выбор READY задач разделён на два класса:

- `rt_pick_ready()` — только on-time jobs;
- `rt_pick_ready_missed()` — только deadline-missed jobs.

Оба используют существующее Stage 4.2 правило внутри своего класса:

```text
priority -> absolute deadline -> slot
```

При наличии хотя бы одной on-time job она выбирается раньше любой missed job.

## Диагностическая программа

Добавлен обычный EXE1 `RTDMISS.EXE`. Она намеренно держит одну job активной дольше небольшого deadline и выводит:

```text
RTDMISS: started ...
RTDMISS: sample ... missed=0
RTDMISS: sample ... missed=1
RTDMISS: job missed -> best-effort
...
RTDMISS: completed with deadline miss
```

Программа не является специальным RT-форматом и загружается существующим `RTD.EXE` так же, как `SENSOR.EXE`.

## Что не менялось

- `SENSOR.EXE`;
- `SENSOR1.EXE` / `SENSOR2.EXE`;
- F10 queue;
- ESC и возврат `toy0>`;
- `SYS_RT_WAIT` как граница завершения job;
- абсолютные `release/deadline` Stage 5.1;
- fixed-priority arbitration Stage 4.4;
- EXECMT;
- PIT 50 Hz;
- VGA/text режим.

## Поддержание regression-тестов при вынесении комментариев

В соответствии с требованием проекта исторические пояснения к изменениям RTD не оставляются в `src/*.c`. Поэтому два старых regression-теста, которые искали текст комментария непосредственно в исходнике (`check_rtd_stage1_errorfix.sh` и `check_rtd_stage4_2.sh`), переведены на поиск соответствующего пояснения в `changes/RTD/`. Исполняемая логика ядра от этого не изменилась.
