# ToyOS v67.11-B FIX21 — RTSTAT WATCH ESC

Base: FIX20 RTD8. FIX20/FIX19 are retained as rollback points.

Change is intentionally narrow: while foreground `RTSTAT WATCH` is active, ESC is owned by WATCH even if IRQ1 arrives while a detached RT task is executing after `SYS_RT_YIELD`. The shell marks WATCH active through internal `SYS_RT_STATS` op 6 before the loop and clears it on every normal loop exit. No public syscall number was added.

Unchanged: RT scheduler policy, 8 RT slots, F10 RTD8 launch, RTDATA, 6h RTSTAT data model, MT 25/20ms, MTDATA, syscall numbers 0..53, kernel area 256 sectors, FAT16 LBA 512.
