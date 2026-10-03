#!/bin/sh
set -eu
grep -q 'load_cfg_file(\&c,cfg_name)' src/netdrv.c
grep -q 'cfg_name="NET_MAST.CFG"' src/netdrv.c
grep -q 'cfg_name="NET_SLV1.CFG"' src/netdrv.c
grep -q 'cfg_name="NET_SLV2.CFG"' src/netdrv.c
grep -q 'build/NET_MAST.CFG=NET_MAST.CFG' build.sh
grep -q 'build/NET_SLV1.CFG=NET_SLV1.CFG' build.sh
grep -q 'build/NET_SLV2.CFG=NET_SLV2.CFG' build.sh
for f in NET_MAST.CFG NET_SLV1.CFG NET_SLV2.CFG; do test -s "$f"; done
grep -q 'IP=192.168.100.10' NET_MAST.CFG
grep -q 'IP=192.168.100.11' NET_SLV1.CFG
grep -q 'IP=192.168.100.12' NET_SLV2.CFG
echo 'check105: PASS'
