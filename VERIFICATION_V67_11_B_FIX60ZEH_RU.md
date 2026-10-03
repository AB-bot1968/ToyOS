# Проверка ToyOS v67.11-B FIX60ZEH

Проверяемые инварианты:

1. `src/kernel.c` идентичен FIX60ZEG; scheduler/runtime policy не изменялись.
2. `CONSOLE_IDLE_TICKS` принимает код 3 только как одноразовый redraw-event и продолжает ждать реальные idle ticks.
3. Код 2 остаётся единственным idle-shell token; любые другие коды дают FAIL.
4. При FAIL runner выводит bytes + FNV-1a32 фактически прочитанного TST-файла.
5. `RUN` rejection имеет явную диагностическую строку.
6. build-stage TST-файлы сравниваются с исходными через `cmp`.
7. `TESTMRT.TST` остаётся acceptance-тестом без снижения MT/RT требований.
8. Число acceptance-тестов остаётся 45.
9. `TST.LOG`, `B*.TST` и `build/` не входят в релизный архив.

Эталон FIX60ZEH TESTMRT:

- bytes: 1685
- FNV-1a32: 3848550741 (0xE5643955)

## Выполненные проверки в текущей среде

- `src/kernel.c`: SHA-256 `3af4e5b2a9c8610eaee3efb1b533836dadf843e1b212922c9fee354c860dd001`, полностью совпадает с FIX60ZEG.
- Freestanding i386 compile (`-m32`) успешно: `kernel.c`, `user_shell.c`, `sonarlog.c`, `sonarpub.c`, `sonarview.c`, `sonardrv.c`, `rt_sensor.c`, `rt_sensor_diag.c`.
- Все 10 host-тестов `tools/test_rt_*.c` успешно выполнены.
- `TST/ACCEPT.TXT`: 45 записей, 45 уникальных, отсутствующих файлов нет.
- Все 45 acceptance-файлов присутствуют в FAT payload `build.sh`.
- Максимальная длина строки acceptance TST = 106 символов (`TESTSONN.TST`, строка 1), что существенно меньше буфера 400.
- `sh -n build.sh`: PASS.
- Полный `sh build.sh` в этой Linux-среде штатно остановлен проверкой toolchain: требуется x86 W64DevKit/i686 target. QEMU/runtime PASS не заявляется.
- Stack-usage контроль: `execute_line` 1584 B, `test_run_file_console` 112 B, `rtstat_command` 96 B; новых крупных stack-buffer не добавлено.
- `build/` после проверок удалён.
