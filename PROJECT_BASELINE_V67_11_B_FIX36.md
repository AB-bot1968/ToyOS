# ToyOS v67.11-B FIX36 — EXIT STATUS + WAIT

Base: runtime-confirmed FIX35B. FIX34A remains the older rollback checkpoint.

FIX36 adds process termination results only. It does not change RT priority, RT release/deadline policy, MT round-robin, the 20 ms MT quantum, RT/MT context-switch paths, the 256-sector kernel area, or FAT16 LBA 512.

## ABI

- Existing syscalls 0..55 are unchanged.
- New `SYS_PROCESS_WAIT = 56`.
- EBX = pointer to 8 writable u32 words, ECX = PID.
- Return 1: completed result copied as PID, TYPE, EXIT_REASON, EXIT_STATUS, VECTOR, ERROR, EIP, CR2.
- Return 0: PID is currently active; caller may yield and retry.
- Return 0xfffffffd: PID is unknown or its bounded retained result expired.
- Return 0xffffffff: invalid call/buffer.
- Normal SYS_EXIT preserves EBX as the exit status.
- Fault termination records EXIT_REASON=FAULT and EXIT_STATUS=0xffffffff.

## Retention

The kernel owns a 64-entry termination-result ring. A result is stored before an FG/MT/RT process instance is torn down. The record is keyed by PID and therefore survives reuse of an RT or MT slot. The table is diagnostic/lifecycle state only and is never read by scheduler policy.

The shell command `wait PID` retries only while the PID is active and calls the existing `SYS_RT_YIELD` between retries, so it does not spin in a CPU busy-loop and background RT/MT work can continue.

Two deterministic test programs are added: EXIT0.EXE and EXIT7.EXE.

## FIX36A build correction
The W64DevKit i386 PE/COFF C symbol for `program_main` is `_program_main`. `exit_test.c` now uses the same `_start -> _program_main` convention as the already working FIX35 fault diagnostics. No kernel, scheduler, syscall, ABI, filesystem, or process-wait behavior was changed by this correction.
