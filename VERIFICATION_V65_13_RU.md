# Проверка v65.13

## Конвертер PNG2RAW

Проверено статически:

- `tools/PNG2RAW.BAT` существует и передаёт два аргумента корректно;
- `tools/PNG2RAW.ps1` больше не содержит `System.Drawing`;
- в скрипте используется `DeflateStream` без `Stream.CopyTo`, совместимо со
  старыми .NET, присутствующими на Windows 7;
- поддерживаются PNG color type 0/2/3/4/6, bit depth 8, без interlace;
- результат формата Toy OS: 320x200x256, 64000 байт.

## Тестовые проверки

- PNG `resources/SPLASH_PREVIEW.png` разобран Python-валидатором: 320x200,
  8-bit indexed, non-interlaced;
- структура PNG содержит корректные `IHDR`, `PLTE`, `IDAT`, `IEND`;
- архив v65.13 проходит `unzip -t`.

Полный запуск на Windows 7 в данной среде не выполняется, поэтому не
выдаётся за выполненный.
