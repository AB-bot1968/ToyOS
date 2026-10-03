# FIX53 — динамическая разметка дискового образа

## Физическая схема

Единственные фиксированные boot-области FIX53:

- LBA0 — BIOS boot sector;
- LBA1 — 512-байтный layout sector;
- первые 64 байта LBA1 — descriptor `LAY1`;
- оставшаяся часть LBA1 — компактный real-mode layout loader.

Kernel начинается с LBA2. Его фактическая длина, резерв роста, защитный gap,
начало FAT16 и размер образа больше не задаются независимыми константами в
boot/kernel/checker.

## LAY1 descriptor

Descriptor состоит из 16 little-endian DWORD:

| DWORD | Содержимое |
|---:|---|
| 0 | magic `LAY1` |
| 1 | version = 1 |
| 2 | header bytes = 64 |
| 3 | checksum: сумма всех 16 DWORD modulo 2^32 должна быть 0 |
| 4 | kernel LBA |
| 5 | kernel bytes |
| 6 | kernel sectors |
| 7 | kernel growth reserve, sectors |
| 8 | protected gap, sectors |
| 9 | FAT16 LBA |
| 10 | FAT16 volume sectors |
| 11 | complete image sectors |
| 12 | FAT alignment, sectors |
| 13 | required free FAT16 data percentage |
| 14 | kernel load address |
| 15 | kernel safe load limit |

## Build policy

Значения по умолчанию являются только политикой сборки и могут быть изменены
environment-параметрами:

- `KERNEL_GROWTH_PERCENT=25`;
- `KERNEL_RESERVE_MIN=64`;
- `LAYOUT_GAP_SECTORS=128`;
- `FAT_ALIGN_SECTORS=256`;
- `FAT_FREE_PERCENT=25`;
- `KERNEL_LOAD_LIMIT=0xA0000` в десятичном виде 655360.

Расчёт выполняется после получения `kernel.bin`. Growth reserve и protected gap
учитываются отдельно. FAT16 выравнивается вверх относительно конца обеих областей.

`KERNEL_LOAD_LIMIT` разрешается уменьшать для проверок/конкретной платформы, но
FIX53 не позволяет поднять его выше `0xA0000`: там начинается VGA aperture —
первая зарезервированная runtime-область. Более крупный kernel требует отдельного
изменения memory/loader architecture.

## Проверки

`layoutcheck` сверяет LAY1, FAT16 BPB, фактический размер файла, отсутствие
пересечений, FAT16 cluster range и требуемую долю свободных data clusters.
`check_fix53_layout_variants.sh` автоматически строит разные варианты образа и
проверяет как успешные перерасчёты, так и обязательный отказ при повреждённом
LAY1 и недопустимой memory boundary.

В ToyOS syscall `SYS_LAYOUT_INFO=69` предоставляет read-only копию descriptor.
`TESTLAY.TST` проверяет descriptor-driven invariants и реальную FAT16
create/write/read/delete операцию после загрузки.
