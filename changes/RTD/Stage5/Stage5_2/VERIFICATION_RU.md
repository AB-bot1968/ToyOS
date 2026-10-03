# ToyOS v67 RTD Stage 5.2 — верификация

## Исходные условия

База: `ToyOS v67 RTD Stage 5.1 FIX1`.

## Статические проверки

- `rt_pick_ready_class()` разделяет on-time и missed jobs — PASS;
- `rt_pick_ready()` сохраняет priority/deadline/slot ordering внутри on-time класса — PASS;
- `rt_pick_ready_missed()` использует ту же ordering внутри missed класса — PASS;
- scheduler использует missed class только при отсутствии on-time выбора — PASS;
- `RTDMISS.EXE` включён в сборку и FAT16 root — PASS.

## Host checks

`tools/test_rt_deadline_miss.c`:

- on-time job превосходит missed job с более высоким priority — PASS;
- среди missed jobs работает priority — PASS;
- при равном priority работает absolute deadline — PASS;
- отсутствие READY missed jobs корректно возвращает `-1` — PASS.

## Compilation

- `src/kernel.c` — 32-bit freestanding compile PASS;
- `src/rt_deadline_miss_diag.c` — 32-bit freestanding compile PASS.

## Runtime

Runtime QEMU в текущем окружении не выполнялся: `qemu-system-i386` отсутствует.

## Следующий шаг

После runtime-подтверждения Stage 5.2 можно зафиксировать версию и перейти к Stage 5.3: определить поведение следующего release после завершения просроченной job (пропуск, catch-up или отдельная политика) уже без изменения базовой обработки miss.
