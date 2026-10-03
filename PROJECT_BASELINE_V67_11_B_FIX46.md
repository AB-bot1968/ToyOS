# ToyOS v67.11-B FIX46 baseline

Base: runtime-confirmed stable FIX45.

FIX46 adds `SYS_HW_WATCHDOG=64` without renumbering syscalls 0..63. The ABI provides status, arm, explicit feed and disarm. Backend 1 is deliberately an emulated QEMU/regression backend: it records watchdog control state but does not claim an independent physical reset. PIT/IRQ does not feed it. Future hardware-specific watchdog code must stay behind this ABI and must not turn timer activity into unconditional feeding.

Regression: `TESTHWWD.TST`; successful completion leaves the watchdog disarmed and MT/RT/resources clean.
