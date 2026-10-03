# ToyOS v67.11-B FIX40 SOFTWARE WATCHDOG / HEARTBEAT

Base: runtime-confirmed stable FIX39.

FIX40 adds a bounded per-PID heartbeat registry and supervisor timeout recovery without changing RT scheduling, MT 20-ms quantum, or priority policy.

New ABI:
- SYS_PROCESS_HEARTBEAT = 60: op selected by ECX/PID. PID=0 publishes caller heartbeat; PID!=0 queries {pid,last_tick,seq}.
- SYS_PROCESS_STOP_PID = 61: MT supervisor may stop another live MT PID. Timeout termination is retained as reason WATCHDOG(3), status 124.

SUPERVIS.EXE now watches both retained process results and heartbeat freshness. Timeout is 100 legacy timer ticks (2 s). Fault and watchdog restart share the existing bounded limit of three restarts. No infinite restart loop is introduced.

Diagnostics: HBEATOK.EXE publishes heartbeats and exits normally; HANGWD.EXE intentionally never publishes a heartbeat.
Regression: TESTHB.TST and TESTWD.TST.
