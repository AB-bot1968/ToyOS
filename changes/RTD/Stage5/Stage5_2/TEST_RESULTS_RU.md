# ToyOS v67 RTD Stage 5.2 — результаты проверок

## Результат исходного изменения

Stage 5.2 реализует soft-deadline/best-effort semantics без изменения F10, ESC, `toy0>`, `SYS_RT_WAIT`, EXE1 loader и PIT 50 Hz.

## Unit/model

`tools/test_rt_deadline_miss.c` — PASS.

Проверено:

- on-time READY job имеет приоритет над missed READY job независимо от numeric priority;
- внутри missed класса работает `priority -> absolute deadline -> slot`;
- отсутствие missed READY корректно возвращает `-1`;
- более высокий priority внутри best-effort класса выбирается первым;
- при равном priority используется более ранний absolute deadline.

## Focused regression

`sh check_rtd_stage5_2.sh` — PASS.

## Предыдущие RTD-этапы

Проверены после изменения:

- Stage 1 — PASS;
- Stage 1 error-fix — PASS;
- Stage 1 FIX3/FIX6 — PASS;
- Stage 2 / FIX1 / FIX2 — PASS;
- Stage 3 / FIX1 / FIX2 / FIX3 / FIX4 / F10 — PASS;
- Stage 4.1 / 4.2 / 4.3 / 4.4 — PASS;
- Stage 5.1 / FIX1 — PASS.

## Freestanding compilation

- `src/kernel.c` — PASS, единственное предупреждение — известная старая функция `fat_find_free_dir`;
- `src/user_shell.c` — PASS, известное старое предупреждение `sys_ls`;
- `src/rtd.c` — PASS;
- `src/rt_sensor.c` — PASS;
- `src/rt_sensor_diag.c` — PASS;
- `src/rt_deadline_diag.c` — PASS;
- `src/rt_deadline_miss_diag.c` — PASS.

## Adapted full build

С адаптированным локальным target-wrapper сборка успешно:

- компилирует kernel;
- создаёт `kernel.bin` размером 53984 bytes;
- создаёт FAT16 image 4194304 bytes;
- создаёт `RTD.EXE`, `SENSOR.EXE`, `SENSOR1.EXE`, `SENSOR2.EXE`, `DEADLINE.EXE`, `RTDMISS.EXE`;
- `fat16check` подтверждает `RTDMISS.EXE` в FAT16 root.

Полный `build.sh` в данном Linux-контейнере останавливается на исторической `check13.sh`, которая проверяет устаревшую текстовую форму EXE stack mapping. Это существующая несовместимость regression-теста, не ошибка Stage 5.2.

## Runtime

QEMU runtime не выполнен, поскольку `qemu-system-i386` отсутствует в окружении.
