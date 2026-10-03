# ToyOS v67 RTD Stage 5.1 FIX1 — верификация

## Проверено автоматически

- наличие `rt_job_missed` — PASS;
- фиксация miss без изменения release/deadline — PASS;
- отсутствие `rt_advance_release()` в deadline-hit path — PASS;
- отсутствие записи нового `rt_deadline_tick` в deadline-hit path — PASS;
- отсутствие `TASK_BLOCKED` в deadline-hit path — PASS;
- `SYS_RT_WAIT` очищает `rt_job_missed` при завершении job — PASS;
- `DEADLINE.EXE` выводит `missed=` — PASS;
- host timing unit-test — PASS;
- `kernel.c` 32-bit freestanding compile — PASS;
- `rt_deadline_diag.c` 32-bit freestanding compile — PASS.

## Runtime expectation

Для команды:

```text
exec RTD.EXE DEADLINE.EXE 1000 100 3
F10
```

абсолютные `release/deadline` одной job не меняются при пересечении deadline.
Изменяется только состояние miss.

## Ограничение среды

Boot/runtime в QEMU в рабочем окружении разработки не выполнялся, так как `qemu-system-i386` отсутствует.
