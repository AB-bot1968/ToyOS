#!/bin/sh
set -eu
fail(){ echo "FIX60C FAT83 CHECK FAIL: $1" >&2; exit 1; }
# Validate every literal destination packed by the active mkfat16 command.
line=$(grep '^\./build/mkfat16\.exe build/disk\.img ' build.sh) || fail mkfat16-command
[ -n "$line" ] || fail mkfat16-command
printf '%s\n' "$line" | tr ' ' '\n' | awk -F= '
function valid83(path, n,a,i,s,p,b,e) {
  n=split(path,a,"/");
  for(i=1;i<=n;i++) {
    s=a[i]; if(s=="") continue;
    if(s=="AUTOSTART.SH") continue;
    p=index(s,".");
    if(p){ b=substr(s,1,p-1); e=substr(s,p+1); if(index(e,".")) return 0; }
    else { b=s; e=""; }
    if(length(b)<1 || length(b)>8 || length(e)>3) return 0;
    if(b !~ /^[A-Za-z0-9_~-]+$/) return 0;
    if(e!="" && e !~ /^[A-Za-z0-9_~-]+$/) return 0;
  }
  return 1;
}
/=/ {
  d=$NF;
  sub(/\$AUTOSTART_FAT_ARG$/, "", d);
  if(d ~ /^\$/) next;
  if(!valid83(d)){ print "invalid FAT 8.3 destination: " d > "/dev/stderr"; bad=1; }
}
END { exit bad?1:0 }
' || fail packed-name
[ -f TST/TESTEXE.TST ] || fail renamed-test-missing
! grep -R 'TESTEXE60\.TST' build.sh check_fix60a_executable_loader.sh VERIFICATION_V67_11_B_FIX60A_RU.md VERIFICATION_V67_11_B_FIX60B_RU.md TST/TESTEXE.TST >/dev/null 2>&1 || fail stale-old-name
printf '%s\n' 'FIX60C FAT83 CHECK OK'
