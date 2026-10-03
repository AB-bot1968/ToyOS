# ToyOS v67.11-B FIX25 — RT/MT console return-context fix

Base: FIX24. This release keeps the established scheduler policy and ABI.

## Change
`SYS_CONSOLE_READ` can directly hand the waiting Ring-3 shell to a ready RT task. FIX24 saved the shell interrupt frame there but did not refresh `rt_return_cr3`. If the previous RT dispatch had interrupted an MT task, the stale value could still point at that MT address space. After the F10 batch completed, an RT task could therefore return from `SYS_RT_WAIT` to MT instead of to the shell that was waiting for keyboard input. With eight 10-ms RT tasks this could repeat indefinitely, leaving a visible `toy0>` prompt and blinking cursor while shell input made no progress.

FIX25 stores the current CR3 whenever `SYS_CONSOLE_READ` directly dispatches RT. It also records the same first-dispatch timing fields as the normal `rt_switch_to()` path, keeping RTSTAT diagnostics consistent.

Unchanged: RT priority/policy, 8 RT slots, 25 MT slots, MT 20-ms quantum, syscall numbers 0..54, RTDATA/MTDATA, RTSTAT/MTSTAT, F10 launch guard, 256-sector kernel area, FAT16 at LBA 512.
