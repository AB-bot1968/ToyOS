# Проверка FIX60S

Статически проверяются: freestanding i386 compilation изменённых C-файлов,
build.sh syntax, checker chain, упаковка TESTLGR/SONARPUB/SONARLOG и неизменность
SONARDRV/UARTRX/SONARSIM относительно FIX60Q STABLE.

Runtime acceptance в W64DevKit/QEMU:
1. test TESTDATA.TST
2. test TESTLG60.TST
3. test TESTSONP.TST
4. test TESTLGR.TST

TESTLGR обязан завершиться FAIL=0. После этого физический TCP стенд:
execmt SONARDRV.EXE SONARLOG.EXE
Обе задачи должны оставаться активными, /SONAR.LOG — 4160 bytes.

Architecture Vision изменён: зафиксирована слоистая deterministic acceptance
SONARLOG и правило process-owned Data Channel reader cleanup.
