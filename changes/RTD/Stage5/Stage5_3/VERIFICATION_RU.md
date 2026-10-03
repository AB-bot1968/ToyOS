# ToyOS v67 RTD Stage 5.3 — результаты проверки

- Формула no-backlog release проверена host unit-test.
- Точная граница `now == next_release` проверена.
- Несколько пропущенных периодов проверены.
- Wrap-around `uint32_t` timer проверен.
- `kernel.c` прошёл 32-bit freestanding compile.
- `RTDSKIP.EXE` прошёл 32-bit freestanding compile.
- Отдельный `SYS_RT_JOB_INFO=44` не изменяет старый `SYS_RT_DEADLINE_INFO=42`.
- `SENSOR.EXE` и Stage 5.2 soft-deadline путь не требуют изменений ABI.

## Интерпретация counters

`deadline_misses` — количество реально созданных job, для которых deadline был пропущен.

`skipped_releases` — количество номинальных периодических release, которые не создавались отдельной job из-за того, что предыдущая job завершилась уже после этих release.

Эти два счётчика намеренно разделены, чтобы не выдавать пропущенную из-за backlog активацию за фактически исполнявшуюся job.
