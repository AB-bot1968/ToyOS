# ToyOS v67.11-B FIX49 — Persistent Fault/Recovery History

Base: FIX48B CURRENT STABLE only. The discarded I/O-bitmap FIX49 branch is not included.

## Scope
- Extends the existing FIX41 RAM event ring without changing its 8-word record format.
- Adds recovery/safety event types while preserving all existing lifecycle events.
- SAFE state transitions are appended to RAM by the kernel: NORMAL=100, DEGRADED=101, SAFE=102.
- Supervisor appends metadata-only markers: RESTART=103, software WATCHDOG timeout=104, restart BUDGET exhausted=105.
- Existing process fault/watchdog-stop lifecycle events remain unchanged (reason 2 / reason 3).
- New SYS_RECOVERY_EVENT=67 only appends metadata to RAM. It performs no recovery action, FAT I/O, scheduling change, or SAFE transition.
- `recoverylog` filters RAM EVENT.LOG history to safety/recovery events; `recoverylog disk` filters the compatible persisted EVENT.LOG image.
- FAT persistence remains explicitly in Ring3 shell via existing `eventlog save`; no FAT write is performed from IRQ, exception, RT, or kernel recovery paths.
- Existing EVENT.LOG disk format/version is unchanged.

## Regression
Primary new test: TESTRCV.TST. Related TESTSUP.TST, TESTWD.TST and TESTPOL.TST are strengthened with recovery-history assertions.
