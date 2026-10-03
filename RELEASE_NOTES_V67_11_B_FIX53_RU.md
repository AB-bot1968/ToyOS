# ToyOS v67.11-B FIX53 — Dynamic Disk Layout

FIX53 прекращает экспериментальную ветку COMDRV_: `src/comdrv_.c`, `COMDRV.CFG` и `TESTCOM.TST` удалены. Исходный `COMDRV.EXE`/`src/comdrv.c` сохранён байт-в-байт относительно FIX52D.

Главное изменение — единый LAY1 descriptor в LBA1. Boot знает только фиксированный metadata sector LBA1; находящийся там layout-loader проверяет descriptor и загружает фактическое число секторов kernel из descriptor-defined LBA. Kernel получает FAT16 LBA из той же runtime-копии. `fat16check` и `layoutcheck` также читают LAY1.

После создания `kernel.bin` build вычисляет фактическое число kernel sectors, отдельный configurable growth reserve, отдельный protected gap и aligned FAT16 LBA. `mkfat16` рассчитывает FAT16 geometry и полный размер образа по фактическому набору файлов и требуемой доле свободных data clusters. Фиксированный рабочий FAT LBA 512 и фиксированный 8192-sector image contract удалены.

Build отдельно проверяет raw kernel end и linker BSS end. `KERNEL_LOAD_LIMIT` может быть уменьшен, но не поднят выше `0xA0000` без отдельного изменения memory architecture.

Новый read-only diagnostic ABI: `SYS_LAYOUT_INFO=69`; shell-команда `layout`; runtime acceptance `TESTLAY.TST`. Host acceptance `check_fix53_layout_variants.sh` проверяет различные layout/file sizes, автоматическое изменение образа, отказ на повреждённом descriptor и отказ при нарушении memory limit.

Port-I/O contract, RT/MT scheduler policy и FIX51 recovery policy не изменены.
