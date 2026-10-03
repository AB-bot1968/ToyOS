# Verification — ToyOS v67.11-B FIX60R

## Причина
В FIX60Q `TESTLG60.TST` проверял формат telemetry log, но не запускал `SONARLOG.EXE`. Реальный logger мог завершиться с status 9 при единственной неудачной попытке открыть Data Channel reader. Кроме того, reader не имел PID owner и при принудительном `mtstop` не освобождался, поэтому повторные запуски могли исчерпать 8 reader slots.

## Изменения
Изменены только `src/sonarlog.c`, минимальный generic Data Channel resource cleanup в `src/kernel.c`, тестовые ASSERT в `src/user_shell.c`, build/test/docs. Production `src/sonardrv.c`, `src/uartrx.c` и `tools/sonarsim/sonarsim.c` сохранены без изменений относительно FIX60Q STABLE.

Контрольные SHA256 неизменённых production-файлов:
- src/sonardrv.c: d63f11028e836b93e341ae00ed13f31ee0ec3f6b7eee1ccf52c2a661eadbdff9
- src/uartrx.c: 6264bb7eb3a07785da20570e4cc699e36c64e5be588d9d04e72d597ba90fd6fb
- tools/sonarsim/sonarsim.c: e38af943c280b88e84b4817e5c28a2481e32bf7c10f81e3f07b1d9acb61bf3ab

## Новый автоматический тест
`test TESTLGR.TST`

Тест:
1. создаёт заведомо неправильный `/SONAR.LOG` длиной 3 байта;
2. запускает `SONARTST.EXE SONARLOG.EXE` как две реальные MT-задачи;
3. ждёт 1000 публикаций producer;
4. ждёт heartbeat logger >=130, что доказывает не менее 129 committed records и переход через capacity=128;
5. проверяет размер `/SONAR.LOG` = 4160;
6. многократно stop/restart выполняется больше 8 раз, чтобы обнаружить утечку reader table;
7. отдельно запускает `SONARLOG.EXE SONARTST.EXE`, то есть consumer раньше producer, и требует heartbeat обеих задач;
8. завершает тест без MT-задач, process FAT handles, SAFE/HWWD/health загрязнения.

## Регрессия
Перед физическим запуском рекомендуется минимум:
- test TESTLG60.TST
- test TESTDATA.TST
- test TESTSONP.TST
- test TESTLGR.TST
- test TESTUART.TST
- test TESTRXP.TST

После этого физическая проверка остаётся прежней:
`execmt SONARDRV.EXE SONARLOG.EXE` через QEMU TCP socket + SONARSIM.

## Architecture Vision
Обновлена секцией FIX60R: зафиксированы независимый startup consumer, PID-owned Data Channel readers, bounded recovery повреждённого telemetry file и неизменность рабочего SONAR transport path.

Полная W64DevKit/QEMU runtime-приёмка должна быть выполнена пользователем; локальные static/compile checks её не заменяют.
