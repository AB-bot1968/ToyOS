# ToyOS v67.11-B FIX18 — RTSTAT 6-hour window

Baseline: FIX17 MINIMAL MT (runtime-confirmed by user).

FIX18 changes only RT statistics/observability. Scheduler policy, RT/MT context switching,
20-ms MT quantum, RTDATA/MTDATA ABI, syscall numbers, FAT16 LBA 512 and kernel slot limit
remain unchanged.

## RT statistics model
- Rolling window: 6 hours.
- Resolution: 72 buckets x 5 minutes, RT clock 100 Hz.
- `RTSTAT`, `COMPARE`, `WATCH`: JOBS/MISS/SKIP and timing aggregates are from this window.
- No 64-bit total job counter is introduced.
- Lifetime anomaly counters: MISS and SKIP only, uint32_t saturating at 0xffffffff.
- Last 16 anomaly events per RT slot: tick, job sequence, miss flag, skipped-release delta,
  deadline lateness in RT ticks.
- `RTSTAT EVENTS [SLOTn]` displays lifetime anomaly counters and the anomaly journal.
- Existing `RTSTAT DUMP` remains the last-eight-completed-jobs diagnostic trace.
- `RTSTAT RESET` clears window statistics, lifetime anomaly counters and event journal for all
  slots and correctly baselines the scheduler's cumulative skip counter.

`lifetime` means since the RT slot statistics were initialized/reset for the current task,
not persistent storage across reboot or slot reuse.
