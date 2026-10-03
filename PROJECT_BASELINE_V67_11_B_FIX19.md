# ToyOS v67.11-B FIX19 — 256-sector kernel area

Base: FIX18 RTSTAT 6H WINDOW.

## Architectural change
- Kernel disk slot: LBA 1..256 (256 sectors = 128 KiB).
- FAT16 remains fixed at LBA 512.
- Reserved gap is now LBA 257..511 (255 sectors).
- Bootloader loads the complete 256-sector slot contiguously to physical 0x07E00..0x27DFF.
- Five EDD/DAP transfers are used: 1 + 64 + 64 + 64 + 63 sectors. No transfer crosses a 64-KiB DMA boundary.
- build.sh rejects kernel.bin above 256 sectors and reports actual/maximum sectors.
- Disk image reserves/zeros LBA 1..256 before overlaying actual kernel.bin.
- FAT16 LBA and volume layout are otherwise unchanged.

## Important compatibility point
The destructive disk syscall self-test scratch sector was moved from LBA 65 to LBA 300. LBA 65 is now inside the enlarged kernel slot; LBA 300 is inside the remaining reserved gap and outside FAT16.

## Unchanged
FIX18 RTSTAT 6-hour window, RT/MT scheduler policy, RTDATA/MTDATA ABI, syscall numbers, 20 ms MT quantum, EXE1 layout and FAT16 start LBA 512 are unchanged.
