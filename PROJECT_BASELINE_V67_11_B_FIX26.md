# ToyOS v67.11-B FIX26 — RT/MT quantum return

Base: FIX25.  This release fixes the confirmed 4MT + 8RT console starvation case.

The MT interval is now charged to both MTSTAT and the actual 20-ms scheduler quantum before RT selection.  If RT preempts MT exactly when that quantum expires, the MT context is saved READY, RT still runs first, and the RT chain returns to the saved shell context instead of resuming the expired MT quantum forever.

Unchanged: RT priority over MT, RT scheduler release/deadline policy, 8 RT slots, 25 MT slots, MT quantum=2 PIT ticks=20 ms, syscalls 0..54, FIX24 F10 launch guard, FIX25 console return capture, FAT16 LBA512, 256-sector kernel area.
