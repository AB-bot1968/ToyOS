# ToyOS v67.11-B FIX48 — Safe Mode Policy

Base: runtime-confirmed FIX47.

FIX48 adds bounded SAFE-mode admission control without changing RT/MT scheduler priority or existing syscall numbers 0..65. SYS_SAFE_POLICY=66 reports policy flags and denial counters. In SAFE, new detached SPAWN, EXECMT and RT starts are denied; existing tasks are not killed automatically and foreground diagnostics remain usable. DEGRADED remains observational/advisory. TESTSFP.TST covers the policy and cleanup.


## FIX48A regression correction
TESTSFP.TST no longer uses the generic START RT directive for an intentionally denied SAFE-policy launch. The test now stages the normal RTD command and triggers F10 explicitly, so the expected policy denial is tested as an outcome rather than being counted as a TEST directive failure. Runtime policy and syscall ABI are unchanged.
