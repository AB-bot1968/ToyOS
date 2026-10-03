# ToyOS v67.11-B FIX45 SUPERVISOR POLICY

Based only on runtime-confirmed FIX44.

FIX45 connects the existing Ring3 supervisor to the FIX44 operational-state foundation without changing RT/MT scheduler policy. A worker fault or watchdog timeout that is eligible for restart moves the system to DEGRADED. Exhaustion of the existing bounded three-restart policy moves the system to SAFE. A healthy worker completion does not silently clear an already degraded/safe state; recovery to NORMAL remains an explicit operator/policy action.

Reasons: 4501 fault/restart -> DEGRADED, 4502 watchdog/restart -> DEGRADED, 4591 repeated fault exhausted -> SAFE, 4592 repeated watchdog exhausted -> SAFE.

No syscall renumbering. SYS_SAFE_MODE remains 63. No automatic RT/MT shutdown is added in this foundation step.
