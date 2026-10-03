# v19 — исправление SYS_CONSOLE_READ и проверка SYS_CONSOLE_WRITE

## Причина

Путь `SYS_CONSOLE_READ` зависит от аппаратного IRQ1 клавиатуры. Предыдущая версия
ожидала данные через `hlt`, но не выполняла явную инициализацию интерфейса 8042 и
безусловно читала порт `0x60` при каждом IRQ1.

## Исправление

Добавлена `keyboard_init()`:

1. команда `0xAE` на порт `0x64` явно включает интерфейс клавиатуры;
2. остаточные байты BIOS из output buffer 8042 удаляются;
3. IRQ1 читает данные только если `KBD_STATUS & 1` (Output Buffer Full).

`SYS_CONSOLE_READ` по-прежнему блокируется в kernel mode через `sti; hlt; cli` и
получает символ из FIFO после IRQ1.

## SYS_CONSOLE_WRITE

Проверяется отдельно через `sys_console_write("console-write OK\\n",17)`.
Пользовательский буфер проходит ту же проверку диапазона user memory, после чего
ядро выводит ровно `n` байт.

## Ограничения

ABI, GDT, сегменты 1 GiB, paging, FAT16, EXE1, SYS_EXEC/SYS_EXIT, linker script
`liker.ld`, W64DevKit/x86 и исходный layout диска не изменены.
