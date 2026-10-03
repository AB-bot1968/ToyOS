# Toy OS v61 — план тестирования cwd / cd / pwd

## 1. Сборка

В Windows 7 + x86 W64DevKit запустить `build.bat`. Сборка должна завершиться
без ошибок, создать `build/toy_os.img` и пройти `check64.sh` и `check65.sh`.

## 2. Базовый cwd

После входа в shell:

```text
pwd
```

Ожидается:

```text
/
```

## 3. Переход в каталог

```text
cd /BIN
pwd
ls
```

Ожидается cwd `/BIN`, а `ls` должен показать содержимое `/BIN`, включая
`HELLO.EXE` и `NETDRV.EXE`.

## 4. Относительные пути

```text
cd /BIN
cat HELLO.EXE
filesize HELLO.EXE
ls .
```

Команды должны искать файлы относительно `/BIN`.

## 5. Нормализация

```text
cd /BIN/./../BIN/./
pwd
cd ..
pwd
```

Ожидается `/BIN`, затем `/`.

## 6. Защита ROOT

```text
cd /
cd ..
pwd
```

Ожидается `/`; выход выше ROOT запрещён нормализатором.

## 7. Ошибка перехода

```text
cd /NO_SUCH_DIR
```

Ожидается `cd: FAIL`, cwd при этом не меняется.

## 8. Автоматический тест

```text
cwd-test
```

Ожидается `cwd-test: PASS`. Тест использует каталог `BIN`, проверяет `/BIN`,
`./../BIN/./`, `..`, `pwd` и относительный `ls`, после чего восстанавливает
исходный cwd.

## 9. Полный syscall regression

```text
syscall-test
```

После выполнения должен присутствовать `=== ALL TESTS PASS ===`.
