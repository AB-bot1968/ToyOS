# ToyOS v67.11-B-FIX10 baseline

Base: v67.11-B-FIX9.

Fix: IRQ0 now preempts any Ring-3 shell foreground activity when a detached RT job is READY. Previously only the SYS_CONSOLE_READ waiting case dispatched an RT job from the shell path; foreground RTSTAT/RTSTAT WATCH/command editing could therefore delay a 10-ms SENSOR job long enough to create false MISS/SKIP growth.

The build/ directory is intentionally excluded from the source archive. It contained generated host-side verification artifacts only and is not part of the ToyOS source tree.
