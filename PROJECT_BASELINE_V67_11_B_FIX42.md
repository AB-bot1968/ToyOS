# ToyOS v67.11-B FIX42 — Persistent Event/Fault Log

Base: runtime-confirmed stable FIX41B.

FIX42 keeps the FIX41 bounded 64-entry kernel RAM event ring and adds an explicit, non-critical-path persistent snapshot in the Ring3 shell. `eventlog save` writes `EVENT.LOG` on FAT16; no filesystem I/O is performed from fault, IRQ, scheduler, RT, or process-exit paths. `eventlog disk` validates and displays the saved snapshot. `eventlog clear` affects RAM only; `eventlog clear disk` explicitly removes the persistent copy.

`EVENT.LOG` is a versioned binary format: magic, version, count, last sequence, FNV-1a checksum, followed by up to 64 unchanged FIX41 8-u32 event records (SEQ,TICK,PID,TYPE,REASON,STATUS,VECTOR,ERROR).

No syscall number or kernel scheduler policy changes were made. FAT16 remains at LBA 512; kernel reserved area remains 256 sectors.

Permanent automatic regression: `TESTLOG.TST`.
