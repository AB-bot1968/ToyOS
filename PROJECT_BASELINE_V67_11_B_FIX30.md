# ToyOS v67.11-B FIX30 — MTSTAT COMPACT

Base: runtime-confirmed FIX29, itself branched from stable FIX28.

Change is presentation-only in `src/user_shell.c`:
- plain `MTSTAT` now uses the same compact two-record-per-line layout as `MTSTAT WATCH`;
- all MT1..MT25 statistics remain visible;
- fields remain D=DISPATCH, Q=QUANTA, C=CPU_TICKS;
- SYS_MT_STATS=54 and its 25 x 10 uint32 snapshot ABI are unchanged;
- scheduler, MT quantum, RT scheduler/policy, RT/MT integration, WATCH/ESC, RTDATA/MTDATA, kernel layout and FAT16 layout are unchanged.

FIX29 remains the rollback/control point for this change.
