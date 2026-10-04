# ToyOS v67.11-B FIX60ZEI

Стабильная зафиксированная версия учебной 32-битной ОС ToyOS для i386.

**Статус релиза:** подтверждённый стабильный baseline. Полный acceptance-набор из **45/45 тестов** пройден на рабочей системе; длительная совместная работа **2 MT + 4 RT + интерактивная консоль + SONARLOG + `execmt SONARVWR.EXE`** проверена без выявленных зависаний.

Рабочий runtime-код этой публикационной версии не изменён относительно frozen baseline FIX60ZEI. Git-обвязка добавлена отдельно и защищена preflight-проверками.

## Автор и авторское право

**Автор:** Ботнев Александр Валерьевич

**Copyright © 2026 Ботнев Александр Валерьевич. All rights reserved.**

Публичная публикация исходного кода сама по себе не предоставляет open-source лицензию. Пока владелец проекта отдельно не добавил файл `LICENSE`, все права сохраняются за автором. Подробности: `COPYRIGHT` и `LICENSE_STATUS_RU.md`.

## Ключевые возможности текущего baseline

- BIOS boot и 32-битный protected mode;
- Ring-3 shell через `INT 80h`;
- FAT16 и файловые операции;
- EXE1 foreground/MT/RT процессы;
- планирование MT и периодических RT-задач;
- RT sweep с ограничением повторного обслуживания слота внутри burst;
- foreground ownership и bounded MT slack;
- SONAR pipeline: `SONARDRV/SONARPUB`, `SONARLOG`, `SONARVWR`;
- UART / Modbus / telemetry / supervisor / watchdog infrastructure;
- 45 acceptance-тестов из `TST/ACCEPT.TXT`;
- console-only test reporting без `TST.LOG`.
- дальнейшая разработка и модернизация с использованием ИИ `TOYOS_DEVELOPMENT_RULES.md`.

`TOYOS_ARCHITECTURE_VISION.md` — долгосрочная архитектура. Мы уже ведём его на русском и продолжаем обновлять при каждом архитектурном изменении.
`TOYOS_CURRENT_STATE.md` — короткое текущее состояние проекта: какая версия зафиксирована, что проверено, какие hashes являются эталонными, что сейчас разрабатываем.
`TOYOS_DEVELOPMENT_RULES.md` — постоянные обязательные требования к работе ИИ. Именно сюда стоит записать всё, что нельзя забывать: не ломать рабочий код, тесты не ослаблять ради PASS, всегда создавать TST для новой функциональности, test должен проходить полный ACCEPT, не создавать build/ в релизе, обновлять Architecture Vision, всегда доводить исправление до конца и выдавать полный ZIP и т. д.
`GitHub AB-bot1968/ToyOS` остаётся внешней зафиксированной историей исходников. Тег v67.11-B-FIX60ZEI не изменяем.


## Сборка

Основная проверенная среда проекта:

- Windows 7 x64;
- x86 W64DevKit с i686-target GCC/binutils;
- QEMU для 32-битного runtime-тестирования.

Из W64DevKit shell:

```sh
cd ToyOS
./build.sh
```

или из `cmd.exe`:

```bat
cd ToyOS
build.bat
```

`build.sh` намеренно проверяет target компилятора и прекращает работу, если используется не i386/i686 toolchain.

## Acceptance-тесты


Загрузка ToyOS (Windows + Qemu):

```text
start.bat
```

После загрузки ToyOS:

```text
test
```

Для этого baseline ожидается:

```text
TESTS: 45  PASSED: 45  FAILED: 0
```

Отдельно критичны смешанные проверки MT/RT/console и SONAR viewer/logger.

## Live SONAR viewer

Viewer запускается как явная MT-задача:

```text
execmt SONARVWR.EXE
```

Это намеренная семантика текущей архитектуры: `exec` остаётся foreground-командой, а live viewer работает в существующей MT-сессии и не останавливает `SONARLOG`.

## Публикация в Git

Планируемый публичный репозиторий: `AB-bot1968/ToyOS`.

Перед публикацией выполните:

```sh
./git_preflight.sh
```

Локально подготовить первый commit и annotated tag без push:

```sh
./git_publish.sh --local
```

Опубликовать в уже созданный пустой Git-репозиторий:

```sh
./git_publish.sh https://github.com/AB-bot1968/ToyOS.git
```

В Windows можно использовать соответствующие `.bat` wrappers.

Для GitHub при наличии GitHub CLI доступен почти полностью автоматический сценарий:

```sh
./github_publish.sh AB-bot1968/ToyOS public
```

Он создаёт/использует GitHub repository, публикует `main`, tag `v67.11-B-FIX60ZEI`, формирует source ZIP через `git archive` и создаёт GitHub Release.

Подробности и безопасные варианты запуска: `GIT_SETUP_RU.md`.

## Защита стабильного baseline

Файл `FIX60ZEI_FROZEN_CONTENT_SHA256.txt` содержит SHA-256 почти всего frozen-дерева исходной стабильной версии. Публикационный preflight проверяет этот manifest до commit/push.

Ключевой hash ядра:

```text
src/kernel.c
3af4e5b2a9c8610eaee3efb1b533836dadf843e1b212922c9fee354c860dd001
```

SHA-256 исходного frozen ZIP FIX60ZEI:

```text
054713d08480739fd4fc869e350959525b96e150a157c0f2553fdb4604b6c434
```

## Репозиторий и сгенерированные файлы

В Git хранятся исходники, тесты, документация и необходимые бинарные ресурсы/fixtures, включая:

- `resources/SPLASH.RAW`;
- `resources/SPLASH_PREVIEW.png`;
- `tools/Python/TEST.BIN`.

Не должны попадать в Git:

- `build/`;
- `dist/`;
- runtime logs;
- QEMU VM images;
- локальные ключи/credentials;
- сгенерированные host binaries.

## Авторское право и лицензия

Copyright © 2026 **Ботнев Александр Валерьевич**. Все права сохранены.

Файл `COPYRIGHT` фиксирует авторство и правообладателя. Открытая лицензия проекта **не выбрана**; отсутствие `LICENSE` означает, что публичный просмотр репозитория не является разрешением на копирование, модификацию или распространение вне прав, предоставляемых применимым законодательством и условиями GitHub. См. `LICENSE_STATUS_RU.md`.

## История проекта

Исторические README, release notes, verification notes и patch history сохранены в `changes/ProjectHistory/` и других документах корня. Они не заменяют текущую архитектурную спецификацию.
