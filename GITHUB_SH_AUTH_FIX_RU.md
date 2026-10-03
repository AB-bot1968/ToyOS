# FIX60ZEI — исправление `publish_to_existing_github.sh`

Дата: 2026-10-03

Проблема проявилась после успешного локального формирования большого Git commit: `git push` запускал browser-auth Git Credential Manager, после чего завершался `fatal` с нечитаемым локализованным сообщением.

Исправление сосредоточено только на shell-пути публикации:

- GitHub write-auth теперь проверяется через `git push --dry-run` до формирования release commit;
- при наличии `gh` используется `gh auth login/setup-git`;
- без `gh` используется Git Credential Manager, включая явный повторный `github login`;
- ошибки аутентификации сохраняются в `GITHUB_AUTH_LAST_ERROR.log`;
- публикация остаётся fast-forward only, force push не используется;
- tag `v67.11-B-FIX60ZEI` не перемещается на другой commit;
- повторный запуск идемпотентен.

Рабочий код ToyOS и acceptance-тесты не менялись.
