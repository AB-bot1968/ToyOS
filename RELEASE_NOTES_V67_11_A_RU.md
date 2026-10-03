# Release Notes — ToyOS v67.11-A

## FILESYSTEM OFFSET

v67.11-A подготавливает файловую систему к следующему этапу RT Background Tasks + RTDATA.

### Изменение

FAT16 теперь начинается с **LBA 512** вместо LBA 130.

### Раскладка

```text
LBA 0          BIOS boot sector
LBA 1..128     kernel slot (128 sectors)
LBA 129..511   reserved gap (383 sectors)
LBA 512       FAT16 boot sector
LBA 513...     FAT16
```

Размер disk image не изменён: 8192 секторов.

### Сохранено

- 32-bit i386 freestanding architecture;
- protected mode;
- существующий syscall/ABI;
- загрузка kernel из LBA 1;
- 128-sector kernel limit;
- FAT16 filesystem implementation;
- case-insensitive command input;
- v67.7 DELETE/LS FIX;
- RT foundation v67.8/v67.9/v67.10.

### Важно

v67.11-A **не реализует ещё RTDATA или фоновые SENSOR-задачи**.
Это отдельный следующий этап. Сейчас меняется только дисковая раскладка и
связанные с ней проверки.
