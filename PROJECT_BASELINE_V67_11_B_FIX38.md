# ToyOS v67.11-B FIX38 — PROCESS RESOURCE OWNERSHIP

Base: runtime-confirmed FIX37A. Scheduler policy is unchanged.

FIX38 introduces PID ownership for FAT16 file handles opened through Ring-3 SYS_FILE_OPEN. Read/write/close require the same owner. On normal exit, contained Ring-3 fault, RT stop, MT stop-one, and MT stop-all, all handles owned by the terminated/stopped PID are closed by the kernel.

SYS_RESOURCE_INFO=58 is diagnostic only: op=0 returns the number of process-owned handles (owner PID != 0); op=1, EBX=PID returns that PID's handle count. Existing syscall numbers 0..57 are unchanged.

Shell: `resstat [PID]`. Test runner adds `WAIT LAST` and `ASSERT PROC_HANDLES N`. `spawn` records the most recent successful PID only for foreground shell/test diagnostics; this is not scheduler state.

LEAKFD.EXE intentionally opens TESTCORE.TST and exits without close. TESTRES.TST verifies that kernel lifecycle cleanup returns process-owned handle count to zero.
