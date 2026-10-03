# ToyOS v67.11-B FIX57 — Modbus RTU Ring3

FIX57 добавляет общий production-модуль `src/modbus_rtu.c` для будущих Ring3-драйверов.

- CRC16 Modbus RTU.
- Формирование запроса Function 04 Read Input Registers.
- Разбор нормального ответа Function 04 с big-endian 16-bit registers.
- Явные результаты partial frame, CRC error, malformed frame и Modbus exception.
- Bounded timeout/retry state machine без бесконечного ожидания.
- Modbus не добавлен в kernel и не выполняется в IRQ.
- SYS_PORT_IN8/OUT8, FIX55 UART transport и FIX56 Data Channel не изменены.
- `TESTMB.TST` тестирует тот же Ring3-модуль без внешнего COM.
