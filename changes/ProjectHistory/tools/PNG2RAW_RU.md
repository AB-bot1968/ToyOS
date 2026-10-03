# PNG2RAW — конвертер заставки Toy OS

## Назначение

`PNG2RAW` — обычная C-утилита, которая собирается GCC из W64DevKit x86.
Она преобразует графический файл PNG в `SPLASH.RAW`, который использует
`VGADRV.EXE` в Toy OS.

Конвертер полностью самостоятельный: он не использует `System.Drawing`,
PowerShell, .NET, GDI+, `zlib.h`, `-lz`, libpng или другие внешние библиотеки.
Весь необходимый декодер zlib/DEFLATE, CRC32 и Adler-32 находится в
`tools/png2raw.c`.

## Формат результата

Результат всегда имеет фиксированные параметры:

- 320×200 пикселей;
- 8 бит на пиксель;
- 256 индексов палитры VGADRV;
- ровно 64000 байт;
- заголовка RAW нет.

PNG автоматически масштабируется до 320×200 методом ближайшего соседа.
Прозрачность смешивается с чёрным фоном.

## Использование

После первой полной сборки проекта:

```text
build.bat
```

утилита находится здесь:

```text
tools\PNG2RAW.EXE
```

Запуск:

```text
tools\PNG2RAW.BAT my_splash.png resources\SPLASH.RAW
```

Можно также напрямую:

```text
tools\PNG2RAW.EXE my_splash.png resources\SPLASH.RAW
```

После замены `resources\SPLASH.RAW` нужно снова выполнить:

```text
build.bat
```

`AUTOSTART.SH` уже запускает:

```text
exec VGADRV.EXE SPLASH.RAW
```

поэтому исходный код ОС менять не требуется.

## Поддерживаемые PNG

Поддерживаются 8-битные неинтерлейсные PNG с типами цвета:

- 0 — grayscale;
- 2 — RGB;
- 3 — indexed palette;
- 4 — grayscale + alpha;
- 6 — RGBA.

Используются все стандартные PNG-фильтры строк: None, Sub, Up, Average,
Paeth.

Внутренний декодер DEFLATE поддерживает stored, fixed Huffman и dynamic
Huffman блоки, как это требуется для обычных PNG IDAT-потоков.

## Сборка

`build.sh` автоматически компилирует:

```text
tools/png2raw.c -> build/png2raw.exe -> tools/PNG2RAW.EXE
```

с hosted-флагами проекта:

```text
-O2 -std=c99 -Wall -Wextra -Werror
-static -static-libgcc
```

Дополнительной библиотеки при линковке нет.
