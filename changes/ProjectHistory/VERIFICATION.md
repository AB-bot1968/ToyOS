# Тройная проверка — Shell Ring 3

## Pass 1/3 — исходники

1. `kernel.c`, `user_shell.c` проходят строгую i386 freestanding C-компиляцию с `-Wall -Wextra -Werror`.
2. `boot.S` и `isr.S` собираются GNU `as --32`.
3. FAT16 host tools проходят C89 warnings-as-errors.
4. В kernel присутствуют GDT user descriptors, TSS, `LTR`, DPL3 gate и pointer validation.
5. `user_shell.c` содержит собственные syscall wrappers и не импортирует kernel `sys_*` functions.
6. User code/rodata помещаются в отдельные page-aligned sections.

## Pass 2/3 — PE/COFF, paging и бинарник

Проверены: boot=512 байт, `55 AA`, kernel начинается с `EA`, selector=`0x08`, unresolved symbols отсутствуют, kernel=17473 байт и помещается в 64×512=32768 байт.

Отдельно проверены машинные инструкции `LTR`, загрузка `CR3`, включение `CR0.PG`, `IRET` и формирование user stack frame с начальным ESP `0x003FFFE0`.

## Pass 3/3 — disk/FAT16

Проверены: image=8192×512 байт, boot LBA0, kernel LBA1..64, scratch LBA65, FAT16 LBA66, `README.TXT`, обе FAT-копии.

## Важное исправление из предыдущей версии

Предыдущий вариант использовал только сегментацию и программную проверку диапазонов. Это не было полноценной memory isolation, поскольку flat user data segment покрывал тот же 1-GiB адресный диапазон. Теперь включено 4-KiB paging: kernel pages остаются supervisor-only, а user-доступ получают только `.utext`, `.urodata` и выделенная user stack page.

## Runtime

QEMU i386 в среде проверки отсутствует, поэтому runtime BIOS/ATA/keyboard/CPL3 тест не выдаётся за выполненный. На Windows 7 с x86 W64DevKit запустите `build.bat`: он выполняет `check10.sh` и `check3.sh`.


## Исправление EXC 13 / отсутствие ввода в Ring 3

В предыдущей сборке компилятор мог автоматически генерировать инструкции SSE/MMX (`MOVAPS`, `MOVDQA`, `XORPS` и другие) даже при freestanding-сборке. Для маленького учебного ядра это нежелательно: состояние FPU/SSE ещё не является частью механизма переключения контекста. Поэтому Ring-3 проект теперь явно запрещает автоматическую SIMD/FPU генерацию параметрами `-fno-tree-vectorize -fno-tree-slp-vectorize -mno-sse -mno-sse2 -mno-mmx -mno-80387`. Проверка `check3.sh` дополнительно запрещает появление таких инструкций в итоговом PE/COFF. Это устраняет скрытую зависимость пользовательского Shell от FPU/SSE и является обязательной частью проверки.


## Исправление v7: U/S в PDE

Критическая причина предыдущего сбоя Ring-3: PDE[0] был создан без бита U/S. При этом отдельные PTE уже имели U/S=1. На x86 пользовательский доступ разрешается только если U/S=1 установлен и в PDE, и в PTE. Поэтому page_directory[0] теперь создаётся с `PAGE_US`, а PTE kernel остаются supervisor-only. Это позволяет CPL3 выполнять только отмеченные пользовательские страницы, не открывая kernel pages.


## V7: исправление U/S в PDE

Критическая ошибка предыдущей версии: `page_directory[0]` содержал `P|RW`, но не содержал `U/S`. Отдельные PTE пользовательских областей уже имели `U/S=1`, однако x86 проверяет U/S на каждом уровне таблиц страниц. Поэтому CPL3 не мог получить доступ ни к `.utext`, ни к `.urodata`, ни к user stack. В v7 PDE[0] создаётся с `P|RW|US`; PTE kernel страниц по-прежнему имеют `US=0`, а только пользовательские страницы получают `US=1`.

Для следующего page fault диагностический обработчик дополнительно выводит `CR2`.


## v8: исправление ложного EXC 8 и усиление проверки PIC

Новая ошибка `EXC 8 EIP=8 CS=582 ERR=38467` была разобрана по машинным значениям. `582 = 0x246` — типичное значение EFLAGS, а `38467 = 0x9643` — адрес внутри kernel image. Это означает, что vector 8 был вызван как аппаратный IRQ0 без CPU error-code, но обработчик `isr8` интерпретировал стек как настоящий Double Fault. Причина — конфликт старого BIOS-маппинга IRQ0 -> INT 8 с CPU vector 8 (#DF).

В v8 PIC 8259A полностью маскируется до ICW, затем получает явные вектора `0x20` и `0x28`, cascade `0x04/0x02`, режим `0x01/0x01`, и только после этого устанавливается известная маска `0xFC/0xFF`. Remap выполняется до `paging_init()`, а IRQ0/IRQ1 разрешаются отдельно после PIT. `check3.sh` теперь проверяет весь PIC remap sequence.

Это изменение не ослабляет требования Ring-3: GDT/TSS, точные 1-GiB сегменты, CPL3 IRET, paging U/S, пользовательский stack, INT 80h ABI, FAT16 и Windows 7/W64DevKit build model сохранены.
