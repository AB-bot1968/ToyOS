# ToyOS v67.11-B FIX37

Base: runtime-confirmed FIX36E.

Adds SYS_PROCESS_SPAWN=57. `spawn FILE.EXE [ARG1..ARG8]` creates a detached MT-class Ring-3 process in a private existing MT slot and returns its PID immediately. ABI for spawned EXE1: EBX=argc, ECX=argv; argv[0] is executable name. Existing SYS 0..56 unchanged. RT policy and MT 20-ms quantum are unchanged.

Help is reformatted into three compact categorized pages.

Candidate until W64DevKit/QEMU runtime validation. FIX36E remains rollback stable.
