# ToyOS v67.11-B FIX36B baseline

Base: FIX36A, which itself is based on runtime-confirmed FIX35B.

FIX36B fixes the active-PID WAIT integration bug found by runtime testing with 4 RT + 4 MT. FIX36A polled SYS_PROCESS_WAIT and then called SYS_RT_YIELD. If no RT job was immediately dispatchable, SYS_RT_YIELD could HLT inside Ring0; PIT interrupts then had Ring0 frames and the scheduler correctly refused Ring3 MT context switching. A synchronous wait on an intentionally infinite RT/MT task also had no interactive cancellation path and looked like a dead OS.

FIX36B keeps syscall number 56 and the completed-result ABI. Pending SYS_PROCESS_WAIT now schedules from its original Ring3 shell frame: RT first, then MT, using the established FIX25/FIX31 ownership patterns. `wait PID` polls ESC and cancels cleanly; kernel ESC routing treats foreground WAIT like WATCH while detached RT temporarily owns CR3. No RT priority, release/deadline, MT quantum, RT->RT fault-return, PID/result-history or existing syscall 0..55 policy is changed.

`TOYOS_ARCHITECTURE_VISION.md` is added as a permanent source-tree document per project agreement.

FIX35B remains the last runtime-confirmed stable rollback until FIX36B passes W64DevKit/QEMU runtime verification.
