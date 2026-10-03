# ToyOS v67.11-B FIX50 — Recovery Manager

FIX50 построен поверх подтвержденного FIX49.

- Добавлен SYS_RECOVERY_MANAGER=68 без изменения номеров ABI 0..67.
- Централизована bounded policy detect -> restart -> degrade -> safe.
- Лимит совместим с FIX49: максимум 3 restart, затем budget exhausted -> SAFE.
- Supervisor обнаруживает fault/watchdog и исполняет решение manager; policy больше не дублируется в supervisor.
- Recovery Manager не выполняет FAT16 I/O и не меняет RT/MT scheduler policy.
- FIX49 EVENT.LOG/recovery history остается источником последовательной диагностики.
- Добавлен TESTRCM.TST и полный автоматический verification plan.
