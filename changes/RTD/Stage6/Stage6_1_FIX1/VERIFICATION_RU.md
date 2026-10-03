# Stage 6.1 FIX1 — Verification

## Static verification

- `sched_irq_tick()` exits immediately for Ring-0 IRQ0 frames in detached RT mode.
- The guard appears before `rt_release_jobs()` and before any RT context switch.
- User-mode IRQ0 remains eligible for RT preemption.
- `SYS_RT_WAIT` and `SYS_CONSOLE_READ` retain their explicit safe dispatch paths.

## Focused verification

`check_rtd_stage6_1_fix1.sh` passed, including Stage 6.1 dual-rate checks and all RTD regressions through Stage 5.3.

## Runtime acceptance criterion

The target runtime test must show multiple `SENSOR1: tick` lines for:

```text
exec RTD.EXE SENSOR1.EXE 10 10 3
F10
```

with no immediate exit, hang or exception.
