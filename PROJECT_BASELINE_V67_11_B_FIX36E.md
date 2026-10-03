# ToyOS v67.11-B FIX36E — CRLF command

Baseline: runtime-confirmed FIX36D TEST STACK SAFETY.

FIX36E adds one shell filesystem helper only: `crlf FILE` appends exactly two bytes, CR (0x0D) and LF (0x0A), to an existing regular file. It does not create a missing file. Command matching remains case-insensitive.

No syscall numbers, kernel ABI, scheduler policy, RT/MT logic, process lifecycle, wait semantics, test-runner semantics, FAT16 layout, or kernel-area layout are changed.

FIX36D remains the rollback point until FIX36E passes W64DevKit/QEMU runtime verification.
