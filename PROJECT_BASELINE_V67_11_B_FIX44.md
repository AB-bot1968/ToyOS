# ToyOS v67.11-B FIX44 baseline

Base: runtime-confirmed stable FIX43.

FIX44 adds the kernel operational-state foundation: NORMAL=0, DEGRADED=1, SAFE=2, with reason, transition sequence and tick. SYS_SAFE_MODE=63 provides structured read/set access from Ring3. This release deliberately does not change scheduler policy or automatically suppress workloads: policy enforcement belongs to a later step after the state mechanism is runtime-confirmed.

Permanent regression: TESTSAFE.TST. Existing ABI 0..62 is unchanged. FAT16 remains at LBA 512 and kernel area remains 256 sectors.
