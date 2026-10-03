# Verification — ToyOS v67.11-B FIX60

## Обязательный новый тест
`test TESTLG60.TST`

Ожидается `FAIL=0` и `TEST EXPECT TELEMETRY_LOG: OK`.

Тест проверяет: PREAD/PWRITE, неизменность последовательной позиции, запрет роста через PWRITE, неизменность размера, CRC заголовка, выбор более нового поколения, переживание повреждения одного заголовка, CRC записи, обнаружение повреждения записи, wrap 128/модельный wrap, удаление временного файла и чистое состояние системы.

## Критическая регрессия
После TESTLOG60 выполнить TESTSIM, TESTSON, TESTMB, TESTDATA, TESTUART, TESTINF, TESTRST, TESTLAY, затем полный набор существующих TST. Все должны завершиться FAIL=0.

## Runtime SONARLOG
Физический QEMU/SONARSIM end-to-end остаётся отдельным стендовым испытанием. Автоматический FIX60 acceptance не заявляет, что внешний COM-канал уже испытан на железе.

## Architecture Vision
Обновлён: зафиксированы positional I/O, fixed-size cyclic log, redundant CRC headers и разделение logging/driver/IRQ paths.
