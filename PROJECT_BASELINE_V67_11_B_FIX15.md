# ToyOS v67.11-B FIX15 — MTDATA

Base: FIX14 MTLIST.

## Added
- `SYS_MT_DATA = 53`; existing syscall numbers 0..52 are unchanged.
- `MTDATA MT1..MT25` reads the last snapshot published by that EXECMT position.
- Each MT position owns a 255-byte payload + NUL, `len`, `seq` and `valid` in kernel memory.
- `seq` and `session_id` are uint32 change markers; compare them only by inequality.
- MT tasks publish through a request structure. The kernel derives the publisher's MT position from the currently running task; a task cannot publish as another MT position.
- A published snapshot survives `SYS_EXIT` and `MTSTOP MTn`.
- MT positions remain owned by the original EXECMT session after task completion.
- `MTSTOP ALL` closes the session and clears all MTDATA snapshots.
- A new `EXECMT` is rejected while an old MT session still exists, including a session whose tasks have all completed. Use `MTSTOP ALL` first.
- Validation MT01..MT25 publish `MTxx:DONE;` immediately before exit.

## Unchanged
- FIX14 MTLIST semantics.
- RT/RTDATA scheduler and ABI.
- 20 ms MT quantum and RT priority.
- FAT16 starts at LBA 512.
- Kernel slot remains limited to 128 sectors by the existing build/boot configuration.
