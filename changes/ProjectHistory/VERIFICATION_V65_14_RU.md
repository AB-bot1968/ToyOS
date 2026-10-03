# Проверка Toy OS v65.14

## PNG2RAW

Проверяется компиляция:

```text
tools/png2raw.c -> build/png2raw.exe
```

с:

```text
-O2 -std=c99 -Wall -Wextra -Werror -static -static-libgcc -lz
```

Затем EXE копируется в:

```text
tools/PNG2RAW.EXE
```

## Формат RAW

Smoke-test выполняет реальную конвертацию `resources/SPLASH_PREVIEW.png` и
проверяет:

```text
64000 bytes
```

Повторная конвертация должна дать бинарно идентичный файл.

## Регрессия

Kernel, VGADRV, AUTOSTART.SH, FAT16 и предыдущие функции проекта не изменены.
Полный Windows 7 + W64DevKit + QEMU runtime здесь не заявляется выполненным.
