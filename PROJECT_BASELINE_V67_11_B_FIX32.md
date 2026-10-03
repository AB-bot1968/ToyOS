# ToyOS v67.11-B FIX32 — VGA foreground with RT/MT

Base: runtime-confirmed FIX31 CONSOLE MT PROGRESS.

Changes are intentionally local:
- SYS_VIDEO_MAP / SYS_VIDEO_TEXT / SYS_VIDEO_UNMAP identify the real foreground EXE1 caller by CR3 instead of treating `sched_active` as proof that the caller is MT.
- VGADRV ignores `SYS_CONSOLE_READ` return value 2, which is an internal scheduler hand-off rather than a key press.
- No scheduler policy, RT release/deadline accounting, MT quantum, syscall numbering, filesystem layout, or RT/MT data ABI was changed.

Preserved: RT8, MT25, MT 20 ms quantum, FIX11, FIX21, FIX26-FIX31, kernel 256 sectors, FAT16 at LBA 512.
