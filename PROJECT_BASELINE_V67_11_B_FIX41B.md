# ToyOS v67.11-B FIX41B — test session cleanup

Baseline: runtime-tested FIX41A event log candidate, with all functional tests reported PASS.

FIX41B corrects regression-test cleanup only. TESTHB.TST, TESTWD.TST, TESTSUP.TST and TESTRES.TST now explicitly close the retained EXECMT/spawn session with `mtstop ALL` after their final assertions. TESTKEY.TST, TESTLOAD.TST and TESTEVT.TST already close their sessions. This prevents a successful full regression from leaving `mt_session_active` set and blocking a later manual `execmt`.

No scheduler policy, syscall ABI, RT/MT timing, watchdog policy, event-log format, FAT16 layout or kernel layout is changed.
