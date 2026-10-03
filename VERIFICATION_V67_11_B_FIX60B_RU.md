# ToyOS v67.11-B FIX60B — финализация сборочного образа

Основа: FIX60A. Функциональная правка загрузчика EXE из FIX60A сохранена.

Исправление FIX60B: `build/toy_os.img` теперь создаётся только после полного набора legacy и
актуальных FIX54–FIX60A проверок. Финальный gate требует непустой `build/disk.img`, успешный
LAY1 layoutcheck, создание `build/toy_os.img`, байтовое равенство через `cmp`, повторный
layoutcheck финального образа и наличие в нём SONARDRV.EXE и TESTEXE.TST.

Добавлен `check_fix60b_build_finalization.sh`, запрещающий регрессию порядка финализации.
Architecture Vision обновлён: финальный boot image является проверяемым release artifact.
Syscall 13/14, UART, Modbus, Data Channel, FAT16 ABI и I/O policy не изменены.

Runtime acceptance: сначала TESTEXE.TST, затем полный регрессионный набор FIX60, затем
физический QEMU/com0com/SONARSIM запуск SONARDRV.EXE. До этого FIX60B не объявляется stable.
