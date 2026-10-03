# ToyOS v67.11-B FIX20 — RTD8

Base: FIX19 KERNEL256 (kept as rollback baseline).

Changes are limited to scaling the RT domain from 4 to 8 slots and extending the existing F10 pending launcher to eight entries.

- RT_MAX_TASKS = 8; SLOT1..SLOT8.
- RT page directories/tables, task descriptors, RTDATA and RTSTAT arrays scale through RT_MAX_TASKS.
- SENSOR1.EXE..SENSOR8.EXE are built from the same silent diagnostic source.
- F10 pending queue accepts at most eight RTD requests.
- RTSTAT/COMPARE/WATCH/DUMP/EVENTS/RESET/STOP and RTDATA accept/report all eight slots.
- Existing RT scheduler policy, release/deadline logic and syscall numbers 0..53 are unchanged.
- MT remains 25 tasks with the existing 20-ms quantum.
- Kernel area remains 256 sectors; FAT16 remains at LBA 512.
- RT physical image slots continue after all 25 MT image slots. With 8 RT slots the highest reserved RT image end is 0x02500000 (37 MiB); the existing runtime memory-size check remains authoritative.
- RT stacks use 0x003e0000 downward by 4 KiB per slot; SLOT8 uses 0x003d9000. These do not overlap the MT stack range immediately below 0x003fd000.

One correctness fix accompanies the scale-up: SYS_RT_STATUS now counts rt_tasks[] rather than sched_tasks[]. This prevents RT status/free-slot accounting from accidentally depending on the MT domain. MT_STATUS itself remains unchanged.
