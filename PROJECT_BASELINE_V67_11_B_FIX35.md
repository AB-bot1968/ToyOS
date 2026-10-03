# ToyOS v67.11-B FIX35 — Ring3 Exception Containment

Baseline: runtime-confirmed FIX34A. FIX34A remains the rollback checkpoint until FIX35 passes runtime tests.

FIX35 adds a deliberately narrow containment layer for EXE1 Ring3 faults: #UD (6), #GP (13), and #PF (14). The handler first checks saved CS CPL and then resolves the actual executing process through the FIX34A current-process identity resolver. Ring0 faults, unsupported vectors, and Ring3 frames without a known process identity keep the old fatal halt path.

For a contained fault the existing domain-specific termination path is reused: foreground EXE -> exe_exit, MT -> sched_exit, RT -> rt_exit. No new scheduler, dispatch policy, RT release/deadline rule, MT quantum rule, CR3 layout, FAT layout, or syscall number was introduced.

Process metadata now retains exit reason and fault vector/error/EIP/CR2. SYS_PROCESS_INFO remains syscall 55. Existing operations 0 (snapshot) and 1 (current identity) are preserved; internal diagnostic operation 2 returns fault detail for a PID. Shell `ps PID` exposes this data. Ordinary `ps` appends `FAULT` to an exited process when appropriate.

Three deterministic EXE1 probes are included: FAULTUD.EXE (`ud2`), FAULTGP.EXE (`cli` from CPL3), FAULTPF.EXE (write to unmapped 0x50000000). They intentionally fault and contain no console output themselves.

Preserved baseline invariants: RT8, MT25, RT priority, MT 20 ms quantum, FIX21 WATCH/ESC, FIX26/FIX27/FIX28 integration behavior, FIX31 console-wait MT progress, FIX32 VGA behavior, FIX33 PID metadata, FIX34A CR3-authoritative current identity, kernel area 256 sectors, FAT16 at LBA 512, syscall ABI 0..55.
