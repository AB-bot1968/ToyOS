# ToyOS v67.11-B FIX54 — Infrastructure Hardening

Parent: FIX53 CURRENT STABLE.

FIX54 is intentionally an infrastructure-only release before UART/Modbus/Sensor Channel work.
It does not change the intentional Ring-3 I/O-port architecture (syscalls 13/14 remain unrestricted by per-process allowlists).

Changes:
- SAFE admission control now covers legacy SYS_MT_START=18 and increments the existing MT denial counter.
- Recovery Manager lifecycle transitions are kernel-validated: STOPPED requires a real terminated process result and no active PID/heartbeat; SPAWNED requires STOPPED state, a distinct live new PID; VERIFIED requires VERIFYING state, live PID and a published heartbeat.
- Administrative MT stop-one/all records PROCESS_EXIT_STOPPED, closes handles and clears heartbeat state.
- TST RUN is an action/setup directive and no longer inflates PASS merely because a command was dispatched.
- WAIT LAST increments PASS only after an actual process result is obtained.
- TESTINF.TST is the FIX54 focused runtime acceptance test.
