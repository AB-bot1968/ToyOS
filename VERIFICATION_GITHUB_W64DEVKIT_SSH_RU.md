# Verification — FIX60ZEI W64DevKit SSH publication tooling

Проверено в подготовительной среде:

- `sh -n publish_to_existing_github.sh` — PASS;
- `sh -n git_preflight.sh` — PASS;
- полный `git_preflight.sh` — PASS;
- frozen manifest: 1109 entries — PASS;
- `src/kernel.c` SHA-256 остаётся `3af4e5b2a9c8610eaee3efb1b533836dadf843e1b212922c9fee354c860dd001`;
- `TST/TESTMRT.TST` SHA-256 остаётся `8bff965aa737de277a2bc901456508ff0199ae1ec350c85196af7f4f78302985`;
- acceptance list: 45 unique tests — PASS;
- large-file check больше не использует GNU `find -size +50M`;
- SSH discovery не зависит от `/c/...` mount и допускает explicit `TOYOS_SSH_BIN`;
- force push отсутствует.

Реальный GitHub publish должен быть подтверждён на пользовательской W64DevKit машине после запуска нового `publish_to_existing_github.sh`.

Дополнительно выполнен полный локальный regression publish-cycle поверх существующего `main`: первый запуск создал один fast-forward release commit и annotated tag, второй запуск был идемпотентным. После fresh clone повторный `git_preflight.sh` прошёл успешно; tag указывает на HEAD release commit; автор commit — `Ботнев Александр Валерьевич <101033309+AB-bot1968@users.noreply.github.com>`.
