# Verification — FIX55A

Сначала выполнить `test TESTUART.TST`; ожидается FAIL=0 и строка `TEST EXPECT UART_TRANSPORT: OK`.
Затем выполнить полный regression suite стабильного FIX54A/FIX55, особенно TESTINF, TESTLOAD, TESTKEY, TESTRST, TESTLAY.
FIX55A остаётся CANDIDATE до runtime подтверждения W64DevKit/QEMU.
