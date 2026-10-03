# ToyOS v67.11-B FIX34A — Process Identity context ownership correction

Base: FIX34 candidate, itself derived from runtime-confirmed FIX33.

FIX34A corrects one regression in the new `process_current_identity()` resolver. The first FIX34 implementation required an RT/MT slot to be `TASK_RUNNING`. During PIT scheduling, however, the interrupted task can already be changed from RUNNING to READY before `rt_switch_to()` replaces the interrupted frame and CR3. At that instant the loaded CR3 still belongs to that RT/MT task.

This caused `rt_find_current()` to return "not RT" during an RT hand-off. `rt_switch_to()` then mistook the interrupted RT frame for a foreground shell frame and overwrote `rt_shell_frame`. With PROCID.EXE as a 10/10/1 RT task this manifested as a visible `toy0>` prompt followed by an unresponsive console.

FIX34A restores the proven FIX33 ownership rule inside the unified resolver: for RT, matching private CR3 plus active READY/RUNNING/BLOCKED slot identifies the caller; for MT, matching current private CR3 plus a live non-STOPPED/non-EXIT slot identifies it. CR3 is authoritative during hand-off windows.

No scheduling policy is changed. RT8, MT25, MT 20 ms quantum, RT release/deadline logic, FIX21 WATCH/ESC, FIX26-32 behavior, FIX33 PID/metadata, syscall ABI 0..55, kernel 256 sectors and FAT16 at LBA 512 are preserved.
