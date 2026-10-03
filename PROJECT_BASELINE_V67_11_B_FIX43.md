# ToyOS v67.11-B FIX43 — BOOT/RESET DIAGNOSTICS FOUNDATION

Base: runtime-confirmed stable FIX42.

FIX43 adds a persistent reason-for-reboot marker in `BOOTSTAT.DAT` (FAT 8.3). The Ring3 shell can write a validated marker with `bootdiag mark N`, display it with `bootdiag`, and remove it with `bootdiag clear`. On shell startup a valid marker is reported automatically. The record is versioned and checksummed. No filesystem I/O is added to IRQ, fault, scheduler, RT, or process-exit paths.

Reason codes: 1=software/planned reboot, 2=operator/external reset, 3=update/maintenance, 4=diagnostic/test. This FIX is the persistence/diagnostics foundation; it does not add a hardware reset syscall or claim to identify an unmarked power loss.

Automatic regression: `TESTBOOT.TST` validates create/read/update/clear and clean scheduler/session state. Existing scheduler policy, syscall numbering 0..62, FAT16 LBA512 and 256-sector kernel area are unchanged.
