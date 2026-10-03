# Verification v51

Добавлен COMDRV.EXE1 — первый драйвер COM.

Проверить в Windows 7 + W64DevKit + QEMU:

- `ls` содержит `COMDRV.EXE`;
- `exec COMDRV.EXE 1 SEND TEST.TXT` передаёт весь файл;
- `exec COMDRV.EXE 1 RECV RX.TXT` создаёт RX.TXT и завершает при тайм-ауте;
- `execq 2 COMDRV.EXE 1 SEND TEST.TXT` выполняет две последовательные передачи;
- `execq 2 COMDRV.EXE 1 RECV RX.TXT` выполняет два последовательных приёма;
- старый `execq REPEAT FILE1 FILE2` не изменён;
- `exec`, `execq`, `execmt`, `ls`, FAT16 и существующие syscall ABI сохранены;
- прямых IN/OUT в COMDRV нет, используются syscall 13/14.

Полная сборка должна выполняться штатным Windows 7 W64DevKit `build.bat`/`build.sh` и запускаться в QEMU.
