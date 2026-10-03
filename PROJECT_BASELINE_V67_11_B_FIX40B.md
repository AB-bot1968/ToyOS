# ToyOS v67.11-B FIX40B

Base: runtime-confirmed FIX40A.

FIX40B changes test infrastructure only. TESTKEY.TST now covers spawn/argv and Ring3 #UD/#GP/#PF containment after the 4MT+8RT logical-key load test. `ASSERT LAST_MTDATA text` resolves LAST spawn PID to its retained MT slot and compares the kernel-owned MTDATA snapshot byte-for-byte. No scheduler policy, syscall ABI, RT/MT timing, watchdog policy, FAT16 layout, or kernel layout changes.
