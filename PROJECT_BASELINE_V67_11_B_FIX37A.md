# ToyOS v67.11-B FIX37A — SPAWN failure containment/diagnostics

Base: runtime-confirmed FIX36E; FIX37 rejected after target returned generic `spawn: FAIL`.

FIX37A keeps SYS_PROCESS_SPAWN=57 and the scheduler policy unchanged. It fixes a real rollback defect in FIX37: if the child image loader fails after a new MT session has been initialized, the empty session is now rolled back instead of leaving stale MT scheduler/session state. The shell also preserves and displays the exact existing EXE loader error class (BUSY/NAME/OPEN/HEADER/MAGIC/IMAGE_SIZE/ENTRY/BSS_SIZE/IMAGE_READ/TRAILING/MEMORY), rather than collapsing every negative kernel result to `spawn: FAIL`.

This diagnostic is intentionally part of the interface: a target-only loader failure must remain observable. No existing syscall number or RT/MT scheduling rule changed.
