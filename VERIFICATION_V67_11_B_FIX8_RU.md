# ToyOS v67.11-B FIX8 — RTSTAT TABLE / DUMP

База: `ToyOS_v67_11_B_FIX7_RTSTAT_WATCH_YIELD`.

Исправления:

1. `RTSTAT` теперь выводит компактную таблицу: SLOT, TASK, JOBS, INT, JIT, DSP, CPU, WALL, MISS, SKIP.
2. `RTSTAT WATCH` использует ту же таблицу и сохраняет обновление раз в секунду с `SYS_RT_YIELD` из FIX7.
3. `RTSTAT DUMP` выводит последние завершённые jobs в табличном виде отдельно для каждого RT slot.
4. Исправлена ошибка учёта SKIP: раньше `rt_stats_record()` выполнялся до обновления `rt_skipped_releases`, поэтому summary/DUMP отставали на один завершённый job.
5. В kernel ring поле SKIP теперь является числом пропусков именно данного sample/job, а не накопительным lifetime-счётчиком. В summary `RTSTAT` SKIP остаётся накопительным счётчиком задачи.
6. ABI `SYS_RT_STATS` и размер ring sample не изменены.
7. Регистронезависимость команд и параметров сохранена.

Единица INT/JIT/DSP/CPU/WALL: один RT tick = 10 ms. Нулевые DSP/CPU/WALL допустимы, если событие/работа укладывается внутри одного 10-ms tick; это не ошибка DUMP.

Проверки Linux environment:
- `src/kernel.c` freestanding i386 compile: PASS;
- `src/user_shell.c` freestanding i386 compile: PASS;
- FIX7 regression check: PASS;
- FIX8 table/DUMP check: PASS.

Полный `build.sh` здесь намеренно останавливается на проверке toolchain: проект требует x86 W64DevKit с target `i686-*`. Полную штатную сборку следует выполнить в W64DevKit.
