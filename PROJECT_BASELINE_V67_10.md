# ToyOS Project Baseline — v67.10

Рабочая база: v67.9 SENSOR RT OBSERVABILITY.

v67.10 добавляет только RT monitor/statistics поверх неё:

- syscall 48 `SYS_RT_STATS`;
- aggregate statistics;
- 8-sample kernel ring;
- `rtstat`, `rtstat compare`, `rtstat watch`, `rtstat dump`, `rtstat reset`;
- SENSOR quiet-by-default;
- RTD `VERBOSE` mode.

Номера syscall 1..47 не меняются. Командный ввод ASCII case-insensitive.
