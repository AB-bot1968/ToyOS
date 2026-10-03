# Verification — ToyOS v67.11-B FIX60T

Host gate: `./check_fix60t_sonarview.sh`.

Runtime smoke test после создания журнала FIX60S:

1. `filesize /SONAR.LOG` -> 4160.
2. `exec SONARVWR.EXE`.
3. Должен появиться заголовок `SONARVIEW: seq time_us x_mm y_mm z_mm status generation`, summary и записи в возрастающем логическом порядке sequence для нормального журнала.
4. После просмотра `/SONAR.LOG` остаётся размером 4160 bytes и `ASSERT SONAR_LOG_VALID 128` продолжает проходить.
5. При отсутствии файла, неверном размере или двух невалидных заголовках viewer завершается с диагностикой и ничего не создаёт/не исправляет.

Полная регрессия: штатный `build.sh`, включая всю цепочку FIX60..FIX60S и новый FIX60T gate.

FAT 8.3 regression: `./check_fix60c_fat83_names.sh` обязан завершаться `FIX60C FAT83 CHECK OK`; FIX60T gate также запускает эту проверку сам. Исключение: только `AUTOSTART.SH`.
