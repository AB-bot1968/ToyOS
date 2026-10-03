# Archive manifest — Stage 6.1

База: ToyOS v67 RTD Stage 5.3.

Новые/изменённые RTD-компоненты:

- `src/kernel.c`
- `src/user_shell.c`
- `src/rtd.c`
- `src/rt_deadline.h`
- `src/rt_deadline_diag.c`
- `src/rt_deadline_miss_diag.c`
- `src/rt_period_skip_diag.c`
- `src/rt_timebase_diag.c`
- `tools/test_rt_timebase.c`
- `check_rtd_stage6_1.sh`
- `build.sh`
- `check33.sh`
- `check34.sh`
- `check_rtd_stage5_1.sh`
- `check_rtd_stage6_1.sh`
- `tools/test_rt_deadline.c`
- `tools/test_rt_timebase.c`
- `README.md`
- `DESIGN.md`

Новый EXE1: `RTTIME.EXE`.

Тестовые изменения также сохраняются для совместимости с новой dual-rate моделью времени.

Каталог `build/` в исходный архив не включается.
