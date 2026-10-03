# ToyOS v67.11-B FIX55A — UART TRANSPORT

Исправление FIX55 после runtime TESTUART PASS=9 FAIL=1.

Причина: обычный TX enqueue включал THRE interrupt даже при выключенном buffered transport. Поскольку IRQ4 уже разрешён PIC после boot, QEMU UART мог аппаратно потребить тестовый TX ring между test-hook операциями. Детерминированный TESTUART тем самым гонялся с физическим UART.

FIX55A:
- THRE interrupt включается только когда uart_transport_enabled=1;
- test TX queue/drain при transport disabled не взаимодействует с hardware IRQ;
- test drain не меняет IER при выключенном transport;
- EXPECT UART_TRANSPORT теперь печатает точный этап внутренней ошибки;
- syscall 13/14 и legacy COMDRV semantics не изменены.
