# ToyOS v67.11-B FIX12 baseline

Base: v67.11-B FIX11 RT_RELEASE_GRID_BUDGET (runtime-confirmed stable MISS/SKIP baseline).

FIX12 adds RT application-data snapshots without changing the FIX11 scheduling policy:
- syscall 50 `SYS_RT_DATA`;
- one opaque 255-byte payload per RT SLOT1..SLOT4 (256 bytes including NUL);
- publish is allowed only from the currently running detached RT task; kernel derives the slot;
- shell command `RTDATA SLOT1|SLOT2|SLOT3|SLOT4` reads the last complete snapshot;
- per-slot uint32 sequence is an internal commit/change counter; wrap is intentional;
- snapshot is cleared on slot start, STOP and RT exit;
- SENSOR1..SENSOR4 publish one `D1:...;...;D8:...;` record per job before SYS_RT_WAIT and never access the console.

Current diagnostic SENSOR fields:
D1=sensor id, D2=job sequence, D3=observed release interval (10-ms ticks),
D4=signed jitter, D5=release lateness, D6=dispatch latency,
D7=cumulative deadline misses, D8=cumulative skipped releases.

The kernel does not parse field names or values. Future sensors may publish any NAME:VALUE; payload up to 255 bytes.
