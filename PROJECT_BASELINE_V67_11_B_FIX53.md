# ToyOS v67.11-B FIX53 baseline

Base: FIX52D source tree, with the experimental COMDRV_ branch deliberately removed. Legacy `COMDRV.EXE` remains unchanged.

FIX53 introduces descriptor-driven disk layout: LBA0 boot, LBA1 LAY1 descriptor/layout-loader, kernel from LBA2, dynamic FAT16 LBA and dynamic FAT16/image sizing. Build policy defaults: 25% kernel growth reserve with 64-sector minimum, 128-sector protected gap, 256-sector FAT alignment, 25% free FAT16 data area. All are configurable build parameters.

CURRENT STABLE remains FIX51 until FIX53 runtime/regression acceptance passes.
