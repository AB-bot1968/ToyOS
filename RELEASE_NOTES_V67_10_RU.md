# Release Notes — ToyOS v67.10

## RT MONITOR / STATISTICS

v67.10 добавляет пользовательский монитор Stage 6.2 поверх v67.9 SENSOR RT
OBSERVABILITY.

### Добавлено

- `SYS_RT_STATS = 48`;
- kernel-side aggregate RT statistics;
- 8-entry per-RT-slot completed-job ring buffer;
- `rtstat`;
- `rtstat compare`;
- `rtstat watch`;
- `rtstat dump`;
- `rtstat reset`;
- режим `VERBOSE` для SENSOR через RTD;
- SENSOR1..SENSOR4 по умолчанию работают без высокочастотного console output.

### Сохранено

- syscalls 1..47;
- RT scheduler и его priority/deadline/release semantics;
- EXE1 ABI;
- регистронезависимый ввод команд;
- SENSOR1..SENSOR4 и их kernel-side timing measurements;
- v67.7 DELETE/LS FIX и все более ранние рабочие исправления.

### Проверка

`check136.sh` проверяет наличие syscall 48, monitor commands, VERBOSE support,
SENSOR1..SENSOR4 и freestanding source-level audit. Полная i386-сборка должна
выполняться тем же W64DevKit/i686 toolchain, который используется исходным
`build.sh`.
