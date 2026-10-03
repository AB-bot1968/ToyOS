# ToyOS v67.11-B FIX55 — UART TRANSPORT

База: FIX54A CURRENT STABLE.

FIX55 добавляет bounded IRQ-driven transport для COM1 (16550) как фундамент будущего SONARDRV/Modbus RTU.

- IRQ4 / INT36 обслуживает только перенос байтов и аппаратного статуса в/из фиксированных ring buffers.
- RX/TX buffers: 256 bytes, без malloc.
- Счётчики RX/TX, RX overflow, line errors, IRQ count и timestamps.
- SYS_UART_TRANSPORT=70: info/read/write/reset/time/enable и детерминированные test hooks.
- Serial timebase получает микросекундное значение из текущего PIT 100 Hz + аппаратного down-counter; частота scheduler не повышалась.
- Transport выключен при boot. Это сохраняет polling-поведение legacy COMDRV и свободный SYS_PORT_IN8/SYS_PORT_OUT8.
- Modbus и Sensor Channel в FIX55 намеренно не реализуются.
- TESTUART.TST проверяет ring semantics без внешнего COM.
