# ToyOS v67.11-B FIX31 — CONSOLE MT PROGRESS

Base: runtime-confirmed FIX29 plus FIX30 compact MTSTAT presentation.

## Fix
`SYS_CONSOLE_READ` no longer leaves MT work stalled while the interactive shell is waiting for a key. After checking ready RT work first, it can hand the original Ring3 shell frame directly to one READY MT task. The saved shell frame returns with EAX=2, already defined by `readline()` as an internal scheduler hand-off.

This removes the artificial difference between repeatedly invoking `MTSTAT` and running `MTSTAT WATCH`: MT tasks continue accumulating DISPATCH/QUANTA/CPU_TICKS while the prompt waits for keyboard input.

## Preserved
- RT has priority: the existing RT dispatch path is checked before MT.
- MT quantum remains 20 ms.
- MTSTAT snapshot ABI remains SYS_MT_STATS=54, 25 x 10 uint32_t.
- No syscall numbers changed.
- FIX21 WATCH/ESC, FIX26 quantum return, FIX27 RT start stats, FIX28 MTSTOP return, FIX29/FIX30 compact MTSTAT output unchanged.
- RT8, MT25, kernel area 256 sectors, FAT16 LBA 512 unchanged.
