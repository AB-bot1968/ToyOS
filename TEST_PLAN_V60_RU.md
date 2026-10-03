# Toy OS v60 — пошаговый план тестирования

## Шаг 0. Сборка

В Windows 7 открыть W64DevKit x86 shell и выполнить:

    cd /путь/к/ToyOS_v60
    sh build.sh

Либо:

    build.bat

Сборка должна завершиться `Build and verification OK`.

## Шаг 1. Автоматические проверки исходников

`build.sh` запускает существующий набор регрессионных проверок и новый `check61.sh`.
`check61.sh` содержит три независимых аудита исходников и три прохода компиляции
FS-изменений.

## Шаг 2. Root listing

В QEMU:

    ls

Должны присутствовать `BIN/` и `DOC/`.

## Шаг 3. Listing каталога

    ls /BIN
    ls /DOC

В `/BIN` должны быть `HELLO.EXE` и `NETDRV.EXE`.

## Шаг 4. Файл в каталоге

    write /DOC/TEST.TXT hello
    cat /DOC/TEST.TXT
    filesize /DOC/TEST.TXT

Ожидается `hello` и `5 bytes`.

## Шаг 5. Append

    append /DOC/TEST.TXT world
    cat /DOC/TEST.TXT

Ожидается `helloworld`.

## Шаг 6. Вложенный каталог

    mkdir /DOC/SUB
    write /DOC/SUB/TEST.TXT nested
    cat /DOC/SUB/TEST.TXT
    ls /DOC/SUB

## Шаг 7. Непустой rmdir должен быть запрещён

    rmdir /DOC/SUB

Ожидается `rmdir: FAIL`.

## Шаг 8. Удаление файла и каталога

    delete /DOC/SUB/TEST.TXT
    rmdir /DOC/SUB

Ожидается два успешных результата.

## Шаг 9. Проверка EXE1 по пути

    exec /BIN/HELLO.EXE

Ожидается обычное успешное выполнение HELLO.

## Шаг 10. Регрессия старого root API

    write TEST.TXT root
    cat TEST.TXT
    delete TEST.TXT

Старые операции без каталогов должны продолжить работать.
