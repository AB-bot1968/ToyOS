# ToyOS v67.11-B FIX60P — UART RX isolation / endurance

- Исправлен ложный `TESTUART TX DRAIN`: deterministic hooks теперь offline.
- Добавлен syscall70/op11 bounded UART diagnostic snapshot и shell `uartstat`.
- Добавлен 1000-cycle `TESTSONP.TST`; SONARTST не обращается к host COM.
- Добавлены `UARTRX.EXE` и `TESTRXP.TST` для физической изоляции RX от Modbus/Data Channel.
- Production SONARDRV сохраняет fully-polled UART, IER=0, PIC IRQ4 masked.
- Syscalls 13/14 и политика I/O-port не изменены.
