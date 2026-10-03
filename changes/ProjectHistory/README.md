# Toy OS v65.15 — стабильная Git-готовая база

Текущая стабильная версия проекта: **v65.15**. В неё входят проверенные функции FAT16/paths/cwd, Ring-3 shell, NETDRV/VGA driver, AUTOSTART и самостоятельная PNG2RAW без внешнего zlib.

Репозиторий предназначен для сборки под **Windows 7 x64 + W64DevKit x86** и для запуска 32-битного Toy OS в QEMU. Инструкция публикации в Git находится в `GIT_SETUP_RU.md`.

Toy OS v65.10: 320x200 VGA splash, восстановление font plane 2 и полноценный возврат в text mode 03h.


## v65.12 — PNG -> SPLASH.RAW utility

Добавлена Windows 7-совместимая утилита `tools/PNG2RAW.BAT`. Основная версия `PNG2RAW.EXE` компилируется исходником `tools/png2raw.c` непосредственно GCC из W64DevKit x86. Внешние библиотеки PNG/zlib не требуются: DEFLATE, CRC32 и Adler-32 встроены в C-исходник. Подробная инструкция находится в `tools/PNG2RAW_RU.md`.

# Toy OS v64 — FAT16 directories, cwd/path, cp and paginated help

> v64 is based strictly on the verified v63.1 baseline. New in this release: `cp SOURCE DEST`, syscall 31, four-page `help`, and full regression/edge-case checks.

# Toy OS v62 — FAT16 directories, cwd/cd/pwd and standalone path module

## v32 — SYS_FILE_SIZE: размер файла в байтах

Добавлен системный вызов №17 `SYS_FILE_SIZE`. Он принимает в `EBX` указатель на NUL-terminated имя FAT16-файла 8.3 и возвращает в `EAX` точный размер файла в байтах. `0xffffffff` означает отсутствие файла или ошибку. Вызов не требует предварительного `SYS_FILE_OPEN` и не оставляет открытый fd.

Ring-3 shell предоставляет команды `filesize FILE` и `syscall 17 FILE`. Для проверки из EXE1 добавлен `QSIZE.EXE`: он получает размер `README.TXT` и проверяет отказ для `NOFILE.TXT`.

```text
filesize README.TXT
syscall 17 README.TXT
exec QSIZE.EXE
```

# Toy OS — Windows 7 + W64DevKit — Ring-3 Shell — v29

This is the **ring-3 shell extension** of the original 32-bit Toy OS. It preserves the original BIOS loader, protected-mode kernel, 1-GiB flat kernel segments, FAT16 layer, syscall ABI, `liker.ld`, raw binary extraction and Windows 7/W64DevKit build model, then adds a real CPL3 transition for the shell.

The kernel slot is increased to 128 sectors because the existing FAT16 write API, syscall infrastructure and ring-3 shell no longer fit in the original 10-sector limit. The bootloader and image layout are changed consistently: it reads the 128-sector kernel slot and FAT16 starts at LBA 130.

## Architecture

```text
BIOS
  -> boot.S (real mode)
  -> E820/E801 memory detection
  -> INT 13h Extensions: 128 sectors -> 0x7E00
  -> protected mode
  -> kernel_main (CPL0)
  -> install kernel/user GDT + TSS
  -> IDT/PIC/PIT/KBD/ATA/FAT16
  -> automatic syscall/file tests
  -> iret to _shell_run at CPL3
       |
       +-- INT 80h --> kernel syscall dispatcher (CPL0)
                         |
                         +-- iret back to CPL3 shell
```

The shell itself is compiled from `src/user_shell.c`, but its entry point runs with `CS=0x1B` and `DS/ES/FS/GS/SS=0x23`. The kernel uses `CS=0x08`, `DS/ES/FS/GS/SS=0x10`.

## Ring-3 protection details

The kernel installs six GDT entries:

| Selector | Entry | DPL | Purpose |
|---:|---|---:|---|
| 0x00 | null | - | null descriptor |
| 0x08 | kernel code | 0 | base 0, exactly 1 GiB |
| 0x10 | kernel data | 0 | base 0, exactly 1 GiB |
| 0x18 | user code descriptor | 3 | selector used from CPL3: `0x1B` (`0x18 | RPL3`); base 0, exactly 1 GiB |
| 0x20 | user data descriptor | 3 | selector used from CPL3: `0x23` (`0x20 | RPL3`); base 0, exactly 1 GiB |
| 0x28 | TSS | 0 | ring-transition stack |

The TSS sets `SS0=0x10` and `ESP0=0x001F0000`. Therefore an interrupt/syscall from CPL3 can switch to a known kernel stack before the C dispatcher executes.

The syscall gate at vector `0x80` is DPL3, so user code may execute `INT 80h`. Hardware IRQ gates remain DPL0.

## User memory validation

The ring-3 dispatcher does not blindly trust user pointers:

- read-only user arguments are checked to stay below 1 GiB;
- writable buffers supplied by ring 3 are restricted to the demo user stack window `0x3FF000..0x3FFFFF`;
- file names must contain a NUL within a bounded 128-byte scan;
- disk sector counts are bounded before multiplication by 512.

This is a deliberately small teaching kernel, not a complete memory-protection subsystem. There is no process isolation, executable loader, heap, scheduler or copy-on-write mechanism. Ring-3 user code is protected by a small identity-mapped 4-MiB paging setup: only `.utext`, `.urodata` and the dedicated user stack page are marked user-accessible.

## Shell commands

The shell uses the syscall ABI rather than calling kernel internals directly:

```text
help
echo hello
ticks
`pit-test`
cat README.TXT
write TEST.TXT Hello from ring3
append TEST.TXT +append
cat TEST.TXT
syscall 1 Hello through syscall 1
syscall 3
syscall 2
```

`syscall 2` is a blocking keyboard-input syscall. It waits for IRQ1 with `sti; hlt; cli`, returns one character per call, and the shell line editor echoes each character with `SYS_CONSOLE_WRITE`, so the real CPL3 -> INT80 -> CPL0 -> iret path is exercised continuously.

## Build on Windows 7

Install the **x86 W64DevKit** and run `build.bat` from its shell. `gcc -dumpmachine` must identify an i686/i386 target.

`build.sh` builds the bootloader, kernel, separate shell object, interrupt stubs and FAT16 host utilities. It then runs the complete regression suite, including `check13.sh`, `check63.sh`, `check64.sh` and the v61 cwd/path regression `check65.sh`.

Штатный запуск сборки в W64DevKit — `build.bat`; из shell можно выполнить `./build.sh`. Файл `build` намеренно не используется, поскольку `build/` является рабочим каталогом сборки и очищается в начале `build.sh`.

## Run

Use QEMU/another i386 emulator with `build/toy_os.img`. Do not point the build at a physical hard disk.

Expected final lines include:

```text
Ready. Entering ring-3 shell.
Toy OS ring-3 shell. Commands use INT 80h.
...
toy0>
```

If an exception or invalid user memory access occurs, the kernel prints the exception number and halts rather than attempting recovery.

## Disk layout

| LBA | Size | Purpose |
|---:|---:|---|
| 0 | 1 sector | BIOS boot sector |
| 1..64 | 128 sectors | kernel + ring-3 shell image |
| 41 | 1 sector | gap |
| 42 | 1 sector | FAT16 boot sector |
| 43..74 | 32 sectors | FAT #1 |
| 75..106 | 32 sectors | FAT #2 |
| 107..138 | 32 sectors | root directory |
| 139+ | remainder | FAT16 data area |

## Preserved original requirements

- GNU `as` bootloader and real-mode entry.
- E820 memory sizing with E801 fallback.
- RAM size is saved by the bootloader at `0xB8FF0`, outside the visible 80x25 VGA text buffer, and decoded by the kernel.
- A20 via port `0x92`.
- INT 13h Extensions for kernel loading.
- Flat base-0 code/data segments with exactly 1-GiB coverage.
- First kernel operation remains the top-level `.start` assembly jump:
  `jmpl $0x08, $_kernel_main`.
- Separate linker script named `liker.ld`.
- `objcopy` raw kernel extraction.
- FAT16 root-file read/write API and host verification.
- Windows 7 compatibility through the x86 W64DevKit toolchain.

## Syscall ABI

`EAX` = number, `EBX` = arg1, `ECX` = arg2, `EDX` = arg3, result in `EAX`.

1. `console_write`
2. `console_read`
3. `timer_get`
4. `disk_read`
5. `disk_write`
6. `file_open`
7. `file_read`
8. `file_write`
9. `file_close`

See `SYSCALLS.md` for the full ABI and the ring-3 pointer-validation rules.

## Files

- `src/boot.S` — BIOS loader.
- `src/isr.S` — interrupt/exception/syscall stubs and BSS clear.
- `src/kernel.c` — CPL0 kernel, drivers, FAT16, GDT/TSS and dispatcher.
- `src/user_shell.c` — shell code executed at CPL3.
- `liker.ld` — kernel linker script.
- `boot.ld` — boot-sector linker script.
- `tools/mkfat16.c` — test image generator.
- `tools/fat16check.c` — FAT16 verifier.
- `SYSCALLS.md` — syscall reference.
- `DESIGN.md` — architecture and audit notes.
- `check10.sh` — ten concrete checks.
- `check3.sh` — three independent verification passes.

## Verification policy

The package is designed for three independent passes:

1. **Source audit** — confirms the ring-3 transition, GDT/TSS, DPL3 gate, pointer validation and shell.
2. **Binary audit** — confirms boot/kernel sizes, signatures, far entry opcode and unresolved-symbol status.
3. **Disk/filesystem audit** — confirms image geometry, boot sector, FAT16 location and file-chain verification.

A runtime boot result must only be claimed after actually running the image in QEMU or another i386 emulator.

## Important Ring-3 isolation correction

The earlier draft of the Ring-3 project described segmentation-only pointer checks as isolation. That description was too strong: without paging, a flat CPL3 segment could still address kernel pages. The current revision fixes this by enabling 4-KiB paging before entering the shell. Kernel pages are supervisor-only; only page-aligned `.utext`, `.urodata`, and the dedicated user stack page `0x003FF000..0x003FFFFF (верхушка стека 0x003FFFE0)` are user-accessible. The user shell also contains its own `INT 80h` wrappers and no longer calls kernel C wrapper functions directly.

## Русская построчная документация

- `docs/SOURCE_ANNOTATED_RU.md` — все строки исходников с отдельным русским комментарием.
- `docs/COMPILER_FLAGS_RU.md` — подробное объяснение каждого параметра компилятора, assembler, linker и objcopy.
- `docs/ARCHITECTURE_RU.md` — подробное объяснение GDT, TSS, paging, CPL3 и INT 80h.


## Исправление EXC 13 / отсутствие ввода в Ring 3

В предыдущей сборке компилятор мог автоматически генерировать инструкции SSE/MMX (`MOVAPS`, `MOVDQA`, `XORPS` и другие) даже при freestanding-сборке. Для маленького учебного ядра это нежелательно: состояние FPU/SSE ещё не является частью механизма переключения контекста. Поэтому Ring-3 проект теперь явно запрещает автоматическую SIMD/FPU генерацию параметрами `-fno-tree-vectorize -fno-tree-slp-vectorize -mno-sse -mno-sse2 -mno-mmx -mno-80387`. Проверка `check3.sh` дополнительно запрещает появление таких инструкций в итоговом PE/COFF. Это устраняет скрытую зависимость пользовательского Shell от FPU/SSE и является обязательной частью проверки.

## Диагностика исключений

Если экспериментальная модификация снова вызовет исключение CPU, ядро теперь выводит не только номер исключения, но и `EIP`, `CS` и `ERR`. Например:

```text
EXC 13 EIP=0000B012 CS=0000001B ERR=00000000
```

`CS=0x1B` подтверждает, что исключение возникло при выполнении Ring-3 кода. `EIP` показывает точную инструкцию, а `ERR` — код ошибки процессора.

### Interrupt/ATA safety

The ATA driver is polling-based, so IRQ14 remains masked. The console-read syscall is intentionally non-blocking: when the keyboard queue is empty it returns zero immediately. The Ring-3 shell retries the syscall after returning to user mode, allowing normal timer/keyboard interrupts to run without nested interrupts inside the INT 80h dispatcher. This avoids re-entrant ISR/IRET paths during synchronous FAT/ATA operations.

## Исправление Ring-3: разделение стеков

В предыдущей версии первичный kernel stack и TSS `ESP0` использовали один верхний адрес `0x001F0000`. Это не оставляло отдельной области для автоматического stack switch при входе из CPL3 и усложняло диагностику повреждений interrupt/IRET frame.

Исправленная версия использует:

- boot/kernel stack: `0x00200000`;
- TSS `ESP0`: `0x001F0000`;
- user stack page: `0x003FF000..0x003FFFFF`;
- начальный user `ESP`: `0x003FFFE0`.

Также `.urodata` принимает `.rdata*` и `.rodata*`, чтобы read-only данные user shell не выпадали из raw kernel image при отличиях между PE/COFF и локальным ELF audit.

## v6: исправление CPL3 entry и синхронизация артефактов

В v6 переход в ring-3 выполняется через отдельный `_user_entry`: IRET переносит управление
на него с IF=0, затем `_user_entry` загружает пользовательские DS/ES/FS/GS и только после этого
включает interrupts и вызывает `_shell_run`. Это делает границу CPL0/CPL3 однозначной.
Также `kernel.dis` пересоздан из текущего audit-бинарника, чтобы исходники, дизассемблирование
и готовый raw image не расходились по версиям.


## Исправление v7: U/S в PDE

Критическая причина предыдущего сбоя Ring-3: PDE[0] был создан без бита U/S. При этом отдельные PTE уже имели U/S=1. На x86 пользовательский доступ разрешается только если U/S=1 установлен и в PDE, и в PTE. Поэтому page_directory[0] теперь создаётся с `PAGE_US`, а PTE kernel остаются supervisor-only. Это позволяет CPL3 выполнять только отмеченные пользовательские страницы, не открывая kernel pages.


## V7: исправление U/S в PDE

Критическая ошибка предыдущей версии: `page_directory[0]` содержал `P|RW`, но не содержал `U/S`. Отдельные PTE пользовательских областей уже имели `U/S=1`, однако x86 проверяет U/S на каждом уровне таблиц страниц. Поэтому CPL3 не мог получить доступ ни к `.utext`, ни к `.urodata`, ни к user stack. В v7 PDE[0] создаётся с `P|RW|US`; PTE kernel страниц по-прежнему имеют `US=0`, а только пользовательские страницы получают `US=1`.

Для следующего page fault диагностический обработчик дополнительно выводит `CR2`.


## v8: исправление ложного EXC 8 и усиление проверки PIC

Новая ошибка `EXC 8 EIP=8 CS=582 ERR=38467` была разобрана по машинным значениям. `582 = 0x246` — типичное значение EFLAGS, а `38467 = 0x9643` — адрес внутри kernel image. Это означает, что vector 8 был вызван как аппаратный IRQ0 без CPU error-code, но обработчик `isr8` интерпретировал стек как настоящий Double Fault. Причина — конфликт старого BIOS-маппинга IRQ0 -> INT 8 с CPU vector 8 (#DF).

В v8 PIC 8259A полностью маскируется до ICW, затем получает явные вектора `0x20` и `0x28`, cascade `0x04/0x02`, режим `0x01/0x01`, и только после этого устанавливается известная маска `0xFC/0xFF`. Remap выполняется до `paging_init()`, а IRQ0/IRQ1 разрешаются отдельно после PIT. `check3.sh` теперь проверяет весь PIC remap sequence.

Это изменение не ослабляет требования Ring-3: GDT/TSS, точные 1-GiB сегменты, CPL3 IRET, paging U/S, пользовательский stack, INT 80h ABI, FAT16 и Windows 7/W64DevKit build model сохранены.


## Исправление v9

Переход в Ring 3 больше не использует `STI` из CPL3. Бит `IF` устанавливается в IRET frame из Ring 0; это устраняет `#GP(13)` из-за привилегированной инструкции `STI`. Fallback `_user_entry` также не содержит `CLI/HLT`. Проверки проекта дополнительно контролируют отсутствие привилегированных инструкций в пользовательском участке.

## v10 — исправление клавиатуры и курсора

В v10 исправлены две ошибки пользовательского интерфейса.

**Клавиатура.** Таблица PS/2/IBM PC Set-1 теперь индексируется непосредственно
scancode. `0x01=Esc`, `0x02=1`, `0x0E=Backspace`, `0x0F=Tab`, `0x1C=Enter`.
В v9 перед `Esc` было два нулевых байта, поэтому таблица была сдвинута на один
индекс; именно поэтому Enter превращался в `]`, а остальные клавиши давали
соседние символы.

**Аппаратный курсор.** Программные `vga_x/vga_y` теперь синхронизируются с
VGA CRTC через `0x3D4/0x3D5` (регистры `0x0E/0x0F`). Синхронизация выполняется
после обычного символа, `\n`, `\r`, Backspace и очистки экрана. Поэтому
мигающий аппаратный курсор следует за приглашением `toy0>` и введённым текстом,
а не остаётся на позиции, которую оставил BIOS.

Команда `delete FILE` удаляет файл из корневого каталога FAT16, освобождает его цепочку кластеров и помечает запись каталога свободной.

## v13 — standalone EXE и `exec`

v13 сохраняет предыдущую архитектуру и добавляет минимальный формат исполняемых
файлов `EXE1`. Исполняемая программа теперь может находиться в FAT16 отдельно
от `kernel.bin`.

В проекте есть:

- `src/hello.c` — минимальная Ring-3 программа;
- `program.ld` — linker script программы с адресом загрузки `0x00100000`;
- `tools/mkexe.c` — упаковщик raw image в EXE1;
- `build/HELLO.EXE` — создаваемый при сборке пример;
- `SYS_EXEC = 11` и `SYS_EXIT = 12`;
- команда shell `exec FILE.EXE`;
- отдельная пользовательская страница стека EXE `0x003FE000..0x003FEFFF`;
- подробное описание в `docs/EXE_FORMAT_RU.md`.

Пример:

```text
toy0> exec HELLO.EXE
Hello from HELLO.EXE (Ring 3)
toy0>
```

Полный путь создания и запуска:

```text
hello.c -> hello.o -> hello.pe -> hello.raw -> HELLO.EXE -> FAT16 -> exec -> 0x00100000 -> iret -> CPL3
```


## v14 — исправление EXE stack page

Исправлена ошибка page fault при запуске EXE. Начальный ESP программы равен `0x003FF000`; первая инструкция `call`/`push` записывает данные по адресу `0x003FEFFC`, то есть использует страницу `0x003FE000..0x003FEFFF`. В v13 эта страница имела запись PTE, но не имела `PAGE_US`, поэтому Ring 3 получал `#PF` с `ERR=7` (user write to a supervisor page). Теперь `paging_set_exec_user()` при запуске EXE явно устанавливает для `EXEC_STACK_PAGE` флаги `P|RW|US`, а при завершении снимает `US`. Добавлена отдельная регрессионная проверка этого условия.


### v15 — диагностика `exec`

Добавлены отдельные коды ошибок `SYS_EXEC` и вывод кода в shell. Кроме того, загрузчик
проверяет точное соответствие размера FAT16-файла формуле `16 + image_size`. Это
позволяет диагностировать `exec FAIL` по конкретной причине и не маскировать ошибки
упаковки/чтения EXE одним общим кодом.

## syscall-test

Для полного функционального прогона системных вызовов из Ring-3 shell используйте:

```text
syscall-test
```

Команда реально вызывает основные syscall 1..16 через `INT 80h` в том числе отдельные интеграционные проверки очереди EXE1. Для `SYS_DISK_READ/WRITE` используется временный сектор LBA 129: его содержимое сохраняется, заменяется контрольным шаблоном, читается обратно, сравнивается и восстанавливается. Для файловых syscall создаётся временный `SYSCALL.TST`, который затем удаляется. Для `SYS_EXEC/SYS_EXIT` запускается штатный `HELLO.EXE`, после чего выполнение возвращается в shell.

Команда `syscall-test` требует ввести `OK` и Enter на этапе проверки `SYS_CONSOLE_READ`.

### v19 — исправление console I/O

В v19 исправлен аппаратный путь `SYS_CONSOLE_READ`: добавлена инициализация
8042/keyboard interface и проверка Output Buffer перед чтением scancode. Также
добавлена отдельная статическая проверка `SYS_CONSOLE_WRITE`. См.
`CONSOLE_IO_FIX_V19_RU.md`.

## v20 — исправлен тест SYS_CONSOLE_READ

Тест `syscall-test` больше не ожидает, что один `SYS_CONSOLE_READ` вернёт всю строку. По ABI один успешный вызов возвращает один символ, поэтому тест читает `O`, `K` и Enter тремя отдельными вызовами. Это соответствует рабочему пути `readline()`/`echo TEST`.


### V21: исправление проверки SYS_CONSOLE_READ

`STI/HLT` в `SYS_CONSOLE_READ` выполняются только после входа в Ring 0, поэтому статическая проверка больше не считает этот kernel-path ошибкой. В Ring 3 привилегированные инструкции по-прежнему запрещены. Интерактивный тест читает `O`, `K` и Enter отдельными однобайтными вызовами.

## v22 — arbitrary byte I/O ports and syscall output fix

Добавлены два Ring-3 системных вызова для доступа к произвольным 8-битным I/O
портам x86 через CPL0:

- `SYS_PORT_OUT8 = 13`: порт в EBX, байт в ECX, возврат 0 либо `0xffffffff`;
- `SYS_PORT_IN8 = 14`: порт в EBX, возврат байта 0..255 либо `0xffffffff` при
  недопустимом номере порта.

Пользовательские программы не выполняют `IN`/`OUT` непосредственно: они
вызывают `INT 80h`, а привилегированную инструкцию выполняет ядро. IOPL при
этом остаётся 0.

Команды shell:

```text
outb PORT VALUE
inb PORT
```

Поддерживаются десятичные и шестнадцатеричные значения `0x...`.

Примеры:

```text
outb 0x80 0x55
inb 0x60
```

Произвольный порт может иметь аппаратные побочные эффекты, поэтому
`syscall-test` не выполняет реальную запись/чтение неизвестного устройства;
он проверяет безопасные ветви валидации аргументов syscall 13/14.

Также исправлен вывод команды `syscall`: результат начинается с новой строки
и больше не смешивается со строкой ввода shell. Для `syscall 1 TEXT` выводится
число реально записанных байт, а интерактивный `syscall 2` переводит курсор на
новую строку после принятого символа.

## v23: циклический запуск EXE1

Добавлен последовательный FIFO-планировщик до 25 EXE1. Команды:

`execq REPEAT FILE1 [FILE2 ... FILE25]` — от 1 до 3000 полных проходов;
`execq forever FILE1 [FILE2 ... FILE25]` — полное бесконечное зацикливание;
`execq-stop` — команда остановки очереди.

Для активной бесконечной очереди предусмотрена клавиша `Esc`, поскольку shell
в этот момент синхронно ожидает возврата `SYS_EXEC_QUEUE`. EXE1 может
использовать `SYS_QUEUE_STOP` для немедленной остановки очереди.

Подробное описание ABI и поведения: `docs/EXE_QUEUE_RU.md`.


## v24 — исправление ошибок строгой компиляции

В v24 устранены две группы ошибок, выявленные строгой сборкой `-Wall -Wextra -Werror`: удалена неиспользуемая локальная переменная из `SYS_CONSOLE_READ`, удалён неиспользуемый shell-wrapper `sys_exit`, а EXE-wrapper-функции портового I/O и очереди помечены `used`, чтобы они сохранялись как доступные EXE1 API без ложного `unused-function`. Добавлена `check24.sh`/`check24.bat`, которая на x86 W64DevKit компилирует все C-исходники и оба GNU as-файла с `-Werror` до этапа PE/COFF-link. Основные требования проекта не изменены.

## v25: исправление сборки host-утилит

`tools/mkfat16.c`, `tools/fat16check.c` и `tools/mkexe.c` являются обычными
host-программами и теперь собираются отдельным блоком `HOST_CFLAGS`, без
freestanding-флагов ядра. `build.sh` явно сообщает о каждой компиляции `[TOOLS]`,
проверяет создание каждого `.exe` и выдаёт диагностику при ошибке.
`check25.sh` отдельно проверяет синтаксис и фактическое создание всех трёх
утилит.

## v26 — kernel slot enlarged after real size audit

В v26 исправлена причина остановки сборки после появления `kernel.bin`. После добавления Ring-3 shell, paging, FAT16 write/delete, EXE1, port I/O и очереди EXE1 итоговый raw kernel больше старого 40-секторного слота: локальная 32-битная проверочная линковка даёт **24441 байт = 48 секторов**. Поэтому старый лимит 40 секторов был физически недостаточен.

Теперь загрузчик читает ровно 64 сектора LBA 1..128, свободный scratch-сектор — LBA 129, а FAT16 начинается с LBA 130. Все исходники, host-утилиты, проверки и документация синхронизированы с этой схемой. `build.sh` перед созданием образа вычисляет фактическое число секторов `kernel.bin` и при переполнении выдаёт явную ошибку вместо молчаливого завершения.


### v27 — исправление выполнения EXE1-очереди

Исправлена ошибка `SYS_EXEC_QUEUE`: список имён является входным read-only
буфером, поэтому проверка теперь использует `user_range`, что разрешает
`.urodata` и другие читаемые пользовательские области. Интеграционный тест
очереди использует полный 13-байтный slot `HELLO.EXE`. Добавлена `check27.sh`.

Команды `execq 2 HELLO.EXE` и `execq forever HELLO.EXE` используют тот же ABI;
порядок переходов остаётся `SYS_EXIT -> следующий EXE1`.

## v28: дополнительные EXE1 для проверки очередей

В `src/queue_tests/` добавлены четыре самостоятельных EXE1:
`QPASS.EXE`, `QPORT.EXE`, `QFROMEXE.EXE`, `QSTOP.EXE`.
`build.sh` автоматически компилирует их, упаковывает через `mkexe` и устанавливает все файлы в FAT16-образ. `mkfat16` теперь принимает несколько EXE1 за один запуск и создаёт отдельные root entries и FAT chains.

После загрузки образа рекомендуются:

```text
execq 2 QPASS.EXE
execq 2 QPASS.EXE QPORT.EXE
execq 3 HELLO.EXE QPASS.EXE QPORT.EXE
exec QFROMEXE.EXE
execq forever QSTOP.EXE QPASS.EXE
```

Для максимального количества элементов очередь принимает 25 имён; для максимального конечного числа циклов — 3000. Бесконечную очередь можно остановить клавишей Esc или через `SYS_QUEUE_STOP` из активного EXE. Подробная методика находится в `docs/EXE_QUEUE_TESTS_RU.md`.


## v31 — SYS_FILE_SIZE

Добавлен системный вызов №17 `SYS_FILE_SIZE`: EBX=указатель на имя файла 8.3, результат EAX — точный размер файла в байтах. При ошибке или отсутствии файла возвращается `0xffffffff`.

В Ring-3 shell добавлена команда:

```text
filesize README.TXT
syscall 17 README.TXT
```

Также добавлен `QSIZE.EXE`, который выполняет тот же системный вызов из EXE1 и дополнительно проверяет ошибку отсутствующего файла. Системный вызов не требует предварительного `file_open` и не оставляет открытый файловый дескриптор.

## v30 — исправление standalone SYS_EXEC_QUEUE из EXE1

В v29 существующая команда `execq` сохранена с прежним ABI и поведением, но
теперь EXE1, являющийся элементом активной очереди, может вызвать
`SYS_EXEC_QUEUE` и создать дочернюю последовательную очередь. Состояние
родительской очереди сохраняется в kernel-side stack глубиной до 4 уровней.
После завершения дочерней очереди kernel восстанавливает родительскую очередь
и продолжает её со следующего элемента. Самостоятельный `exec QFROMEXE.EXE`
по-прежнему создаёт верхнеуровневую очередь и после неё возвращается в shell.

Добавлены `QNESTED.EXE` для проверки продолжения родительской очереди после
дочерней и `QFILE.EXE` для проверки `CREATE/TRUNC` записи с последующей
отдельной `APPEND` дозаписью в FAT16-файл. Оба EXE1 автоматически собираются,
проверяются `fat16check` и устанавливаются на образ.

Рекомендуемые тесты:

```text
execq 1 QPASS.EXE QNESTED.EXE QPASS.EXE
exec QNESTED.EXE
exec QFILE.EXE
cat EXELOG.TXT
```

Для первой команды ожидается порядок `QPASS -> QNESTED -> QPASS`, при этом
внутри `QNESTED` сначала выполняются `QPASS -> QPORT`, затем управление
возвращается в родительскую очередь и запускается её последний `QPASS`.
Для `QFILE.EXE` файл `EXELOG.TXT` должен содержать строки `QFILE: first write`
и `QFILE: append write`.

Для увеличения kernel slot после добавления nested queue размер зарезервирован
как 128 секторов (64 KiB): загрузчик читает LBA 1..128, scratch-сектор — LBA 129,
FAT16 начинается с LBA 130. `build.sh` вычисляет фактический размер raw kernel
и завершает сборку с ошибкой, если он превышает 64 сектора.


### v30 — исправление вложенной очереди

Исправлен дефект v29: `SYS_EXEC_QUEUE` теперь корректно различает верхнеуровневый
вызов из standalone EXE1 и вложенный вызов из EXE1, являющегося членом активной
`execq`. В первом случае сохраняется shell frame, во втором — состояние
родительской очереди. Поэтому теперь оба теста допустимы:

```text
execq 1 QPASS.EXE QNESTED.EXE QPASS.EXE
exec QNESTED.EXE
```

## v31 — исправление EXC 14 после `exec QNESTED.EXE`

Исправлена критическая ошибка v30 в переходе standalone EXE1 → `SYS_EXEC_QUEUE`.
`QNESTED.EXE` уже имел сохранённый shell frame после `SYS_EXEC`; v30 ошибочно заменял
его своим текущим Ring-3 frame. После завершения дочерней очереди этот frame возвращал
управление в уже отключённые страницы QNESTED, что вызывало `EXC 14` с `CR2 == EIP`.

В v31 shell frame сохраняется в `queue_prepare()` только для вызова из shell, когда
нет активного EXE. Для standalone EXE и nested EXE сохраняется существующий shell frame;
для nested queue отдельно сохраняется состояние родительской очереди.

Регрессионные тесты:

```text
exec QNESTED.EXE
execq 1 QPASS.EXE QNESTED.EXE QPASS.EXE
exec QFILE.EXE
cat EXELOG.TXT
```

Подробный отчёт: `VERIFICATION_V31_RU.md`.

## v33 — PIT 50 Hz

The preemptive scheduler is not enabled yet. This version only changes the PIT to 50 Hz (20 ms nominal period), keeps IRQ0 counting in `timer_ticks`, and adds the `pit-test` shell test. The existing `execq` and syscall 15/16 queue implementation is unchanged.

## v34: IRQ0 context experiment

v34 established IRQ0 preemptive 19-word context switching for test tasks A/B. v35 extends only this experiment with separate CR3/page tables: A and B keep the same virtual user layout and the same virtual stack address, but that address maps to different physical stack pages. The shell CR3 is saved/restored explicitly. `SYS_MT_START=18` and `SYS_MT_STOP=19` remain dedicated to the experiment. Existing `execq` remains unchanged.


## v35 — separate address spaces / CR3

v35 is the next isolated multitasking experiment. It does not change the EXE1 loader or `execq`. The two test tasks A/B now have separate page-directory roots and page tables. Their user code/rodata are mapped to the same virtual addresses, while virtual `0x003fd000` is backed by physical `0x003fd000` for A and `0x003fc000` for B. IRQ0 saves the fixed 19-word architectural context, flips A/B, loads the next CR3, and restores the next context. `SYS_MT_START` saves the shell frame and shell CR3; `SYS_MT_STOP` restores both. No dynamic allocator, copy-on-write, or per-process executable loader is introduced in v35.

## v36 — фактическая проверка изоляции памяти A/B

v36 сохраняет архитектуру v35 и добавляет не только проверку наличия разных CR3, но и поведенческий тест изоляции. Перед запуском A/B физические страницы их общих виртуальных стеков очищаются. Обе задачи используют один виртуальный адрес `0x003FD000`, но A получает физическую страницу `0x003FD000`, а B — `0x003FC000`.

A записывает маркер `0xA5A55A5A` в локальную переменную на своём стеке. B использует ту же виртуальную позицию в своём стеке: если маркер A виден, тест сообщает `ISOLATION FAIL`; иначе B записывает собственный маркер. После серии IRQ0-переключений A проверяет, что его маркер не изменился. Таким образом, тест одновременно проверяет отсутствие видимости записи A в B и сохранность состояния A после переключений CR3.

## v37 — нормальный scheduler с состояниями задач

v37 заменяет специальную логику «переключить A/B» на маленький общий планировщик.
Каждая задача хранит 19-словный архитектурный контекст, собственный CR3,
состояние READY/RUNNING/STOPPED, идентификатор и счётчик переключений.
IRQ0 сохраняет текущий контекст, переводит его в READY, выбирает следующую
не остановленную задачу round-robin, загружает её CR3 и восстанавливает её
контекст. При остановке теста восстанавливаются CR3 и контекст оболочки.
Пользовательский `mt-test` сохраняет прежний тест реальной изоляции памяти:
A и B используют один виртуальный стек, но разные физические страницы и CR3.



## v39 EXECMT

Добавлен `execmt FILE1.EXE ... FILE25.EXE`: до 25 реальных EXE1 запускаются как независимые scheduler-задачи. Каждая получает отдельный CR3, физический image slot 1 MiB и физический stack page при одинаковых виртуальных адресах. `SYS_EXECMT=23`. Старые `exec` и `execq` не удалены. `build.sh` сохраняет строгий контроль `kernel.bin` по прежнему лимиту 64 сектора.

## v38 — scheduler с жизненным циклом задач
v38 расширяет v37 до четырёх независимых Ring-3 задач. TCB хранит 19-словный context, CR3, id, state и счётчик переключений. Состояния: CREATE, READY, RUNNING, BLOCKED, STOPPED, EXIT. IRQ0 делает round-robin между READY. SYS_SCHED_BLOCK=20 переводит текущую задачу в BLOCKED, SYS_SCHED_WAKE=21 переводит указанную BLOCKED-задачу в READY; если IRQ0 уже перевёл её в READY, повторный wake считается успешным, SYS_SCHED_EXIT=22 завершает текущую задачу в EXIT. SYS_MT_START/STOP сохранены.

Тест A/B/C/D проходит через блокировку и пробуждение: A блокируется, B будит A и блокируется, C будит B и завершается, D завершается, затем A и B возобновляются и завершаются. Тест v36 изоляции памяти сохраняется: A и B используют общий виртуальный адрес стека, но разные физические backing и CR3; marker A невидим B и сохраняется после переключений.


### EXECMT diagnostic output

`execmt` prints one scheduler diagnostic line for each requested task when it first becomes RUNNING. The line contains the MT number, EXE1 filename, CR3 and private stack physical address. Legacy EXE1 programs that terminate with `SYS_EXIT` are also valid EXECMT tasks: while EXECMT is active, `SYS_EXIT` is routed to the scheduler and transitions the current task to EXIT.


## v43 — восстановление `help`, `ls` и SYS_LS

v43 фиксирует две команды, которые ранее повторно выпадали из `help`: `execmt` и `ls`. Команда `ls` выводит содержимое FAT16 root directory одной строкой: имена регулярных файлов 8.3 разделены пробелами и печатаются синим атрибутом VGA `0x01`. Каталоги, volume label, LFN и удалённые записи не выводятся.

Добавлен `SYS_LS=24`. Он доступен не только shell, но и обычным Ring-3 EXE1: пример `QPASS.EXE` вызывает `SYS_LS` и сообщает результат.

Строка `help` содержит обе команды:
`ls`
`execmt FILE1.EXE [FILE2.EXE ... FILE25.EXE]`

## v43 baseline

The v43 project baseline explicitly restores `ls` and `execmt` in shell `help`. `ls` is implemented through `SYS_LS=24`, prints regular root files on one space-separated line in VGA blue, and is callable from Ring-3 EXE1. `QPASS.EXE` exercises `SYS_LS` as a regression test.

## v61 — рабочий каталог shell

v61 реализует следующий запланированный этап после FAT16 каталогов: `cd`, `pwd` и
рабочий каталог (cwd). cwd хранится в ядре как канонический абсолютный путь и не
требует отдельного формата на диске. `SYS_CHDIR=29` проверяет каталог, а
`SYS_GETCWD=30` безопасно копирует cwd в user buffer.

Нормализатор путей обрабатывает относительные пути, `.`, `..` и повторные `/`.
Поэтому после `cd /BIN` команды `ls`, `cat`, `write`, `append`, `delete`, `filesize`,
`exec`, `execmt`, `execq`, `mkdir` и `rmdir` работают относительно `/BIN`, а
`/ABSOLUTE/PATH` остаётся абсолютным. `cd ..` в ROOT не уходит выше `/`.

Команда `cwd-test` автоматически проверяет `/BIN`, нормализацию
`./../BIN/./`, переход `..`, `pwd` и относительный `ls`.

## Toy OS v62 — единый модуль путей

v62 выносит синтаксис и канонизацию путей из `kernel.c` в `src/path.c`/`src/path.h`.
Модуль не знает о FAT16: он обрабатывает только абсолютные/относительные пути,
`.`, `..`, повторные `/` и имена FAT 8.3. Файловая подсистема затем использует
канонический путь для поиска объекта. Это уменьшает дублирование и делает
следующее развитие shell-команд безопаснее.

# Toy OS v65.5 — надёжная передача аргумента VGADRV.EXE

В v65.5 имя изображения для `VGADRV.EXE` получается через новый `34 SYS_EXEC_ARG`. Ядро возвращает сохранённый аргумент запуска в user-буфер, поэтому VGADRV больше не зависит от входных регистров `EBX/ECX/EDX`. Команда `exec VGADRV.EXE SPLASH.RAW` сохраняется без изменений.


Исправлена передача имени графического файла в `VGADRV.EXE`.

Команда:

```text
exec VGADRV.EXE SPLASH.RAW
```

теперь передаёт аргументы в VGADRV через явный cdecl-мост стартовой точки.
`EBX`, `ECX`, `EDX` укладываются на стек в обратном порядке и затем вызывается
`program_main(const char *port, const char *dir, const char *file)`. Это
устраняет использование неопределённого пустого inline-asm для чтения `EDX`.

Проверка запуска:

```text
exec VGADRV.EXE SPLASH.RAW
```

Ожидается загрузка 320x200x256 изображения из обычного FAT16-файла и после
нажатия клавиши восстановление текстового режима с возвращением в shell.

### v65.6: возврат VGA в текстовый режим

`VGADRV.EXE` показывает `SPLASH.RAW` в VGA Mode 13h и ждёт любую клавишу. После нажатия драйвер вызывает `SYS_VIDEO_TEXT = 35`; kernel полностью восстанавливает стандартный цветной режим 80x25, возвращает стандартную DAC-палитру, очищает B8000 и обновляет курсор. Затем драйвер снимает временный доступ к VRAM и завершается.


## v65.10

Исправлен возврат из VGA Mode 13h в штатный VGA 80x25 text mode: корректно восстанавливаются font plane 2, SEQ04 и GC04/GC05/GC06.


## v65.11 — AUTOSTART.SH и нижний prompt

После `SYS_VIDEO_TEXT` kernel оставляет последнюю строку экрана под `toy0>`,
поэтому `VGADRV: OK` оказывается строкой выше, а приглашение — в левом нижнем углу.

Ring-3 shell поддерживает необязательный `/AUTOSTART.SH` в root FAT16. Каждая
непустая строка выполняется тем же `execute_line()`, что и интерактивная команда.
Поддерживаются CR/LF, пустые строки и комментарии `#...`; максимальная длина строки
399 символов. Поставляемый образ содержит `resources/AUTOSTART.SH` с командой:

```text
exec VGADRV.EXE SPLASH.RAW
```

Удаление этого ресурса перед сборкой отключает автозапуск и возвращает стандартный
интерактивный shell.

### Важное примечание об AUTOSTART.SH

`AUTOSTART.SH` не является классическим именем FAT 8.3 (у него 9 символов до точки). Это единственное специальное системное имя: `mkfat16` создаёт стандартную LFN-запись и короткий alias `AUTOST~1.SH`. Обычные файлы и каталоги проекта по-прежнему используют классические 8.3-имена.

Обработчик автозапуска использует тот же `execute_line()`, что и интерактивная консоль. Сам `execute_line()` не читает клавиатуру: интерактивный ввод выполняет только `shell_loop()`. Это гарантирует построчное выполнение `AUTOSTART.SH`.


Примечание v65.13: PNG2RAW больше не использует System.Drawing; PNG декодируется напрямую через встроенный DeflateStream, что устраняет ошибку загрузки System.Drawing.Bitmap на Windows 7.

## PNG2RAW (v65.15)

Для замены заставки используется `tools\PNG2RAW.BAT`. Утилита `PNG2RAW.EXE`
собирается исходным кодом `tools\png2raw.c` через GCC из W64DevKit x86 в начале
обычной сборки проекта и устанавливается в `tools\PNG2RAW.EXE`.

Пример:

```text
tools\PNG2RAW.BAT my_splash.png resources\SPLASH.RAW
```

Конвертер не использует System.Drawing/PowerShell и не требует zlib, libpng
или любой другой внешней библиотеки сжатия. Декодер zlib/DEFLATE встроен
непосредственно в `tools\png2raw.c`.

После сборки `check101.sh` автоматически делает пробную конвертацию
`resources\SPLASH_PREVIEW.png` и проверяет, что получено ровно 64000 байт.

## v66 UDP Telemetry

Stable base: v65.15. v66 adds a minimal UDP/IPv4 telemetry mode to the existing NE2000 network driver and fixed-position VGA text output. See `RELEASE_NOTES_V66_UDP_TELEMETRY_RU.md` and `VERIFICATION_V66_RU.md`.

## v66.2 UDP telemetry fixes

Slave processes are independent: if the Master is absent, they keep running and retry ARP. The Master can be stopped with `ESC`. For two physical Windows PCs, connect each `tap0` and the real LAN adapter with a Windows Network Bridge before testing cross-PC ARP/UDP.

## v66.2

UDP telemetry slaves are independent of Master availability and retry ARP instead of terminating. The Master accepts `ESC` to return from telemetry mode. For cross-PC Ethernet, bridge `tap0` with the real LAN adapter on each Windows host.

## v66.11 — UDP telemetry console dashboard

Следующая версия после проверенной v66.10. Передача UDP не меняется по смыслу; переработан фиксированный dashboard Master/Slave без накопления диагностических строк.

## v67 RTD Stage 3 — несколько одновременно работающих RT-задач

Stage 3 построен от зафиксированной `ToyOS v67 RTD Stage 2 FIX2` и добавляет
до четырёх одновременно активных detached RT-задач. Каждая задача использует
собственный CR3, image slot, stack и сохранённый контекст существующего
scheduler. Shell сохраняет отдельный FIX2 context.

Начиная с Stage 3 F10 команда RTD в интерактивном shell сначала **подготавливает**
задачу и не запускает её немедленно. Можно ввести несколько команд RTD подряд,
а затем нажать `F10`; все подготовленные задачи запускаются последовательно
через штатный `RTD.EXE` и работают одновременно. Максимум — четыре подготовленные
задачи.

Пример:

```text
exec RTD.EXE SENSOR.EXE 20 20 3
exec RTD.EXE SENSOR.EXE 40 40 7
exec RTD.EXE SENSOR.EXE 100 100 3
F10
```

Обе программы остаются обычными EXE1. Периодические release/deadline state
ведутся независимо. `ESC` адресуется текущей RT-задаче.

Priority arbitration между несколькими готовыми RT-задачами намеренно не
перенесён в Stage 3: здесь добавляется именно многозадачность и отложенный
старт. Отдельный следующий этап может изменить выбор готовой задачи на
fixed-priority policy.

Подробности текущих изменений: `changes/RTD/Stage3_F10/`.

## v67 RTD Stage 4.2 — fixed-priority arbitration

Stage 4.2 built from the verified `ToyOS v67 RTD Stage 3 F10 FIX1` line and Stage 4.1 comparator. The scheduler now uses the numeric `priority` value when choosing among READY RT jobs: a larger value has higher priority. If priorities are equal, the previous absolute-deadline ordering remains as a deterministic tie-breaker, followed by the task slot id.

`period`, `deadline`, `SYS_RT_WAIT`, F10 preparation/launch, ESC completion, ordinary EXE1 format, shell context handling, VGA/text transition and PIT 50 Hz are not otherwise changed in this step.

The focused Stage 4.2 regression is `check_rtd_stage4_2.sh`; its documentation is stored in `changes/RTD/Stage4/Stage4_2/`.

## v67 RTD Stage 5.1 — абсолютный deadline одной job

Stage 5.1 formalizes the RT job timing model without changing the stable Stage 4.4 scheduling policy. Each RT job now records an absolute `rt_job_release_tick`, and its `rt_deadline_tick` is calculated only from that release tick plus the requested deadline interval. The deadline therefore does not move when the task is dispatched later. Timer comparisons are wrap-safe for the existing 50 Hz PIT. A new diagnostic syscall `SYS_RT_DEADLINE_INFO=42` returns the current job release tick, absolute deadline tick, current timer tick, period ticks, and missed-deadline count. Stage 5.1 FIX1 keeps the current job's release/deadline immutable even after the deadline is reached; a miss is recorded separately and the next release is created only when the current job completes via `SYS_RT_WAIT`.

Для runtime-проверки добавлен обычный EXE1 `DEADLINE.EXE`. Он специально не является RT-форматом: RTD загружает его так же, как `SENSOR.EXE`.

Пример:

```text
exec RTD.EXE DEADLINE.EXE 1000 100 3
F10
```

Ожидается строка `DEADLINE: started release=R deadline=D now=N`, затем несколько `DEADLINE: sample ...` с тем же абсолютным `release` и `deadline`, пока `now` увеличивается. При `50 Hz` один системный tick равен 20 ms.

Подробности Stage 5.1: `changes/RTD/Stage5/Stage5_1/`. Исправление неизменности текущего absolute deadline: `changes/RTD/Stage5/Stage5_1_FIX1/`.

## v67 RTD Stage 5.2 — обработка deadline miss

Stage 5.2 uses soft-deadline semantics. When `now >= absolute deadline`, the current RT job is marked missed but its `release` and `deadline` remain unchanged and its Ring-3 context is preserved. A missed job becomes best-effort: any on-time READY RT job is preferred regardless of numeric priority. If no on-time RT job is READY, missed jobs may continue and use the existing `priority -> deadline -> slot` ordering among themselves. The job still finishes through `SYS_RT_WAIT`; only then is the next release created.

Для runtime-диагностики добавлен обычный EXE1 `RTDMISS.EXE`. Для первого теста: `exec RTD.EXE RTDMISS.EXE 1000 40 7`, затем `F10`. Для проверки demotion относительно on-time задачи можно подготовить `RTDMISS.EXE` с `priority=7` и `SENSOR1.EXE` с `priority=3` при готовых сроках и запустить их одним F10.

Подробности Stage 5.2: `changes/RTD/Stage5/Stage5_2/`.

## RTD change documentation

Начиная с RTD Stage 3, пояснения изменений RTD, планы тестирования и patch-файлы хранятся в `changes/RTD/` с разбиением по этапам и FIX. Исполняемые regression-тесты остаются в корне проекта и подключаются из `build.sh`.

## v67 RTD Stage 4.4 — диагностический priority-preemption

Stage 4.4 сохраняет fixed-priority выбор Stage 4.2 и добавляет только наблюдаемость и управляемую диагностическую нагрузку. `SYS_RT_TRACE=41` возвращает id RT-задачи, priority, число переключений и глобальный dispatch sequence. `SENSOR1.EXE` и `SENSOR2.EXE` остаются обычными EXE1. При `period=deadline=1000 ms` и priority 3/7 включается отдельный PRIORITY-PROBE: задача удерживает CPU несколько системных тиков до `SYS_RT_WAIT`, чтобы переход HIGH→LOW можно было наблюдать по dispatch sequence. Остальные параметры сохраняют обычное поведение Stage 4.3.

Тест:

```text
exec RTD.EXE SENSOR1.EXE 1000 1000 3
F10
exec RTD.EXE SENSOR2.EXE 1000 1000 7
F10
```

Наблюдайте `dispatch=`: при совместной готовности сначала выбирается `priority=7`. Для непосредственной проверки preemption сначала запускается SENSOR1, затем во время `PRIORITY-PROBE` подготавливается и через F10 запускается SENSOR2; следующий scheduler dispatch должен перейти к SENSOR2 раньше очередного dispatch SENSOR1.

## v67 RTD Stage 5.3 — завершение job и no-backlog release policy

Stage 5.3 defines what happens after `SYS_RT_WAIT` completes a periodic job. The scheduler keeps the current job's absolute `release` and `deadline` unchanged until completion. The next nominal release is derived from the previous release plus `period`. If completion happens after one or more nominal releases, those stale releases are skipped rather than queued as a backlog; `rt_skipped_releases` counts them. The first release strictly after the completion time is selected. If completion happens exactly at the next release tick, that release is not skipped and the next job may become READY immediately.

`deadline_misses` counts actually created jobs that exceeded their absolute deadline. `skipped_releases` counts periodic activations that were never instantiated as separate jobs because the previous job finished too late. These counters are intentionally separate.

A new diagnostic syscall `SYS_RT_JOB_INFO=44` returns job sequence, current absolute release/deadline, current time, next release, active/missed state, cumulative deadline misses and skipped releases. Existing `SYS_RT_DEADLINE_INFO=42` ABI is unchanged.

Для runtime-проверки добавлен обычный EXE1 `RTDSKIP.EXE`:

```text
exec RTD.EXE RTDSKIP.EXE 20 20 3
F10
```

Программа намеренно работает несколько PIT-tick перед `SYS_RT_WAIT`. Для `period=20 ms` это позволяет увидеть несколько пропущенных release. После `SYS_RT_WAIT` строка `next-job` должна показывать новый `release`, находящийся в будущем относительно момента завершения, а `skipped` — количество пропущенных номинальных активаций.

Подробности: `changes/RTD/Stage5/Stage5_3/`.


## v67 RTD Stage 6.1 — отдельный RT timebase 100 Hz

Stage 6.1 adds a real 10 ms RT timebase without changing the logical 50 Hz system timer exposed by `SYS_TIMER_GET`. The PIT hardware is programmed for 100 Hz; `rt_time_ticks` advances on every IRQ0, while the legacy `timer_ticks` advances on every second interrupt and therefore remains 50 Hz. The existing EXECMT scheduler continues to run on the logical 50 Hz cadence, while the detached RT scheduler is serviced at every 10 ms interrupt. RT `period/deadline/release` conversions now use the 100 Hz RT clock, so `10 ms` is exactly one RT tick, `20 ms` is two RT ticks, `40 ms` is four RT ticks, and `100 ms` is ten RT ticks. `SYS_RT_TIME_GET=45` exposes the RT clock to Ring 3; `SYS_TIMER_GET=3` remains the legacy 50 Hz system clock.

The ordinary EXE1 diagnostic `RTTIME.EXE` compares both counters. Example: `exec RTD.EXE RTTIME.EXE 1000 1000 255`, then `F10`. It reports one RT tick per hardware interval and approximately one legacy system tick per two RT ticks.

Подробности Stage 6.1: `changes/RTD/Stage6/Stage6_1/`.
