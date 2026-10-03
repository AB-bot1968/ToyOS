# Toy OS v60 — файловая система FAT16 с каталогами

## Цель

v60 расширяет существующий FAT16-слой до иерархической файловой системы без
изменения формата FAT16 на диске. Реализованы обычные 8.3-каталоги, пути и
операции `mkdir`/`rmdir`.

## Что изменено

- `fat_lookup_path()` разрешает `/FILE`, `/BIN/FILE` и более глубокие пути.
- `fat_resolve_parent()` отделяет родительский каталог от конечного имени.
- обычный каталог хранится в кластерной цепочке FAT16;
- новый каталог получает `.` и `..`;
- `fat_dir_find_free()` расширяет подкаталог новым кластером при необходимости;
- `SYS_LS=24` сохранил номер syscall, но теперь принимает `EBX=PATH`;
  `EBX=0` означает `/` для обратной совместимости;
- `SYS_MKDIR=27` создаёт каталог;
- `SYS_RMDIR=28` удаляет только пустой каталог;
- `SYS_FILE_OPEN`, `SYS_FILE_DELETE`, `SYS_FILE_SIZE` и `SYS_EXEC` теперь
  работают с вложенными путями;
- LFN намеренно не реализован; все компоненты — FAT 8.3;
- `mkfat16` умеет задавать назначение через `HOSTFILE=DESTPATH`;
- `fat16check` умеет проверять вложенные пути и FAT-цепочки каталогов.

## Ручная проверка в QEMU

После сборки загрузить `build/toy_os.img`.

### 1. Root

    ls

Ожидается обычный список файлов, включая `BIN/` и `DOC/`.

### 2. Каталоги

    mkdir /TEST
    ls /
    ls /TEST

Ожидается `TEST/` в root и пустой каталог с двумя служебными записями `.`/`..`.

### 3. Файл внутри каталога

    write /TEST/HELLO.TXT hello-directory
    cat /TEST/HELLO.TXT
    filesize /TEST/HELLO.TXT

Ожидается `hello-directory` и размер `15 bytes`.

### 4. Перезапись и append

    write /TEST/HELLO.TXT first
    append /TEST/HELLO.TXT second
    cat /TEST/HELLO.TXT

Ожидается `firstsecond`.

### 5. Вложенный каталог

    mkdir /TEST/SUB
    write /TEST/SUB/FILE.TXT nested
    cat /TEST/SUB/FILE.TXT
    ls /TEST/SUB

### 6. Удаление

    delete /TEST/SUB/FILE.TXT
    rmdir /TEST/SUB
    rmdir /TEST

Ожидается `delete: OK`, затем два раза `rmdir: OK`.

### 7. Защита непустого каталога

    mkdir /TEST
    write /TEST/FILE.TXT data
    rmdir /TEST

Ожидается `rmdir: FAIL`. После этого удалить файл и каталог:

    delete /TEST/FILE.TXT
    rmdir /TEST

### 8. Выполнение EXE1 по пути

    exec /BIN/HELLO.EXE

Ожидается обычный вывод `HELLO.EXE`, что подтверждает прохождение пути через
тот же `fat_open()`, который используется обычным файловым вводом-выводом.

## Host-side проверки

Построитель образа должен создать:

- `BIN/HELLO.EXE`;
- `BIN/NETDRV.EXE`;
- `DOC/NET.CFG`.

Затем `fat16check` проверяет:

    BIN
    BIN/HELLO.EXE
    BIN/NETDRV.EXE
    DOC
    DOC/NET.CFG

Для kernel/shell/host filesystem sources `check61.sh` выполняет три уровня:

1. `-fsyntax-only`;
2. обычную компиляцию объектного файла;
3. повторную компиляцию с `-fno-omit-frame-pointer`.

Все проверки используют строгие предупреждения для host tools и тот же
freestanding-набор флагов, что и основной W64DevKit build.

## Ограничения v60

- только FAT 8.3;
- нет LFN;
- нет `cd`/`cwd`;
- относительный путь в kernel трактуется относительно ROOT;
- `rmdir` только для пустого каталога;
- каталог нельзя открыть через `SYS_FILE_OPEN` как обычный файл.

Эти ограничения намеренные. Следующий этап может добавить `cd`, `pwd`,
нормализацию `.`/`..` и отдельный `opendir/readdir` API.

## Исправление сборки образа: v60.1

Исправлена ошибка в `build.sh`: `mkfat16` принимает назначение файла в формате
`HOSTFILE=DESTPATH`. Предыдущий вариант передавал `BIN/HELLO.EXE` и подобные
строки как отдельные имена исходных файлов, поэтому сборка пыталась открыть
несуществующий файл `BIN/HELLO.EXE` на хосте.

Используемый теперь формат:

```text
build/HELLO.EXE=BIN/HELLO.EXE
build/NETDRV.EXE=BIN/NETDRV.EXE
build/NET.CFG=DOC/NET.CFG
```

### Обязательная проверка на Windows 7 / W64DevKit

После распаковки проекта выполнить `build.bat` или `sh build.sh` в окружении
W64DevKit x86. На этапе `mkfat16` не должно появляться сообщения:

```text
bin/hello.exe: No such file or directory
```

После создания образа должны успешно пройти проверки:

```text
fat16check.exe build/disk.img BIN
fat16check.exe build/disk.img BIN/HELLO.EXE
fat16check.exe build/disk.img BIN/NETDRV.EXE
fat16check.exe build/disk.img DOC/NET.CFG
```
