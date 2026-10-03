# Manifest — ToyOS v67 RTD Stage 5.1

## База

`ToyOS v67 RTD Stage 4.4`

## Новые/изменённые файлы Stage 5.1

```text
src/kernel.c
src/rt_deadline.h
src/rt_deadline_diag.c
build.sh
README.md
check_rtd_stage5_1.sh
tools/test_rt_deadline.c
changes/RTD/Stage5/Stage5_1/RELEASE_NOTES_RU.md
changes/RTD/Stage5/Stage5_1/TEST_PLAN_RU.md
changes/RTD/Stage5/Stage5_1/VERIFICATION_RU.md
changes/RTD/Stage5/Stage5_1/ARCHIVE_MANIFEST_RU.md
changes/RTD/Stage5/Stage5_1/STAGE5_1.patch
```

## Артефакты сборки

Каталог `build/` в исходный архив не включается.

`DEADLINE.EXE` генерируется штатным `build.sh` и помещается в FAT16 runtime image.

## Документация истории

Предыдущие RTD материалы находятся в `changes/RTD/`, а исторические проектные Markdown
файлы — в `changes/ProjectHistory/`.
