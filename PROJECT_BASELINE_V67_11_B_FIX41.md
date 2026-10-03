# ToyOS v67.11-B FIX41 — EVENT LOG FOUNDATION

Base: runtime-confirmed FIX40B.

FIX41 changes only diagnostics/lifecycle logging and load-test cleanup:
- SYS_EVENT_LOG=62; existing syscall numbers 0..61 unchanged.
- Kernel-owned bounded 64-record lifecycle event ring. No FAT I/O occurs on fault/exit paths.
- Each retained process result also records: sequence, timer tick, PID, process type, exit reason, status, vector, error.
- `eventlog` and `eventlog clear` shell commands.
- TEST runner `ASSERT EVENT_REASON N` reads structured kernel data.
- TESTEVT.TST regression for Ring3 #UD event containment/logging.
- TESTLOAD.TST and TESTKEY.TST now perform `rtstat reset` after stopping their RT load, so manual testing starts from clean RT statistics.
- No RT scheduler policy, release/deadline logic, MT 20-ms quantum, watchdog policy, FAT16 LBA, or kernel-area layout changes.

FIX40B remains the rollback baseline until FIX41 is runtime-confirmed.
