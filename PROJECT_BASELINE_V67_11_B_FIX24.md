# ToyOS v67.11-B FIX24 baseline

Основа: FIX23. Сохраняются RTD=8, MT=25, MT quantum=20 ms, MTSTAT/MTSTAT WATCH, RTSTAT WATCH/ESC, ABI syscall 0..54, kernel area 256 sectors, FAT16 LBA 512.

Изменение FIX24 ограничено безопасным пакетным запуском RT через F10: SYS_RT_STATS op=7 временно откладывает background RT/MT dispatch, не останавливая IRQ0, часы и release accounting. После завершения F10 batch guard обязательно снимается. Алгоритмы выбора RT/MT и их приоритеты не изменены.
