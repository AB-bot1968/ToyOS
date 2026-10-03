# Проверка ToyOS v67.11-B FIX60ZEI

## Область изменения

FIX60ZEI создан от FIX60ZEH. Runtime scheduler не изменён. `src/kernel.c` имеет SHA-256:

`3af4e5b2a9c8610eaee3efb1b533836dadf843e1b212922c9fee354c860dd001`

Он совпадает с FIX60ZEH/FIX60ZEG.

В `src/` изменён только `user_shell.c`: добавлен test-only ASSERT `MT_HEARTBEAT_IDLE_WAIT`. Старый `MT_HEARTBEAT_WAIT` сохранён для тестов, которым foreground busy-wait соответствует их контракту.

## TESTMRT

`TST/TESTMRT.TST` содержит 58 строк, как и FIX60ZEH. Номера диагностических строк не сдвинуты.

Строка 43:

`ASSERT MT_HEARTBEAT_IDLE_WAIT 2 12 800`

Строка 46:

`ASSERT MT_HEARTBEAT_IDLE_WAIT 2 24 1200`

Пороги heartbeat и timeout не уменьшались. `RT_SWEEP_FAIR 4 4`, `SONAR_LOG_MIN 8`, проверки активности и cleanup сохранены.

Fingerprint `TESTMRT.TST` FIX60ZEI:

- bytes: 1692
- FNV-1a32: 2456257148 (`0x9267827C`)

## Доступные проверки

- freestanding i386 compile: `kernel.c`, `user_shell.c`, `sonarlog.c`, `sonarpub.c`, `sonarview.c`, `sonardrv.c`, `rt_sensor.c`, `rt_sensor_diag.c` — PASS;
- stack usage `test_assert`: 320 bytes, то же значение, что FIX60ZEH;
- stack usage `test_run_file_console`: 112 bytes, то же значение, что FIX60ZEH;
- 10 host RT tests — PASS;
- 45 уникальных acceptance entries, все файлы существуют;
- все 45 acceptance TST входят в `mkfat16` payload;
- source-to-staged `cmp` guard FIX60ZEH сохранён;
- максимальная длина строки acceptance TST = 106 символов (`TESTSONN.TST`, строка 1), меньше буфера test runner 400;
- `sh -n build.sh` — PASS;
- полный `sh build.sh` в текущей среде штатно остановился на проверке toolchain: требуется x86 W64DevKit/i686 target;
- `TST.LOG`, `B*.TST`, packaged `build/` отсутствуют.

Полный QEMU/runtime acceptance в данной среде не заявляется. Окончательное подтверждение — запуск `test` в собранном ToyOS.
