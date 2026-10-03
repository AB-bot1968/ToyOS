# ToyOS Project Baseline — v67.11-A

## Рабочая точка

`v67.11-A — FILESYSTEM OFFSET`

## Зафиксировано

FAT16 volume start:

    LBA 512

Kernel slot:

    LBA 1..128

Reserved gap:

    LBA 129..511

Disk image:

    8192 sectors = 4 MiB

FAT16 BPB volume size:

    7680 sectors

## ABI

Syscalls and EXE1 ABI are unchanged. No new syscall is introduced by v67.11-A.

## Next planned stage

Следующим этапом поверх этой файловой базы планируется RT Background Tasks +
RTDATA, включая `rtstat stop`, `rtstat stop sensorN`, sliding statistics window
и публикацию произвольной строки прикладных данных от защищённой RT-задачи.
