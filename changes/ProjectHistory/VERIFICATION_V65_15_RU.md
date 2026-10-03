# Проверка Toy OS v65.15

## PNG2RAW

Строгая hosted-сборка:

```text
gcc -O2 -std=c99 -Wall -Wextra -Werror -static -static-libgcc \
    tools/png2raw.c -o build/png2raw.exe
```

Сборка не требует `zlib.h`, libz, libpng или другой внешней библиотеки сжатия.

## Сравнительное тестирование

Новый встроенный DEFLATE-декодер проверен побайтно с эталоном zlib на PNG:

- grayscale;
- RGB;
- RGBA;
- indexed palette;
- dynamic Huffman;
- fixed Huffman;
- stored blocks;
- PNG-фильтры 0, 1, 2, 3, 4.

Во всех случаях получен одинаковый RAW.

## Самотест сборки

`check101.sh` запускает собранный `build/png2raw.exe` на
`resources/SPLASH_PREVIEW.png` и проверяет результат `64000` байт.
