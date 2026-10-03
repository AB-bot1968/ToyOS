# Проверка ToyOS v67.11-B FIX36E CRLF

База: подтвержденный runtime FIX36D. Изменение минимальное: новая shell-команда `crlf FILE` дописывает в существующий файл ровно CR/LF (0x0D 0x0A).

## 1. Сборка
Собрать штатным `build.sh` в W64DevKit. Ошибок компиляции/линковки быть не должно.

## 2. Основная проверка CRLF
После загрузки:

    write C.TXT ONE
    crlf C.TXT
    append C.TXT TWO
    crlf C.TXT
    append C.TXT THREE
    crlf C.TXT
    cat C.TXT

Ожидаемый вид:

    ONE
    TWO
    THREE

`filesize C.TXT` должен показать 17 bytes: ONE=3, CRLF=2, TWO=3, CRLF=2, THREE=5, CRLF=2.

## 3. Нечувствительность регистра

    write CASE.TXT A
    CrLf CASE.TXT
    append CASE.TXT B
    CRLF CASE.TXT
    cat CASE.TXT

Должны быть две строки A и B.

## 4. Несуществующий файл

    rm NOFILE.TXT
    crlf NOFILE.TXT

Ожидается `crlf: file not found/open failed`. Команда НЕ должна создавать NOFILE.TXT.

## 5. Ошибка синтаксиса

    crlf

Ожидается `usage: crlf FILE` и нормальный prompt.

## 6. Создание TST непосредственно в ToyOS

    write MYTEST.TST PRINT CRLF TEST
    crlf MYTEST.TST
    append MYTEST.TST ASSERT RT_ACTIVE 0
    crlf MYTEST.TST
    append MYTEST.TST ASSERT MT_ACTIVE 0
    crlf MYTEST.TST
    test MYTEST.TST

Ожидается успешное выполнение без EXC 14 и FAIL=0.

## 7. Regression FIX36D/FIX36B
Повторить `test TESTCORE.TST`, затем базовую проверку 4RT+4MT и `wait` активного RT/MT с ESC. Поведение должно совпадать с подтвержденными FIX36D/FIX36B.

После всех тестов выполнить `ps`, `rtstat`, `mtstat`, `ls` — shell должен оставаться работоспособным.
