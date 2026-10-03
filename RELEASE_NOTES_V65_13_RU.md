# Toy OS v65.13 — исправление PNG2RAW для Windows 7

Базой является успешно работающая v65.11/v65.12. Изменения ограничены
конвертером PNG2RAW и документацией. Kernel, VGADRV, AUTOSTART.SH и VGA
text-mode не изменялись.

## Исправление

Предыдущий PNG2RAW.PS1 создавал `System.Drawing.Bitmap`. На части установок
Windows 7 / .NET это приводило к ошибке загрузки `System.Drawing.Bitmap`.

В v65.13 System.Drawing полностью удалён из конвертера. PNG разбирается
непосредственно по chunk-ам, IDAT распаковывается через встроенный
`System.IO.Compression.DeflateStream`, PNG-фильтры выполняются самим скриптом.
Дополнительно удалён `Stream.CopyTo`, которого может не быть в старых .NET,
и используется обычный цикл `Read/Write`.

Результат остаётся прежним: 320x200, 8-bit VGA indexed RAW, ровно 64000 байт.
