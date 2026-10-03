# Проверка ToyOS v67.11-B FIX36B

## 1. Сборка

Собрать штатным `build.sh` в W64DevKit. Нельзя ослаблять target checks. Убедиться, что EXIT0.EXE/EXIT7.EXE собираются, FAT16 остаётся с LBA 512, kernel area 256 sectors.

## 2. Сначала воспроизвести точную ошибку FIX36A

1. Запустить 4 RT + 4 MT.
2. `ps`, выбрать реально работающий MT PID.
3. `wait PID` этого MT. Должно появиться `WAIT: ESC to cancel`; RT/MT не должны зависнуть. Нажать ESC: ожидается `wait: cancelled` и немедленный рабочий `toy0>`.
4. Сразу выполнить `mtstat`, `rtstat`, `ps`, `ls`. Система должна отвечать, счётчики работающих задач должны продолжать изменяться.
5. Повторить для реально работающего RT PID (включая SLOT3, соответствующий исходному тесту). Нажать ESC и повторить `rtstat`, `mtstat`, `ps`, `ls`.
6. Повторить несколько раз подряд MT -> RT -> MT, чтобы проверить отсутствие повреждения shell frame/CR3 и отсутствие оставшегося WAIT/ESC state.

## 3. Настоящее завершение во время wait

Использовать конечную EXIT7/FAULT задачу в RT и MT сценариях. `wait PID` должен сам завершиться после завершения процесса, без ESC, и показать сохранённый NORMAL/7 либо FAULT result. После возврата ESC не должен оставаться в keyboard queue.

## 4. Result history и slot reuse

Повторить FIX36A проверки: EXIT0 -> NORMAL/0; EXIT7 -> NORMAL/7; FAULTUD/GP/PF -> FAULT и правильный vector; старый PID после reuse RT/MT slot должен сохранять свой result до вытеснения из 64-entry history.

## 5. Нагрузка и регрессии

Обязательно проверить 4 MT + 8 SENSOR RT; fault RT ниже/между/выше SENSOR; RTSTAT WATCH + ESC; MTSTAT WATCH + ESC; `mtstop all` с первого Enter; PROCID FG/MT/RT; `syscall-test` с вводом OK без фона, с RT и с RT+MT; VGA SPLASH; RTDATA/MTDATA; ls/mkdir/read/write/cp/rm; обычный exec.

## 6. Критерий приемки

Любой hang, EXC, потеря shell, остановка RT/MT статистики, неверный exit status, повреждение WATCH/ESC, необходимость второго Enter после mtstop all или регрессия FIX35B означает, что FIX36B не фиксируется как stable. До успешной runtime-проверки stable rollback — FIX35B.
