# Проверка ToyOS v67.11-B FIX60U

## Host/static

1. `check_fix60u_test_layout.sh` — плоский `/TST`, полный `ACCEPT.TXT`, отсутствие root `.TST` и legacy source-ссылок.
2. `check_fix60c_fat83_names.sh` — FAT 8.3.
3. FIX60S/FIX60T gates — отсутствие регрессии SONARLOG/SONARVWR.
4. Изменённые `user_shell.c`, `leakfd.c`, `rconce.c` должны компилироваться freestanding i386.
5. `build.sh` и check scripts проходят syntax check.

## Runtime W64DevKit/QEMU

Собрать проект штатным `build.sh`, загрузить `build/toy_os.img` и выполнить `test`. Во время прогона консоль должна показывать текущий тест/диагностику, но итоговая статистика должна находиться в `/TST/TST.LOG`. После завершения выполнить `cat /TST/TST.LOG` и проверить `ACCEPTANCE: PASSED`.

Затем выполнить `test TESTTST.TST`, снова `cat /TST/TST.LOG` и убедиться, что журнал содержит только новый одиночный запуск. При обнаружении runtime FAIL версия не фиксируется как stable до исправления.

## Исправление остановки сборки после MT25.EXE
Причина: следующий после сборки MT25 этап `mkfat16` пытался поместить полный плоский `/TST` в один кластер каталога (16 directory entries, из них две заняты `.` и `..`). Исправлено: host `mkfat16` резервирует необходимое число кластеров каталога и связывает их FAT-chain. Синтетическая host-проверка с 45 файлами в одном `/TST` успешно создаёт FAT16 image. Runtime `fat_dir_lookup` в kernel уже поддерживает переход по FAT-chain каталогов.

После исправления TST.LOG полный `test` обязан завершиться PASS, а `cat /TST/TST.LOG` обязан открыть журнал и показать итог `ACCEPTANCE: PASSED`. `TESTTST.TST` дополнительно проверяет runtime create/reopen/delete внутри многокластерного `/TST`.

### Регрессия TST.LOG / TESTSUP
Статический gate `check_fix60u_tstlog_buffer.sh` проверяет, что подробный журнал одного .TST буферизуется в RAM и сбрасывается после теста, а TESTTST.TST сохраняет проверки TST_DIR_RW/TST_APPEND_RW. В QEMU требуется полный `test`, затем `cat /TST/TST.LOG`; отчёт должен содержать строки после TESTSUP.TST и завершаться `ACCEPTANCE: PASSED`.
