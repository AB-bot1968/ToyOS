# Verification ToyOS v67.11-B FIX55

## Обязательный новый runtime test
`test TESTUART.TST`

Он проверяет RX ordering, bounded capacity, overflow counter, TX ordering, bad user pointer, reset, serial timebase и чистое состояние системы. Test hooks проверяют transport semantics без внешнего COM; физический IRQ/QEMU COM будет отдельной интеграционной проверкой при создании драйвера.

## Полная регрессия
После TESTUART выполнить все тесты стабильного FIX54A, включая TESTINF, TESTLOAD, TESTKEY, TESTRST и TESTLAY.

## Host checks
- `check_fix55_uart.sh`
- `check_fix54_infrastructure.sh`
- `check_fix53_dynamic_layout.sh`

## Architecture Vision
Обновлён: добавлен принцип минимального IRQ transport — IRQ выполняет только bounded I/O buffering/status; протоколы остаются Ring3. Решение о свободном доступе Ring3 ко всем I/O ports через syscall 13/14 не изменено.
