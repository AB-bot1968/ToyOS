# ToyOS v67.11-B FIX41A — EVENT LOG 8.3 BUILD FIX

FIX41A is a minimal build correction over FIX41 EVENT LOG FOUNDATION.

- Renamed `TESTEVENT.TST` to FAT 8.3-compatible `TESTEVT.TST` (`TESTEVENT` was 9 characters).
- Updated build image insertion, qname verification, static checker and verification documentation.
- Added a static audit of every explicit FAT destination in the `mkfat16.exe` command for 8.3 compliance.
- No scheduler, RT/MT policy, syscall ABI, event-log semantics, watchdog, filesystem layout or kernel layout changes.
- Runtime-confirmed FIX40B remains rollback baseline until FIX41A passes runtime verification.
