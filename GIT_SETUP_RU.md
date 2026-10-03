# ToyOS FIX60ZEI — подготовка и публикация в Git

Эта версия подготовлена для безопасной первичной публикации стабильного baseline **ToyOS v67.11-B FIX60ZEI**.

Целевой GitHub repository: **`AB-bot1968/ToyOS`**.

Рабочий runtime-код не изменён ради Git-публикации. Перед commit/push выполняется автоматическая проверка frozen SHA-256 manifest, 45 acceptance-тестов, обязательных ресурсов и типичных утечек credentials.

## 1. Что подготовлено

В корне находятся:

- `git_preflight.sh` / `git_preflight.bat` — безопасная проверка перед Git;
- `git_publish.sh` / `git_publish.bat` — init + commit + branch + tag + push;
- `github_publish.sh` / `github_publish.bat` — необязательная полная GitHub-автоматизация через `gh`;
- `GIT_RELEASE.env` — единые имя релиза, branch, tag и контрольные hashes;
- `FIX60ZEI_FROZEN_CONTENT_SHA256.txt` — manifest frozen baseline;
- `.github/workflows/preflight.yml` — CI-проверка source/release integrity;
- `.gitignore` — build/runtime/credential exclusions с исключениями для обязательных `SPLASH.RAW` и `TEST.BIN`.

Release metadata:

```text
branch: main
tag:    v67.11-B-FIX60ZEI
commit: ToyOS v67.11-B FIX60ZEI stable baseline
```

## 2. Автор публикации и Git identity

Для этого зафиксированного релиза автор публикации задан в `GIT_RELEASE.env`:

```text
Ботнев Александр Валерьевич
101033309+AB-bot1968@users.noreply.github.com
```

`git_publish.sh` записывает эти значения **только в локальную конфигурацию данного репозитория** (`git config user.name/user.email`) и не изменяет глобальные настройки пользователя. Noreply-адрес связывает commit с GitHub-аккаунтом `AB-bot1968`, не раскрывая личный e-mail.

Проверьте наличие Git:

```sh
git --version
```

Никогда не записывайте пароль, Personal Access Token или private SSH key в исходники, BAT/SH-файлы или Git history.

## 3. Обязательный preflight

Из W64DevKit shell или Git Bash:

```sh
./git_preflight.sh
```

Из Windows `cmd.exe`:

```bat
git_preflight.bat
```

Preflight проверяет:

1. frozen content SHA-256 manifest;
2. неизменность `kernel.c` и `TESTMRT.TST`;
3. наличие 45 уникальных acceptance-тестов;
4. отсутствие `build/`, `dist/`, `TST.LOG`, `B*.TST`;
5. наличие обязательных binary resources/fixtures;
6. базовые сигнатуры случайно опубликованных credentials;
7. отсутствие файлов >50 MiB;
8. shell syntax публикационных/сборочных скриптов;
9. `check_fix60zei_idle_heartbeat.sh`, если доступен Python 3.

## 4. Самый безопасный вариант: сначала только локальный Git

```sh
./git_publish.sh --local
```

или:

```bat
git_publish.bat --local
```

Скрипт:

1. запускает preflight;
2. задаёт локальную Git identity автора из `GIT_RELEASE.env`;
3. выполняет `git init`;
4. индексирует файлы;
5. проверяет, что `resources/SPLASH.RAW` и `tools/Python/TEST.BIN` реально tracked;
6. выполняет `git diff --cached --check`;
7. создаёт root commit;
8. переименовывает ветку в `main`;
9. создаёт annotated tag `v67.11-B-FIX60ZEI`;
10. **ничего не отправляет в сеть**.

После этого полезно выполнить:

```sh
git status
git log --oneline --decorate -1
git show --stat v67.11-B-FIX60ZEI
```

## 5. Универсальная публикация в GitHub/GitLab/другой Git server

Сначала создайте **пустой** remote repository без README/LICENSE/.gitignore на сервере.

Затем одной командой:

```sh
./git_publish.sh https://github.com/AB-bot1968/ToyOS.git
```

или SSH:

```sh
./git_publish.sh git@github.com:AB-bot1968/ToyOS.git
```

Windows:

```bat
git_publish.bat https://github.com/AB-bot1968/ToyOS.git
```

Скрипт не использует force push. Если remote `main` или release tag уже содержат другую историю, публикация остановится.

Повторный запуск для уже опубликованного **того же самого** commit/tag безопасен: совпадение будет распознано, повторное переписывание истории не выполняется.

## 6. Максимальная автоматизация GitHub через `gh`

Если установлен GitHub CLI и выполнен `gh auth login`:

```sh
./github_publish.sh AB-bot1968/ToyOS public
```

или приватный репозиторий:

```sh
./github_publish.sh AB-bot1968/ToyOS private
```

Сценарий:

1. выполняет local Git preparation;
2. при необходимости создаёт GitHub repository;
3. публикует `main` и release tag;
4. формирует `dist/ToyOS_v67_11_B_FIX60ZEI_SOURCE.zip` командой `git archive`;
5. создаёт GitHub Release и прикладывает source ZIP;
6. использует `GITHUB_RELEASE_NOTES_FIX60ZEI_RU.md` как описание релиза.

`dist/` находится в `.gitignore` и в историю не попадает.

## 7. CI после публикации

`.github/workflows/preflight.yml` автоматически запускает `git_preflight.sh` на GitHub при push и pull request.

Это **не замена** полной сборки и QEMU runtime acceptance. CI подтверждает целостность source/release tree. Авторитетная runtime-проверка текущего baseline — 45/45 acceptance и длительный смешанный сценарий, уже выполненные на рабочей системе.

## 8. Сборка после clone

```sh
git clone <URL>
cd ToyOS
./git_preflight.sh
./build.sh
```

Для `build.sh` требуется именно x86/i686 W64DevKit toolchain.

## 9. Авторское право и лицензия

Автор и правообладатель: **Ботнев Александр Валерьевич**.

`COPYRIGHT` содержит: `Copyright © 2026 Ботнев Александр Валерьевич. All rights reserved.`

Открытая лицензия намеренно не выбрана автоматически. Публичный репозиторий без `LICENSE` остаётся защищённым авторским правом; подробности — `LICENSE_STATUS_RU.md`.

## 10. Если публикация прервалась

Скрипты не выполняют force push и не удаляют remote history.

Проверьте:

```sh
git status
git remote -v
git log --oneline --decorate -3
git tag -n
```

Если remote оказался неправильным, не меняйте его автоматически: сначала проверьте адрес, затем вручную выполните `git remote set-url origin ...` только после осознанного решения.

## Windows cmd.exe после создания GitHub repository

Для текущего `AB-bot1968/ToyOS` удалённый `main` уже существует и содержит publication bootstrap. Поэтому полный FIX60ZEI публикуется не через сценарий «пустого remote», а так:

```bat
publish_to_existing_github.bat
```

Windows BAT-path является нативным: `git_preflight.bat` вызывает `git_preflight_windows.ps1`, который вычисляет SHA-256 через .NET и **не требует `sha256sum`**. Файлы BAT оставлены ASCII-only; имя автора задаётся отдельным PowerShell UTF-8 BOM скриптом, чтобы `cmd.exe` не искажал кириллицу.

Для текущей рабочей машины **рекомендуемый путь — только SSH через W64DevKit/Git Bash**:

```sh
./publish_to_existing_github.sh
```

Скрипт по умолчанию использует `git@github.com:AB-bot1968/ToyOS.git`, автоматически ищет OpenSSH из Git for Windows и до создания release commit проверяет сначала read-доступ (`git ls-remote`), затем write-доступ (`git push --dry-run`). HTTPS, browser-auth и Git Credential Manager для этого сценария не используются.

BAT-вариант сохранён как вспомогательный, но для данной машины после подтверждённой SSH-аутентификации его использовать не требуется. Публикация остаётся fast-forward only; force push не используется.


## W64DevKit: SSH discovery без /c/ mount

На проверенной машине W64DevKit команда `ssh` отсутствует в PATH, хотя OpenSSH установлен вместе с Git for Windows в `C:\Program Files\Git\usr\bin\ssh.exe`. W64DevKit shell не обязан понимать Git-Bash/MSYS путь `/c/Program Files/...`.

Актуальный `publish_to_existing_github.sh` получает путь установки Git через:

```sh
git --exec-path
```

и автоматически строит native Git-for-Windows путь к `usr/bin/ssh.exe`. Путь с пробелами передаётся через quoted `GIT_SSH_COMMAND`. Если Git установлен нестандартно, можно явно задать:

```sh
TOYOS_SSH_BIN='C:/Program Files/Git/usr/bin/ssh.exe' ./publish_to_existing_github.sh
```

Проверка крупных файлов также не использует GNU-only `find -size +50M`; размер сверяется через `wc -c`, поэтому preflight совместим с BusyBox/W64DevKit.
