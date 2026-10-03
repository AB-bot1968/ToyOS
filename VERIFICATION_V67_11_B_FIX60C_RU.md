# ToyOS v67.11-B FIX60C — FAT 8.3 packaging correction

Основа: FIX60B. Исправлен дефект имени нового автоматического теста: предыдущее имя нового теста нарушало FAT 8.3 (9 символов до точки). Тест переименован в TESTEXE.TST во всех активных ссылках build/check/verification.

Добавлен check_fix60c_fat83_names.sh. Он до упаковки проверяет назначения активной команды mkfat16 на соответствие FAT 8.3. Историческое AUTOSTART.SH остаётся явным специальным LFN-исключением существующей системы.

Функциональные изменения FIX60A executable-loader и FIX60B final-artifact gate сохранены. Syscall 13/14, UART, Modbus RTU, Data Channel и I/O policy не менялись. TOYOS_ARCHITECTURE_VISION.md по существу архитектуры не изменён: исправление относится к build/test discipline.

Runtime acceptance: test TESTEXE.TST, затем полный регрессионный набор FIX60, затем физический SONARDRV + QEMU + com0com + SONARSIM.
