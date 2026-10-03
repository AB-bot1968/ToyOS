# ToyOS v67.11-B FIX60 — Cyclic Telemetry Log

Статус: CANDIDATE. Основа: FIX59 SONARSIM CURRENT STABLE.

FIX60 добавляет долговременный bounded журнал сонарной телеметрии без файлового I/O в IRQ/Modbus/Data Channel paths.

- SYS_FILE_PREAD=72: чтение по явному offset без изменения позиции handle.
- SYS_FILE_PWRITE=73: запись по явному offset без изменения позиции handle; запись за текущий EOF запрещена.
- SONARLOG.EXE: независимый Ring3 consumer Data Channel 0.
- SONAR.LOG: 4160 байт = 2*32 байта headers + 128*32 байта records.
- Два CRC32-защищённых заголовка A/B. Запись commit: record -> alternate header.
- Record: sequence, timestamp_us, X/Y/Z int32 mm, status, producer generation, CRC32.
- TESTLG60.TST: positional I/O, сохранение позиции, no-growth, header CRC/select/corruption, record CRC/corruption, wrap, clean-state.

FIX60 не изменяет syscall 13/14, UART, Modbus RTU, SONARDRV или политику SAFE/Recovery.


### FIX60U: verified chunk append для TST.LOG
QEMU-приёмка выявила воспроизводимый отказ большой buffered append-записи: `/TST/TST.LOG` оставался частично записанным (до `TEST PASS line 16` в `TESTSUP.TST`), а `SYS_FILE_WRITE` возвращал ошибку. Предыдущие retry и RAM buffering не устраняли этот класс отказа. Runner больше не передаёт весь отчёт одного `.TST` одной растущей FAT-записью: flush делится на 128-байтовые append-транзакции, и после каждой транзакции проверяется фактический размер directory entry. Неоднозначный частичный commit никогда не повторяется вслепую. `TESTTST.TST` содержит `TST_APPEND_STRESS`: 48 независимых append по 128 байт (6144 байта) с проверкой размера после каждого шага и полным read-back. ABI, FAT16 on-disk format и рабочие механизмы supervisor/SONAR/EXECMT не изменены.
