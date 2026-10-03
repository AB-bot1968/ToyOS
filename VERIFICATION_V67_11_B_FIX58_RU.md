# Проверка ToyOS v67.11-B FIX58

## Обязательный новый acceptance
`test TESTSON.TST` -> требуется FAIL=0.

TESTSON проверяет production SONAR decode + FIX57 parser: нормальный F04 из 6 регистров, X=123456, Y=-654321, Z=2000000000 mm, partial frame, bad CRC, exception response, неверное число регистров и чистое состояние системы.

## Регрессия
После TESTSON обязательно выполнить весь прежний набор FIX57, особенно TESTMB.TST, TESTDATA.TST, TESTUART.TST, TESTINF.TST, TESTRST.TST и TESTLAY.TST. Везде требуется FAIL=0.

## Host/static
Запустить `check_fix58_sonar.sh`, затем check_fix57_modbus.sh, check_fix56_data_channel.sh, check_fix55_uart.sh, check_fix54_infrastructure.sh и check_fix53_dynamic_layout.sh.

Architecture Vision изменён: добавлен контракт supervised Ring3 sensor driver и условие operational heartbeat после валидного sensor cycle.

Физический end-to-end COM/Modbus тест намеренно не объявляется доказанным в FIX58: он будет добавлен вместе с Windows SONARSIM на следующем этапе.
