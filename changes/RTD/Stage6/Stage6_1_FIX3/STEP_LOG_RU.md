# STEP LOG — Stage 6.1 FIX3

1. Зафиксирован runtime-симптом: `SENSOR1.EXE 10 10 3` выдаёт только один `tick`.
2. Прослежен путь `SYS_RT_WAIT -> TASK_BLOCKED -> next release`.
3. Установлено, что следующий IRQ0 часто приходит при `f->cs=Ring0` внутри `SYS_CONSOLE_READ`.
4. Подтверждено, что FIX1 корректно запрещает context switch из Ring-0, но ранний `return` не позволял вызвать `rt_release_jobs()`.
5. В Ring-0 ветку добавлен безопасный вызов `rt_release_jobs(now)`.
6. Добавлен focused regression `check_rtd_stage6_1_fix3.sh`.
7. Проверки изменённых C-файлов выполнены в 32-bit freestanding режиме.
8. Предыдущие RTD regression tests выполнены отдельно.
9. Полный архив сформирован без каталога `build/`.
