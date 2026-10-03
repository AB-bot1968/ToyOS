# Verification — ToyOS v67.11-A

## Выполнено в текущем окружении

### 1. Host FAT16 tool compilation

Успешно скомпилированы без предупреждений при `-Wall -Wextra -Werror`:

- `tools/mkfat16.c`
- `tools/fat16check.c`

### 2. FAT16 image geometry

Тестовый образ построен с:

- total image = 8192 sectors;
- FAT16 start = LBA 512;
- FAT16 BPB total sectors = 7680;
- FAT1 = LBA 513;
- FAT2 = LBA 545;
- root = LBA 577;
- data = LBA 609.

### 3. Reserved gap

LBA 129..511 содержит 383 сектора и не пересекается с FAT16.

### 4. FAT16 functional check

Проверены:

- root file;
- nested `BIN/HELLO.EXE`;
- nested `DOC/NET.CFG`;
- FAT16 cluster chain.

`CHECK137 PASS`.

## Полная i386-сборка

Полная `build.sh` в текущем Linux-окружении не запускалась до конца, потому что
проект требует исходный 32-bit W64DevKit/i686 toolchain и `build.sh` намеренно
останавливается при `gcc -dumpmachine != i686-*`.

Поэтому отсутствие полной i386-сборки в этом окружении не трактуется как PASS.
Она должна быть выполнена на штатном W64DevKit.

## Новая regression-проверка

`check137.sh` проверяет:

- FAT16 BPB на LBA 512;
- kernel slot 128 sectors;
- reserved gap 383 sectors;
- BPB total = 7680 sectors;
- согласованность `src/kernel.c`, `tools/mkfat16.c`, `tools/fat16check.c`;
- доступность root и nested FAT16 entries.
