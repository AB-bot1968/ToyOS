# ToyOS v67.11-B FIX36D TEST STACK SAFETY

Base: runtime-confirmed stable FIX36B. FIX36C is rejected due to Ring3 #PF in TESTCORE.

FIX36D reintroduces the TEST infrastructure with stack-safe workspace. Scheduler policy and syscall ABI 0..56 are unchanged.

Changes:
- TEST process snapshot (34x10 u32), 400-byte line buffer and 256-byte file chunk are static `.userdata`, not local Ring3 stack arrays.
- interactive `test FILE.TST` is dispatched directly by `shell_loop`, avoiding a second large `execute_line` frame around the whole runner.
- `test_command`, `test_assert`, and dispatcher boundaries are kept non-inlined where required for predictable stack use.
- nested `RUN test ...` is rejected.
- RUN otherwise reuses the normal command dispatcher.
- FIX36B WAIT behavior is unchanged.
- TESTCORE/TESTPROC remain FAT16 regression scripts.

FIX36B remains rollback/stable until FIX36D passes W64DevKit/QEMU runtime verification.
