# ToyOS v67.11-B FIX60I — проверка

Статически подтверждено:
- SONARDRV.EXE не содержит SYS_CONSOLE_WRITE и остаётся фоновым/тихим;
- sched_announce не пишет в console;
- UART op8 очищает software RX ring и bounded-drain physical RX FIFO;
- op8 не включает UART IRQ/IER;
- SONARDRV вызывает op8 перед каждой попыткой, включая retries и следующий циклический запрос.

Автотесты в текущей среде:
- check_fix58_sonar.sh: PASS
- check_fix59_sonarsim.sh: PASS
- check_fix60_telemetry.sh: PASS
- check_fix60a..check_fix60h: PASS
- check_fix60i_sonar_repeat.sh: PASS
- check67.sh: PASS
- i386 freestanding compile src/kernel.c: PASS (только ранее существующие warnings)
- i386 freestanding compile src/sonardrv.c: PASS

Полный build.sh здесь не является целевым Windows/W64DevKit+i686 окружением, поэтому финальную image/QEMU/com0com проверку необходимо выполнить на целевом стенде.

Физический acceptance:
1. Запустить SONARSIM normal на парном COM.
2. `execmt SONARDRV.EXE` должен сразу вернуть `toy0>`; фоновых `[MTxx] RUNNING`/SONAR сообщений быть не должно.
3. Ввод shell после появления `toy0>` должен продолжать работать.
4. SONARSIM `request=N` должен расти минимум до 10 без остановки после первой транзакции.
5. `mtlist`/`mtstat` должны показывать живой SONARDRV, heartbeat/данные должны обновляться.
6. `mtstop all` должен штатно остановить задачу.
