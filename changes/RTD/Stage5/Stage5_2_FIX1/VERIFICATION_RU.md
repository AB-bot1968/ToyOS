# Verification — Stage 5.2 FIX1

Проверяются:

- on-time READY job имеет приоритет над любой missed READY job;
- missed job остаётся допустимым best-effort fallback;
- при равном priority используется wrap-safe absolute deadline;
- RTDMISS выдаёт фактический dispatch trace;
- kernel.c компилируется в 32-bit freestanding режиме;
- RTDMISS diagnostic EXE1 компилируется в 32-bit freestanding режиме.

Runtime-boot в QEMU должен быть подтверждён на целевой системе отдельно.


## Результат проверки последнего runtime-сценария

Модель воспроизводит три перехода: `RTDMISS (on-time) → SENSOR1`, затем после deadline miss `RTDMISS (missed) → SENSOR1 (on-time)`, затем при блокировке SENSOR1 `→ RTDMISS (best-effort)`. Это соответствует выбранной мягкой политике Stage 5.2.
