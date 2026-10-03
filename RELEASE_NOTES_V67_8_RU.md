# ToyOS v67.8 — Release Notes

## Stage 6.2 — RT release jitter measurement

База: **ToyOS v67.7 — DELETE/LS FIX**.

### Добавлено

- измерение фактического момента обслуживания RT release;
- фактический интервал между последовательными release;
- min/max интервала;
- текущая и максимальная задержка release;
- задержка от release до первого dispatch;
- `SYS_RT_JITTER_INFO = 46`;
- исполняемый диагностический `JITTER.EXE`;
- команда shell `rt-jitter` для просмотра текущих измерений;
- host arithmetic test `tools/test_rt_jitter.c`;
- автоматическая проверка `check134.sh`.

### Регрессии

- существующие syscall 1..45 не изменены;
- RT scheduler policy не изменена;
- period/deadline conversion Stage 6.1 не изменена;
- history command mechanism не изменён;
- case-insensitive command parsing сохраняется как обязательное требование;
- параметры COMDRV/NETDRV/VGADRV/LOADER/RTD продолжают нормализоваться по прежним правилам.

### Статус

`check134.sh` — PASS.
