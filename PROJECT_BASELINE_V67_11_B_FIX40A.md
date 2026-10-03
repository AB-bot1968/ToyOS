# ToyOS v67.11-B FIX40A — LOAD TEST DIRECTIVES

Baseline: runtime-confirmed FIX40 SOFTWARE WATCHDOG.

FIX40A intentionally changes only the Ring3 TEST runner and test files. Kernel, syscall ABI, RT scheduler policy, MT scheduler policy, watchdog behavior, FAT16 LBA layout and process lifecycle are unchanged.

## New TEST directives

- `START MT N` — start the deterministic prefix MT01.EXE..MTNN.EXE, N=1..25, through the existing SYS_EXECMT path.
- `START RT N` — prepare SENSOR1.EXE..SENSORN.EXE with the established diagnostic load profile and launch them through the existing RTD pending/F10 handler, N=1..8.
- `TYPE command` — stage one normal shell command inside TEST.
- `KEY ENTER` — submit the staged command through the normal shell command dispatcher.
- `KEY F10` — invoke the same `rt_launch_pending()` handler used by a real shell F10 event.

`ASSERT MT_ACTIVE N` and `ASSERT RT_ACTIVE N` keep their original assertion semantics: N is the expected active-process count. START is deliberately separate so an assertion never mutates system state.

No synthetic keyboard IRQ/scancode is generated. KEY is a deterministic logical shell-event test facility.

## Permanent load tests

- TESTLOAD.TST — fully automatic default 25 MT + 8 RT test. START/ASSERT N can be edited for mixes such as 4 MT + 8 RT.
- TESTKEY.TST — logical ENTER/F10 path, default 4 MT + 8 RT.

Both files are always copied into the FAT16 image by build.sh.
