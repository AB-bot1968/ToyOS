# ToyOS v67.11-B FIX56 — Sensor/Data Channel

FIX56 добавляет универсальный bounded Sensor/Data Channel для будущих Ring3-драйверов датчиков.

- syscall 71 `SYS_DATA_CHANNEL`;
- 4 стабильных channel ID;
- ring depth 8, payload до 32 байт;
- generation меняется при новом producer epoch;
- sequence и timestamp назначаются ядром;
- до 8 независимых reader cursors;
- явные состояния EMPTY / SAMPLE / OVERRUN / GENERATION;
- fixed-size storage, без FAT I/O и динамического выделения памяти;
- `TESTDATA.TST` проверяет publish/read, payload, независимых readers, overrun, generation restart, bad pointer и clean post-state.

Syscall 13/14 и FIX55A UART transport не изменены по смыслу. Modbus/SONARDRV в FIX56 не входят.
