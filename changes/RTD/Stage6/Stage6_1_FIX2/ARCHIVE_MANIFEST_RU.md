# Manifest — Stage 6.1 FIX2

Исходная база: `ToyOS v67 RTD Stage 6.1 FIX1`.

Изменения:

- `src/kernel.c` — fallback выбора MISSED RT job внутри `SYS_CONSOLE_READ`;
- `src/rt_sensor_diag.c` — диагностический RT clock 10 ms;
- `build.sh` — запуск `check_rtd_stage6_1_fix2.sh`;
- `check_rtd_stage6_1_fix2.sh` — focused regression test;
- `changes/RTD/Stage6/Stage6_1_FIX2/*` — документация и patch.

Каталог `build/` в исходный архив не входит.
