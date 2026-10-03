#!/bin/sh
set -eu
fail(){ echo "CHECK FAILED: $1" >&2; exit 1; }
# Regression for the observed: FAIL code=4294967294 (-2).
# SYS_RT_START receives a binary rt_start_request, so rt_start_task() must not
# call user_cstr() on the struct pointer. It must validate req.name instead.
grep -Rq 'validate the actual name field instead' changes/RTD || fail missing-fix-comment
grep -q 'n=0;while(n<QUEUE_NAME_SIZE&&req.name\[n\])n++;' src/kernel.c || fail binary-request-name-validation
grep -q 'if(n==0u||n>=QUEUE_NAME_SIZE)return EXE_ERR_NAME;' src/kernel.c || fail name-termination-check
if grep -q 'if(!user_cstr(p))return EXE_ERR_NAME;' src/kernel.c; then fail old-bug-still-present; fi
# The user's exact command must fit the unchanged 3x16-byte EXE1 argument ABI.
grep -q 'exec RTD.EXE SENSOR.EXE PERIOD_MS DEADLINE_MS PRIORITY' src/user_shell.c || fail command-abi
# RTD must pass the request structure, not a C-string pointer.
grep -q 'SYS_RT_START,(uint32_t)\&req' src/rtd.c || fail rtd-request-call
printf '%s\n' 'RTD stage 1 error-fix checks passed'
