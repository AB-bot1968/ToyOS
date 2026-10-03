# Проверка v55 — COMDRV ABI и полный разбор `exec` / `execq`

## Причина ошибки `invalid COM port`

Строка QEMU:

```bat
d:\qemu\qemu-system-i386.exe -drive format=raw,file=toy_os.img -chardev file,id=comlog,path=com1_tx.bin -device isa-serial,chardev=comlog,iobase=0x3f8,irq=4
```

для COM1 корректна. Она не могла сама по себе превратить номер порта `1` в ошибку `invalid COM port`: это сообщение выдавал уже `COMDRV.EXE` до обращения к UART.

Реальная проблема находилась в передаче трёх аргументов EXE1. `program_main()` извлекал EBX/ECX/EDX тремя независимыми inline-asm блоками с неопределёнными для компилятора регистрами. При оптимизации это нарушало ABI: компилятор мог использовать промежуточный регистр не так, как предполагал исходный код.

В v55 аргументы забираются одним asm-блоком с явными constraints:

- EBX -> `port`;
- ECX -> `dir`;
- EDX -> `file`.

Таким образом, `exec COMDRV.EXE 1 SEND TEST.TXT` получает именно строки `1`, `SEND`, `TEST.TXT`.

## Проверенный разбор команд

Прямой запуск:

```text
exec COMDRV.EXE 1 SEND TEST.TXT
exec COMDRV.EXE 1 RECV TEST.TXT
exec COMDRV.EXE COM1 SEND TEST.TXT
exec COMDRV.EXE COM1 RECV TEST.TXT
```

Конечный shell-parser передаёт helper уже после префикса `exec COMDRV.EXE `.

Обычная очередь:

```text
execq 1 COMDRV.EXE 1 SEND TEST.TXT
execq 2 COMDRV.EXE 1 SEND TEST.TXT
execq 1 COMDRV.EXE 1 RECV TEST.TXT
```

Бесконечная очередь:

```text
execq forever COMDRV.EXE 1 SEND TEST.TXT
execq forever COMDRV.EXE 1 RECV TEST.TXT
```

Для `execq N COMDRV.EXE ...` helper получает исходную строку после `execq ` и сам разбирает `N COMDRV.EXE PORT DIRECTION FILE`.

Для `execq forever COMDRV.EXE ...` helper получает строку после `execq forever `; это `COMDRV.EXE PORT DIRECTION FILE`.

## QEMU

Для `SEND` текущая команда с `-chardev file` подходит: все байты, отправленные гостем через COM1, записываются в `com1_tx.bin`.

```bat
d:\qemu\qemu-system-i386.exe -drive format=raw,file=toy_os.img -chardev file,id=comlog,path=com1_tx.bin -device isa-serial,chardev=comlog,iobase=0x3f8,irq=4
```

`irq=4` оставлен для стандартного COM1, хотя текущий COMDRV работает polling-методом и UART IRQ ему не требуется.

Для двунаправленного `RECV` нужен двунаправленный backend, например TCP socket; file backend предназначен для записи TX и не является каналом ввода гостю.

## Проверки исходников

- `check55.sh` — PASS.
- `user_shell.c` — `gcc -m32 -ffreestanding -fsyntax-only` — PASS.
- `comdrv.c` — `gcc -m32 -ffreestanding -fsyntax-only` — PASS.
- Проверена генерация i386 assembly: EBX/ECX/EDX теперь используются как ABI-входы без неопределённого обмена регистрами.
- Существующие syscall и функции не удалялись.
