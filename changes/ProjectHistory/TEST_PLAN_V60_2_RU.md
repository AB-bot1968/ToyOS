# Toy OS v60.2 — пошаговый план тестирования

## Шаг 1. Проверка сборочной среды

В Windows 7 открыть shell W64DevKit x86 и перейти в каталог проекта.
Запустить:

```text
build.bat
```

Ожидаемый результат в конце:

```text
Build and verification OK.
```

Если сборка остановилась, сохраняйте **первую** строку `ERROR:` и весь вывод
после неё.

## Шаг 2. Проверка ROOT DIRECTORY

После успешной сборки в QEMU:

```text
ls
```

В корне должны присутствовать как минимум:

- BOOT.BIN
- HELLO.EXE
- COMDRV.EXE
- LOADER.EXE
- NETDRV.EXE
- NET.CFG
- QPASS.EXE
- QPORT.EXE
- QFROMEXE.EXE
- QSTOP.EXE
- QNESTED.EXE
- QFILE.EXE
- QSIZE.EXE
- MT01.EXE ... MT25.EXE
- BIN/
- DOC/
- README.TXT

Появление файлов после 16-й записи ROOT является отдельной проверкой
исправления v60.2.

## Шаг 3. Проверка чтения системных файлов

```text
cat /NET.CFG
cat /README.TXT
filesize /NET.CFG
```

Ожидается нормальное чтение без `FAIL`.

## Шаг 4. Проверка каталогов

```text
ls /BIN
ls /DOC
cat /BIN/HELLO.EXE
cat /DOC/NET.CFG
```

Команды `cat` для бинарных EXE могут выводить нечитаемые символы — это нормально;
важно отсутствие `cat: FAIL`.

## Шаг 5. Проверка создания файлов в каталоге

```text
mkdir /TEST
write /TEST/HELLO.TXT hello
cat /TEST/HELLO.TXT
filesize /TEST/HELLO.TXT
mkdir /TEST/SUB
write /TEST/SUB/NEST.TXT nested
cat /TEST/SUB/NEST.TXT
ls /TEST/SUB
```

Ожидается корректное чтение `hello` и `nested`.

## Шаг 6. Проверка запрета удаления непустого каталога

```text
rmdir /TEST/SUB
```

Ожидается:

```text
rmdir: FAIL
```

После этого:

```text
delete /TEST/SUB/NEST.TXT
rmdir /TEST/SUB
delete /TEST/HELLO.TXT
rmdir /TEST
```

Все четыре операции удаления должны завершиться `OK`.

## Шаг 7. Проверка EXE1

```text
exec /HELLO.EXE
exec /BIN/HELLO.EXE
exec /COMDRV.EXE
```

Для COMDRV используйте существующий для проекта формат аргументов, если драйвер
ожидает параметры. Для проверки самого path resolver достаточно убедиться, что
файл найден и загрузчик не сообщает `exec: FAIL` из-за пути.

## Шаг 8. Что сообщить разработчику

Если тест не прошёл, передайте:

1. команду, которую вводили;
2. полный текст ответа shell;
3. при ошибке `build.bat` — полный вывод начиная с первой ошибки;
4. версию W64DevKit, если она известна.

Не продолжайте длинную последовательность тестов после первой серьёзной ошибки:
это упрощает точное определение причины.
