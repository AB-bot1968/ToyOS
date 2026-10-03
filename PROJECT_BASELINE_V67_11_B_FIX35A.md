# ToyOS v67.11-B FIX35A — RT fault containment correction

Base: FIX35 candidate, whose base is runtime-confirmed FIX34A.

FIX35 runtime testing showed that a faulting RT task could be contained when it was the lowest-priority RT job, but a #PF from a fault task that preempted another RT job could fall through to the fatal exception path. FIX35A keeps FIX34A scheduler policy unchanged and hardens only exception ownership/teardown.

Changes:
- selected Ring3 #UD/#GP/#PF containment remains FIX35 functionality;
- if the normal FIX34A identity resolver cannot classify a synchronous exception, containment performs an exact private RT CR3 lookup; no scheduler state is changed by this lookup;
- after a fault has identified an RT slot, teardown uses that known slot directly instead of resolving current identity a second time;
- normal SYS_EXIT/SYS_SCHED_EXIT continue to use the existing FIX34A identity resolver;
- syscall-test now treats SYS_CONSOLE_READ return value 2 as an internal scheduler hand-off and continues waiting, matching the already proven FIX32 VGADRV behavior.

Unchanged: RT8, MT25, RT priority/release/deadline policy, 20 ms MT quantum, FIX21 WATCH/ESC, FIX26-FIX32 scheduler integration, FIX33 metadata, FIX34A identity semantics outside exception containment, syscall ABI 0..55, 256-sector kernel area, FAT16 at LBA 512.

FIX34A remains the runtime-confirmed rollback point until FIX35A passes target tests.
