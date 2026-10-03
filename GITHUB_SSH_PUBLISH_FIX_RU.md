# FIX60ZEI — SSH-публикация GitHub

Дата: 2026-10-03

После проверки на рабочей машине подтверждена успешная SSH-аутентификация GitHub для аккаунта `AB-bot1968` с ключом `~/.ssh/id_ed25519`. Поэтому основной shell-путь публикации FIX60ZEI переведён с HTTPS/Git Credential Manager на SSH.

`publish_to_existing_github.sh` теперь по умолчанию использует:

`git@github.com:AB-bot1968/ToyOS.git`

Основные свойства:

- HTTPS, browser-auth, Git Credential Manager и `gh auth` больше не участвуют в shell-публикации;
- скрипт автоматически ищет `ssh` в `PATH`, затем в стандартных каталогах Git for Windows;
- найденный `ssh.exe` передаётся Git через `GIT_SSH`, поэтому отсутствие `ssh` в PATH W64DevKit не блокирует публикацию;
- до release commit выполняются `git ls-remote` и `git push --dry-run`, поэтому read/write SSH-доступ подтверждается заранее;
- после подтверждения доступа выполняются frozen-preflight, один fast-forward release commit и annotated tag `v67.11-B-FIX60ZEI`;
- `--force` не используется;
- повторный запуск остаётся идемпотентным.

Рабочий runtime-код ToyOS, scheduler, TST и frozen baseline FIX60ZEI не изменялись.
