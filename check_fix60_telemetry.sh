#!/bin/sh
set -eu
fail(){ echo "FIX60 CHECK FAIL: $1"; exit 1; }
grep -q 'SYS_FILE_PREAD.*72u' src/kernel.c || fail PREAD
grep -q 'SYS_FILE_PWRITE.*73u' src/kernel.c || fail PWRITE
grep -q 'off>h->size||count>h->size-off' src/kernel.c || fail NO_GROW
grep -q 'SONARLOG.EXE' build.sh || fail SONARLOG_PACK
grep -q 'TST/TESTLG60.TST' build.sh || fail TEST_PACK
grep -q 'TLOG_CAPACITY 128u' src/sonarlog.c || fail CAPACITY
grep -q 'tlog_crc32' src/telemetry_log.c || fail CRC
grep -q 'SYS_DATA_CHANNEL' src/sonarlog.c || fail DATA_CHANNEL
grep -q 'SYS_PORT_OUT8    13u' src/kernel.c || fail PORT13
grep -q 'SYS_PORT_IN8     14u' src/kernel.c || fail PORT14
grep -q 'FIX60 — bounded cyclic telemetry persistence' TOYOS_ARCHITECTURE_VISION.md || fail VISION
echo 'FIX60 telemetry static checks: PASS'
