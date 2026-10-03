# ToyOS v67.11-B FIX17 — minimal silent EXECMT tasks

Based on FIX16. RT/MT scheduler policy and MTDATA kernel ABI are unchanged.

- MT01.EXE..MT25.EXE are identical minimal CPU-bound tasks except for ID.
- Each task publishes exactly one `MTxx:RUN;` MTDATA snapshot and then loops forever.
- No MT test task uses console, BLOCK, WAKE or EXIT syscalls.
- MT switching is therefore caused by the kernel 20 ms MT quantum only (plus RT preemption / explicit stop).
- A second EXECMT while the fixed MT session is active remains forbidden by design; shell now reports a readable session-active message instead of unsigned `4294967295` for EXE_ERR_BUSY (-1).
- `MTSTOP MTn` and `MTSTOP ALL` remain the explicit stop mechanisms.
