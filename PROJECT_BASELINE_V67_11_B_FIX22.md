# ToyOS v67.11-B FIX22 — MTSTAT diagnostics

Основа: FIX21. Планировщик и его политика не изменены.

Изменения:
- добавлен read-only `SYS_MT_STATS=54`; ABI syscall 0..53 сохранён;
- добавлены kernel counters для каждого из 25 MT slots: DISPATCH, QUANTA, CPU_TICKS;
- `CPU_TICKS` учитывает реально завершённые 10-ms интервалы MT даже если на границе tick задачу перехватывает RT;
- `QUANTA` — диагностическое число каждых двух накопленных MT CPU ticks (=20 ms CPU), не управляющее scheduler;
- `DISPATCH` увеличивается только при фактической передаче управления MT task;
- counters обнуляются в начале новой EXECMT session;
- shell: `MTSTAT` и `MTSTAT WATCH`, WATCH обновляется раз в секунду и выходит по ESC;
- для ESC во время активного RT `MTSTAT WATCH` использует подтверждённый FIX21 foreground-watch routing;
- MT tasks ничего не печатают в console; MTDATA не изменён.

Не изменено:
- RTD: 8 slots, RT priority/policy/release/deadline;
- MT: 25 slots, scheduler quantum `MT_QUANTUM_TICKS=2` (20 ms);
- F10/RTDATA/RTSTAT/MTLIST/MTDATA/MTSTOP;
- kernel area 256 sectors; FAT16 starts at LBA 512.
