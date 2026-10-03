# Toy OS v65.15 — PNG2RAW без zlib

v65.15 исправляет сборку утилиты `PNG2RAW.EXE` в установках W64DevKit, где
отсутствует `zlib.h` или библиотека сжатия.

## Изменение

`tools/png2raw.c` больше не подключает внешнюю библиотеку для PNG/DEFLATE.
В исходник встроены:

- zlib header и Adler-32;
- CRC32 PNG chunk;
- DEFLATE stored blocks;
- fixed Huffman blocks;
- dynamic Huffman blocks;
- LZ77 length/distance;
- декодирование PNG-фильтров.

## Сборка

`build.sh` компилирует инструмент только средствами GCC/W64DevKit:

```text
gcc -O2 -std=c99 -Wall -Wextra -Werror -static -static-libgcc \
    tools/png2raw.c -o build/png2raw.exe
```

Дополнительная библиотека при линковке отсутствует. После компиляции программа
копируется в `tools\PNG2RAW.EXE`.

## Формат

Формат `SPLASH.RAW` не изменился:

```text
320x200
8 bit/pixel
64000 bytes
```

Рабочая VGA/FAT16/Ring-3 часть Toy OS не изменялась.
