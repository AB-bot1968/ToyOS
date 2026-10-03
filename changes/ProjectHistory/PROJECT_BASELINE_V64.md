# Toy OS v64 — baseline

v64 является прямым продолжением проверенного `v63.1`.

## Новое
- `SYS_FILE_COPY=31` (`EBX=SOURCE`, `ECX=DEST`).
- shell `cp SOURCE DEST`.
- DEST-каталог означает `DEST/BASENAME(SOURCE)`.
- копируются только обычные файлы;
- source==destination после нормализации запрещён;
- перезапись существующего файла выполняется через новую FAT-цепочку с переключением directory entry только после успешного копирования;
- `help [PAGE]`, четыре страницы справки.

## Сохранено из v63.1
- FAT16 8.3 без LFN;
- стандартные подкаталоги и `.`/`..`;
- kernel-side cwd и `cd`/`pwd`;
- standalone `src/path.c`;
- относительные пути для файловых операций;
- `rmdir` только пустых каталогов;
- защита от удаления ROOT, cwd и его предков;
- `rm` + legacy alias `delete`;
- полный набор старых regression checks.
