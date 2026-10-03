@echo off rem Отключаем отображение самих команд сборки в консоли.
cd /d "%~dp0" rem Переходим в каталог, где расположен этот BAT-файл.
where gcc >nul 2>nul rem Проверяем наличие GCC в PATH.
if errorlevel 1 ( rem Если GCC не найден, начинаем обработку ошибки.
    echo ERROR: gcc.exe not found in PATH. rem Сообщаем пользователю об отсутствии GCC.
    exit /b 1 rem Завершаем сборку с кодом ошибки.
) rem Закрываем блок проверки наличия GCC.
gcc -std=c11 -O2 -Wall -Wextra -Wpedantic -o win_sendfile.exe win_sendfile.c -lws2_32 rem Компилируем исходник и подключаем Winsock 2.
if errorlevel 1 ( rem Если GCC сообщил об ошибке, начинаем обработку ошибки сборки.
    echo ERROR: build failed. rem Сообщаем о неудачной компиляции.
    exit /b 1 rem Завершаем сборку с кодом ошибки.
) rem Закрываем блок проверки результата GCC.
echo BUILD OK: win_sendfile.exe rem Сообщаем об успешном создании программы.
exit /b 0 rem Завершаем BAT-файл успешно.
