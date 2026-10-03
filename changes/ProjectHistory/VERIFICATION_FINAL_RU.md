# Финальная проверка Ring-3 v8

## Основное исправление

Ошибка `EXC 13 ... ERR=65532` была вызвана неверным описанием структуры `frame` относительно реального порядка сохранения регистров в `_isr_common`.

`src/isr.S` делает:
1. `push ds`
2. `push es`
3. `push fs`
4. `push gs`
5. `pushal`

После `pushal` вершина стека содержит `EDI, ESI, EBP, original ESP, EBX, EDX, ECX, EAX`, затем `GS, FS, ES, DS`, затем `int_no, error, EIP, CS, EFLAGS`.

В предыдущей версии `struct frame` начиналась с `GS`, хотя фактически там находился `EDI`. Из-за этого `f->eax` ссылался на сохранённый `DS`; запись результата syscall через `f->eax=...` перезаписывала сохранённый сегментный selector. Последующий `pop %ds` мог загрузить число возврата syscall как selector и вызвать `#GP(13)`.

В v6 структура приведена к реальному машинному layout:

`EDI, ESI, EBP, original ESP, EBX, EDX, ECX, EAX, GS, FS, ES, DS, int_no, error, EIP, CS, EFLAGS`.

Это является непосредственным исправлением ABI между `_isr_common` и `interrupt_dispatch`.

## Дополнительное исправление

В `SYS_DISK_READ/WRITE` устранено целочисленное переполнение выражения `c*512` при пользовательском `c`. Поскольку user writable window составляет одну страницу 4096 байт, пользовательским syscall разрешено максимум 8 секторов за один вызов. Проверка выполняется до умножения.

## Адреса

- kernel stack / TSS ESP0: `0x001F0000`
- boot stack: `0x00200000`
- user stack page: `0x003FF000..0x003FFFFF`
- initial user ESP: `0x003FFFE0`
- user code selector: `0x1B`
- user data selector: `0x23`
- TSS selector: `0x28`
- code/data effective segment coverage: exactly 1 GiB

## Статическая проверка

- boot sector: 512 bytes
- kernel raw image: within 64-sector slot
- unresolved symbols: none
- SIMD/MMX/x87: none
- IRET path: present
- INT 80h DPL3: present
- user page permissions: checked
- user pointer validation: checked
- TSS/GDT: checked
- frame layout: checked against ISR push order
- disk syscall multiplication overflow: guarded

Настоящий runtime-тест QEMU в текущем окружении не выполнялся, поскольку `qemu-system-i386` отсутствует. Для Windows 7 окончательную PE/COFF сборку следует выполнять x86 W64DevKit через `build.bat`.


## Исправление v7: U/S в PDE

Критическая причина предыдущего сбоя Ring-3: PDE[0] был создан без бита U/S. При этом отдельные PTE уже имели U/S=1. На x86 пользовательский доступ разрешается только если U/S=1 установлен и в PDE, и в PTE. Поэтому page_directory[0] теперь создаётся с `PAGE_US`, а PTE kernel остаются supervisor-only. Это позволяет CPL3 выполнять только отмеченные пользовательские страницы, не открывая kernel pages.


## V7: исправление U/S в PDE

Критическая ошибка предыдущей версии: `page_directory[0]` содержал `P|RW`, но не содержал `U/S`. Отдельные PTE пользовательских областей уже имели `U/S=1`, однако x86 проверяет U/S на каждом уровне таблиц страниц. Поэтому CPL3 не мог получить доступ ни к `.utext`, ни к `.urodata`, ни к user stack. В v7 PDE[0] создаётся с `P|RW|US`; PTE kernel страниц по-прежнему имеют `US=0`, а только пользовательские страницы получают `US=1`.

Для следующего page fault диагностический обработчик дополнительно выводит `CR2`.


## v8: исправление ложного EXC 8 и усиление проверки PIC

Новая ошибка `EXC 8 EIP=8 CS=582 ERR=38467` была разобрана по машинным значениям. `582 = 0x246` — типичное значение EFLAGS, а `38467 = 0x9643` — адрес внутри kernel image. Это означает, что vector 8 был вызван как аппаратный IRQ0 без CPU error-code, но обработчик `isr8` интерпретировал стек как настоящий Double Fault. Причина — конфликт старого BIOS-маппинга IRQ0 -> INT 8 с CPU vector 8 (#DF).

В v8 PIC 8259A полностью маскируется до ICW, затем получает явные вектора `0x20` и `0x28`, cascade `0x04/0x02`, режим `0x01/0x01`, и только после этого устанавливается известная маска `0xFC/0xFF`. Remap выполняется до `paging_init()`, а IRQ0/IRQ1 разрешаются отдельно после PIT. `check3.sh` теперь проверяет весь PIC remap sequence.

Это изменение не ослабляет требования Ring-3: GDT/TSS, точные 1-GiB сегменты, CPL3 IRET, paging U/S, пользовательский stack, INT 80h ABI, FAT16 и Windows 7/W64DevKit build model сохранены.


## v9: исправление EXC 13 в CPL3

Ошибка `EXC 13 EIP=42739 CS=27 ERR=0` указывает на `#GP` в Ring 3: `CS=27` — это `0x1B`, пользовательский code selector, а error code `0` совместим с общим нарушением защиты, не обязательно связанным с selector. citeturn0search13turn0search12

В исходнике обнаружена непосредственная причина: `_user_entry` выполнял `STI` после перехода в CPL3. При `IOPL=0` это запрещено. Исправление сделано архитектурно правильно: `enter_user_shell()` устанавливает `EFLAGS.IF=1` в IRET frame, а CPL3-код больше не выполняет `STI`. Также удалены `CLI/HLT` из fallback-пути `_user_entry`, потому что они также являются привилегированными инструкциями.

Дополнительная проверка `check3.sh` теперь явно запрещает `STI`, `CLI` и `HLT` внутри `_user_entry` и требует установки `IF` в IRET frame.

Локальный i386 audit после исправления: raw kernel `24441` bytes (current v26 layout), unresolved symbols отсутствуют, `.utext` `0xA000..0xA5EC`, `_user_entry=0xA5D6`; в `.utext` обнаружены только разрешённые `INT 80h`, привилегированных `CLI/STI/HLT/IN/OUT` нет. Базовые `10/10` проверки прошли.


## v11 — исправление прокрутки консоли

Исправлена ошибка консоли после Enter на нижних строках экрана. Ранее `console_put()` при достижении строки 25 сбрасывал `vga_y` в 0, поэтому новый вывод начинался с верхней строки и затирал старый текст. В v11 реализована настоящая прокрутка VGA: строки 1..24 сдвигаются на одну строку вверх, последняя строка очищается, а курсор остаётся в строке 24. Отдельная функция `console_newline()` используется для Enter и обычного перевода строки.

Команда `clear` по-прежнему очищает весь экран напрямую через kernel console; изменение прокрутки не меняет остальные требования проекта.

Команда `delete FILE` удаляет файл из корневого каталога FAT16, освобождает его цепочку кластеров и помечает запись каталога свободной.

## v13 — EXE1 / exec

В v13 добавлены standalone EXE1 и команды/syscalls `exec`/`exit`. Локальная audit-сборка
подтвердила: raw kernel `14344` bytes, raw `HELLO.EXE` image `81` bytes, полный
`HELLO.EXE` `97` bytes, `EXE1` magic и entry `0x00100000`, корректное размещение
файла в FAT16 начиная с cluster 3 и корректную FAT mirror chain. `check10.sh`,
`check3.sh` и `check13.sh` прошли.

Проверен standalone HELLO.EXE: используются `INT 80h`, а `CLI/STI/HLT/IN/OUT`
в его коде отсутствуют. Проверен kernel PE: unresolved symbols отсутствуют,
`IRET` присутствует. QEMU в текущей среде отсутствует, поэтому реальный запуск
BIOS image и проверка перехода `exec -> CPL3 -> SYS_EXIT -> shell` здесь не
эмулировались. Для Windows 7 целевая сборка остаётся `build.bat` из x86 W64DevKit.


## v14 — исправление EXE stack page

Исправлена ошибка page fault при запуске EXE. Начальный ESP программы равен `0x003FF000`; первая инструкция `call`/`push` записывает данные по адресу `0x003FEFFC`, то есть использует страницу `0x003FE000..0x003FEFFF`. В v13 эта страница имела запись PTE, но не имела `PAGE_US`, поэтому Ring 3 получал `#PF` с `ERR=7` (user write to a supervisor page). Теперь `paging_set_exec_user()` при запуске EXE явно устанавливает для `EXEC_STACK_PAGE` флаги `P|RW|US`, а при завершении снимает `US`. Добавлена отдельная регрессионная проверка этого условия.

## v18 — полный функциональный syscall-test

Добавлена команда `syscall-test`, выполняющая все 12 системных вызовов из Ring-3 через `INT 80h`.

Особое внимание уделено двум опасным для файловой системы операциям: `SYS_DISK_WRITE` тестируется только на LBA 65 с сохранением и восстановлением исходного сектора, а файловый тест использует временный `SYSCALL.TST` и удаляет его после проверки.

`SYS_EXEC` запускает `HELLO.EXE`; успешный `SYS_EXIT` должен вернуть выполнение в продолжение shell-команды. QEMU в среде разработки отсутствует, поэтому runtime boot/GUI/keyboard result не заявляется как фактически выполненный тест.

## v23 — EXE1 circular queue

Добавлена последовательная циклическая очередь EXE1: до 25 файлов, от 1 до
3000 полных проходов либо бесконечный режим. Переход к следующему элементу
происходит через существующий `SYS_EXIT`; одновременно загружен только один
EXE1, поэтому архитектура общего user image slot не изменена.

Добавлены `SYS_EXEC_QUEUE=15` и `SYS_QUEUE_STOP=16`, shell-команды `execq`,
`execq forever`, `execq-stop`, а также аварийная остановка `Esc` для активной
бесконечной очереди. `SYS_QUEUE_STOP` из активного EXE немедленно завершает
очередь и восстанавливает shell frame.

`check23.sh` проверяет лимиты 25/3000, кольцевой переход, бесконечный режим,
остановку, EXE wrappers, отсутствие привилегированных инструкций в Ring-3 и
documentation. Локальная 32-битная freestanding-компиляция `kernel.c`,
`user_shell.c` и `hello.c` выполнена успешно.


## v24 — strict compile fix

Добавлена отдельная проверка `check24.sh`: C-исходники `kernel.c`, `user_shell.c`, `hello.c` и GNU as `boot.S`, `isr.S` должны проходить `-Wall -Wextra -Werror`. Исправлены обнаруженные `unused-variable`/`unused-function` ошибки без изменения ABI, syscall numbers или архитектуры Ring 3.
