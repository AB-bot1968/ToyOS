# Toy OS v62 — план ручного тестирования

После успешной сборки:

```text
pwd
ls
cd /BIN
pwd
ls
cd ..
pwd
cd /DOC
pwd
ls
cd ../BIN/./
pwd
cwd-test
```

Ожидается отсутствие ошибок `FAIL`, а `cwd-test` должен завершиться строкой:

```text
cwd-test: PASS
```

Дополнительно проверить относительные файловые операции на существующих файлах:

```text
cd /BIN
filesize HELLO.EXE
cat ../DOC/NET.CFG
exec ./HELLO.EXE
cd /
```

Проверить защиту ROOT:

```text
cd /
cd ..
pwd
```

Ожидается `/`.

## v62.2 — обязательная проверка `ls`

После загрузки shell выполнить:

```text
pwd
ls
ls .
ls /
mkdir TEST
cd TEST
ls
ls .
cd ..
ls TEST
rmdir TEST
```

Ожидание: ни одна команда `ls` не должна печатать `ls: FAIL`. После `cd TEST`
команды `ls` и `ls .` должны успешно показать содержимое текущего пустого
каталога. `ls TEST` из `/` также должен завершаться успешно.
