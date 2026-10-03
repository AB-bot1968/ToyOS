# ToyOS v67.11-B FIX13 — separated RT/MT scheduler domains

Base: FIX12 RTDATA.

- RTD uses independent `rt_tasks[]`, RT page directories/tables, image slots and stacks.
- EXECMT keeps independent `sched_tasks[]`; it cannot overwrite RT SLOT1..SLOT4.
- EXECMT is background round-robin with a 20 ms CPU quantum (2 x 10 ms PIT ticks).
- RT has precedence over MT; RT execution does not advance the MT quantum counter.
- Shell remains runnable between MT quanta.
- Added `MTSTAT`, `MTSTOP MT1..MT25`, `MTSTOP ALL` (case-insensitive shell parser).
- Removed user-facing `rt-jitter` and `mt-test` commands/help. Historical syscalls/test helpers remain for ABI/regression compatibility.
- Existing RT syscalls 38..50 retain numbers. New MT management syscalls are 51/52.
- FIX12 RTDATA ABI remains unchanged.

Runtime validation in QEMU/W64DevKit is required before promoting this branch to stable.
