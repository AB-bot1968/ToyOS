# ToyOS v67.11-B FIX39 SUPERVISOR FOUNDATION

Base: runtime-confirmed FIX38. FIX39 adds a user-space MT supervisor without changing scheduler policy.

- SUPERVIS.EXE launches one detached MT worker through SYS_PROCESS_SPAWN.
- SYS_PROCESS_RESULT=59 is a non-blocking termination-result query. Existing syscalls 0..58 are unchanged.
- On NORMAL worker termination supervisor exits status 0.
- On FAULT supervisor restarts the worker at most 3 times. After the third restart also faults, supervisor publishes FAILED and exits status 3.
- Status is published through existing MTDATA; shell commands: `supstart FILE.EXE`, `supstat`.
- TESTSUP.TST is the automatic regression scenario using FAULTUD.EXE.
- RT scheduling, release/deadline accounting, MT 20-ms quantum, FAT16 LBA512, kernel 256-sector area and process resource ownership are unchanged.
