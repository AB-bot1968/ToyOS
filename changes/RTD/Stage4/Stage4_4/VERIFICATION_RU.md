# ToyOS v67 RTD Stage 4.4 — Verification

## Проверка текущего исходного архива

1. `sh check_rtd_stage4_1.sh` — PASS.
2. `sh check_rtd_stage4_2.sh` — PASS.
3. `sh check_rtd_stage4_3.sh` — PASS.
4. `sh check_rtd_stage4_4.sh` — PASS.
5. `tools/test_rt_priority_preemption.c` — PASS: `Stage 4.4 priority-preemption model passed`.
6. `src/kernel.c` — 32-bit freestanding strict compile — PASS с подавлением только существующего `unused-function` warning.
7. `src/user_shell.c` — 32-bit freestanding strict compile — PASS с подавлением только существующего `unused-function` warning.
8. `src/rt_sensor_diag.c`, ID=1 и ID=2 — 32-bit freestanding strict compile — PASS.

## Что проверяет Stage 4.4

`SYS_RT_TRACE=41` позволяет диагностическому EXE1 получить `task_id`, `priority`, `switches`, `dispatch_seq` и `state`. `rt_dispatch_seq` обновляется при каждом реальном RT dispatch, включая прямой dispatch из `SYS_CONSOLE_READ`.

`SENSOR1.EXE` и `SENSOR2.EXE` остаются обычными EXE1. Для пары `period=deadline=1000 ms`, `priority=3/7` включается контролируемый `PRIORITY-PROBE`, который удерживает job runnable на один/два системных тика. Это позволяет наблюдать выбор и preemption по `dispatch=`.

## Runtime

Фактический boot/runtime в текущем development container не выполнялся: `qemu-system-i386` отсутствует. Runtime должен быть подтверждён на целевой машине.
