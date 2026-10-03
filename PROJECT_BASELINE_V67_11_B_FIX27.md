# ToyOS v67.11-B FIX27 — RT START STATS COMMIT

Baseline: FIX26 RT/MT QUANTUM RETURN, which is frozen as the confirmed control point.

FIX27 changes only the start-time accounting of RT tasks created by an F10 guarded batch.
The FIX24 launch guard prevents those new tasks from executing while RTD.EXE helpers are
loaded. Previously their release grid started inside SYS_RT_START, so loading the rest of
the batch was later counted as MISS/SKIP. FIX27 records the set of RT slots that existed
before F10 and, when the guard is released, commits only newly-created slots to a common
`rt_time_ticks` release epoch. Their initial scheduler/stat counters are re-baselined before
first execution. Pre-existing RT slots are not rebased or cleared.

No scheduler priority/quantum/release-grid algorithm change after commit. ABI 0..54,
RT=8, MT=25, MT quantum=20ms, FAT16 LBA512, kernel area=256 sectors remain unchanged.
