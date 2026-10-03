# ToyOS v67.11-B FIX23 — RT/MT integration

Base: FIX22 MTSTAT. Scheduler policy remains hierarchical: RT has priority over MT; MT quantum remains 20 ms.

Corrections:
1. Foreground EXE/RTD is no longer classified as the currently running MT task merely because an EXECMT session exists. SYS_EXIT/SYS_SCHED_EXIT use the actual current CR3.
2. IRQ0 does not dispatch MT over a foreground EXE/RTD frame that shares the shell address space.
3. When an RT job finishes and no RT job is ready, available slack may be handed directly to a READY MT task instead of waiting for a later PIT tick. The existing shell-turn rule is retained.
4. SYS_RT_STATUS active/free uses the same active-slot predicate as rt_recount_active (state plus non-zero RT period).

Unchanged: RT_MAX_TASKS=8, SCHED_TASKS=25, MT_QUANTUM_TICKS=2, syscalls 0..54, RTDATA/MTDATA/MTSTAT, FIX21 WATCH ESC routing, 256-sector kernel area, FAT16 LBA 512.
