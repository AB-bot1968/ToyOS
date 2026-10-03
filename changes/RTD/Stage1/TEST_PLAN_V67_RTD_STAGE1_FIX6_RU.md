# Test plan — RTD Stage 1 FIX6

1. Запустить `exec RTD.EXE SENSOR.EXE 20 20 3`.
2. Убедиться, что появляются `SENSOR: tick`.
3. Нажать ESC.
4. Убедиться, что появляется `SENSOR: ESC -> stopped`.
5. Убедиться, что сразу после этого появляется новая строка с `toy0> `.
6. Ввести `ticks`, `help` и `ls` и убедиться, что shell продолжает работать.

Статическая проверка `check_rtd_stage1_fix6.sh` должна завершаться сообщением `RTD stage1 FIX6 prompt checks passed`.
