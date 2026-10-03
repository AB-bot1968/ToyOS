# ToyOS v67.11-B FIX28 — MTSTOP ALL shell return

Base: runtime-confirmed FIX27. FIX27 is frozen as the control checkpoint.

FIX28 changes only the SYS_MT_STOP / `mtstop all` return path. `sched_active` describes an active MT scheduler/session, not the identity of the syscall caller. FIX27 restored `sched_saved_shell` whenever `sched_active` was true, even when the live SYS_MT_STOP frame already belonged to the foreground shell. With RT+MT active this could restore an older console-read frame, requiring a second Enter before the command result/prompt appeared.

FIX28 snapshots `mt_current_running()` before stopping the session. The saved shell frame/CR3 is restored only when SYS_MT_STOP was actually called from a running MT context. A normal shell `mtstop all` keeps its current syscall frame and returns EAX=0 directly. MT quantum/fairness state is also cleared when the whole MT session ends.

Unchanged: FIX27 RT start-statistics correction; FIX26 RT/MT quantum return; 8 RT slots; 25 MT slots; 20 ms MT quantum; RT priority; RT release/deadline policy; syscall numbering 0..54; WATCH/ESC; RTDATA/MTDATA; kernel area 256 sectors; FAT16 LBA 512; case-insensitive shell commands/parameters.
