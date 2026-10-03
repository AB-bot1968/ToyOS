# FIX59 SONARSIM.EXE

Windows 7 / W64DevKit Modbus RTU slave для стенда ToyOS SONARDRV.

Сборка: `build_w64devkit.bat`. Скрипт также запускает `SONARSIM.EXE --selftest`.

Пример: `SONARSIM.EXE --port COM2 --baud 115200 --slave 1 --mode normal`.
Фиксированные координаты: `--xyz 123456 -654321 2000000000`.

Режимы: `normal`, `no-response`, `bad-crc`, `delay`, `partial`, `exception`.
`delay` задерживает полный ответ на `--delay-ms`; `partial` передает корректный кадр двумя частями с той же задержкой; `exception` возвращает Modbus exception code 4. По умолчанию XYZ динамически изменяются на каждом корректном запросе.

Контракт SONARDRV: slave=1, Function 04, start register=0, count=6; X/Y/Z — signed int32 mm, high 16-bit register first, big-endian bytes внутри регистра. Serial: 8N1, baud по параметру.

Для QEMU требуется связать гостевой COM1 с Windows COM-портом/виртуальной serial-парой; конкретная схема QEMU зависит от используемого запуска и не зашита в симулятор.

## FIX60Q: рекомендуемый TCP transport для QEMU/Windows

Для физической приемки ToyOS на Windows рекомендуется не `--port COM2`, а TCP
socket chardev QEMU, чтобы не входить в Windows native COM backend QEMU.

QEMU: `-chardev socket,id=sonar,host=127.0.0.1,port=4555,server,nowait,nodelay -device isa-serial,chardev=sonar,iobase=0x3f8,irq=4`

SONARSIM: `SONARSIM.EXE --tcp 127.0.0.1 4555 --slave 1 --mode normal --xyz 123456 -654321 2000000000`

Строка готовности TCP: `READY bounded-socket-io`. COM mode сохранен для
совместимости, но не является основным acceptance path FIX60Q.
