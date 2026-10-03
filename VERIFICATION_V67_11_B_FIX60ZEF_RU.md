# Проверка ToyOS v67.11-B FIX60ZEF

В доступной среде выполнены следующие проверки исходников:

- `src/kernel.c` и `src/user_shell.c` успешно компилируются GCC в режиме `-m32 -ffreestanding`.
- Также успешно компилируются `sonarlog.c`, `sonarpub.c`, `sonardrv.c`, `sonarview.c`, `rt_sensor.c`, `rt_sensor_diag.c`.
- Все host-модели `tools/test_rt_*.c` собраны и завершились с кодом 0.
- `check_fix60zef_foreground_ownership.sh` проходит.
- В `TST/ACCEPT.TXT` ровно 45 уникальных тестов; все файлы существуют и присутствуют в FAT payload `build.sh`.
- Максимальная длина строки acceptance-тестов — 106 символов, что ниже буфера test runner.
- `TST.LOG` и `B*.TST` отсутствуют.
- Каталог `build/` перед упаковкой удалён.

Полный `build.sh` в текущей среде не выполнен: установленный GCC имеет не i686 target, а `build.sh` корректно прекращает сборку с сообщением `ERROR: use the x86 W64DevKit (gcc target must be i686).` Поэтому QEMU/runtime PASS в этом документе не заявляется.

Новая runtime-проверка `ASSERT CONSOLE_IDLE_TICKS 8` добавлена в `TESTMRT.TST` и `TESTVWR.TST`; она предназначена именно для обнаруженного сценария потери консоли после `rtstat` при активных RT+MT.
