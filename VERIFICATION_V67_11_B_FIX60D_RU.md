# ToyOS v67.11-B FIX60D — публикация загрузочного образа

FIX60D исправляет регрессию FIX60B/C: `build/toy_os.img` больше не откладывается до окончания всех legacy regression checker'ов.

После сборки `build/disk.img`, записи boot/layout/kernel, `layoutcheck` и `check_fix53_layout_variants.sh` образ атомарно публикуется через `build/toy_os.img.tmp`. После всей цепочки regression checker'ов он повторно атомарно обновляется через `build/toy_os.img.final` и проверяется `cmp`, `layoutcheck` и `fat16check`.

Ошибка любого checker'а по-прежнему означает ошибку всей сборки. Ранний образ предназначен для сохранения уже успешно собранного диагностического артефакта, а не для объявления релиза успешным.

FIX60C FAT 8.3 guard и имя `TESTEXE.TST` сохранены. Syscall 13/14, UART, Modbus RTU, Data Channel и политика I/O-портов не изменены. Architecture Vision по существу не менялась: изменение относится к build/release pipeline.
