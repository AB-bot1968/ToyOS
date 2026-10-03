# FIX60ZEI — W64DevKit SSH publication compatibility fix

Исправлена только GitHub publication infrastructure. Runtime ToyOS, `src/`, `TST/`, scheduler policy и frozen hashes FIX60ZEI не изменялись.

На реальной машине W64DevKit обнаружены две несовместимости shell-tooling:

1. MSYS-путь `/c/Program Files/Git/usr/bin/ssh.exe` не виден из W64DevKit, хотя native Windows-файл `C:\Program Files\Git\usr\bin\ssh.exe` существует и ранее успешно прошёл `ssh -T git@github.com`.
2. Встроенный `find` не поддерживает GNU-форму `-size +50M`.

`publish_to_existing_github.sh` теперь вычисляет Git-for-Windows root через `git --exec-path` и использует native-путь к `ssh.exe` через quoted `GIT_SSH_COMMAND`. При необходимости поддерживается `TOYOS_SSH_BIN`.

`git_preflight.sh` проверяет размер файлов через `wc -c` с порогом 52428800 bytes и больше не выводит кириллицу в PASS-строке copyright, хотя точная UTF-8 строка правообладателя продолжает проверяться.
