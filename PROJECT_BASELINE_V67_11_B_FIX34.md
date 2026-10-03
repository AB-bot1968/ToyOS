# ToyOS v67.11-B FIX34 — Process Identity Hardening

Base: runtime-confirmed FIX33, itself based on runtime-confirmed FIX32.

FIX34 adds one authoritative **observational** resolver for the Ring-3 caller. It returns `{type, slot, pid, cr3}` for FG, MT or RT by validating the address space and the existing live state. It does not choose a task, change priority, change the 20-ms MT quantum, or alter RT release/deadline policy.

## Integration
- `rt_find_current()` now delegates to the common resolver.
- `mt_current_running()` now delegates to the common resolver.
- `SYS_EXIT` and `SYS_SCHED_EXIT` dispatch termination by resolved caller type.
- VGA MAP/UNMAP/TEXT require resolved `FG` ownership instead of repeating CR3 logic.
- Existing RTDATA and MTDATA publisher ownership therefore also exercise the common RT/MT identity paths through `rt_find_current()` / `mt_current_running()`.

## Diagnostic ABI
`SYS_PROCESS_INFO` remains syscall **55**. FIX33 `EBX=0` snapshot ABI is unchanged. FIX34 adds `EBX=1`: return one 10-u32 record for the current Ring-3 process. EAX=1 when FG/MT/RT identity is resolved, EAX=0 when the Ring-3 caller is not an EXE1 process, and `0xffffffff` for invalid arguments.

`PROCID.EXE` is a diagnostic EXE1. As FG it prints its resolved PID/type/slot. As MT it publishes the same identity through MTDATA and remains CPU-bound. As RT it publishes through RTDATA once per job and waits through SYS_RT_WAIT. Background modes never write to the shared console.

## Preserved baseline
RT8, MT25, RT priority, 20-ms MT quantum, FIX21 WATCH/ESC, FIX26/FIX27/FIX28 scheduler fixes, FIX31 console-wait MT progress, FIX32 VGA behavior, FIX33 PID/PS, kernel 256 sectors and FAT16 at LBA 512 are intentionally preserved. Syscalls 0..54 are unchanged; 55 is extended only by an operation selector.
