#!/bin/sh
# Host-side FIX53 acceptance: exercise different kernel/file/layout sizes and
# verify that corruption and low-memory overflow are rejected.
set -eu
TOOLDIR=${1:-build}
TEMPLATE=${2:-build/layout_loader.bin}
MKFAT="$TOOLDIR/mkfat16.exe"
MKLAY="$TOOLDIR/mklayout.exe"
LAYCHK="$TOOLDIR/layoutcheck.exe"
fail(){ echo "FIX53-VARIANTS FAILED: $1" >&2; exit 1; }
[ -x "$MKFAT" ] || fail "missing mkfat16"
[ -x "$MKLAY" ] || fail "missing mklayout"
[ -x "$LAYCHK" ] || fail "missing layoutcheck"
[ -f "$TEMPLATE" ] || fail "missing layout-loader template"
TMP=${TMPDIR:-/tmp}/toyos_fix53_variants_$$
mkdir -p "$TMP"
trap 'rm -rf "$TMP"' EXIT HUP INT TERM
printf 'small\n' > "$TMP/SMALL.BIN"
dd if=/dev/zero of="$TMP/LARGE.BIN" bs=1024 count=2048 2>/dev/null

make_variant(){
    tag=$1; part=$2; freepct=$3; kbytes=$4; file=$5
    ksecs=$(( (kbytes + 511) / 512 ))
    reserve=$(( (ksecs * 25 + 99) / 100 )); [ "$reserve" -ge 64 ] || reserve=64
    # part is deliberately chosen aligned to 256 and sufficiently far from kernel.
    gap=$(( part - (2 + ksecs + reserve) ))
    [ "$gap" -ge 128 ] || fail "$tag protected gap"
    "$MKFAT" "$TMP/$tag.img" --part-lba "$part" --free-percent "$freepct" "$file=$tag.BIN" >/dev/null
    imgbytes=`wc -c < "$TMP/$tag.img" | tr -d ' '`
    imgsecs=$(( imgbytes / 512 )); fatsecs=$(( imgsecs - part ))
    "$MKLAY" "$TEMPLATE" "$TMP/$tag.layout" 2 "$kbytes" "$ksecs" "$reserve" "$gap" "$part" "$fatsecs" "$imgsecs" 256 "$freepct" 32256 655360
    dd if="$TMP/$tag.layout" of="$TMP/$tag.img" bs=512 seek=1 count=1 conv=notrunc 2>/dev/null
    "$LAYCHK" "$TMP/$tag.img" >/dev/null || fail "$tag valid layout rejected"
}

make_variant V1 768 25 100000 "$TMP/SMALL.BIN"
make_variant V2 1024 40 300000 "$TMP/LARGE.BIN"
S1=`wc -c < "$TMP/V1.img" | tr -d ' '`
S2=`wc -c < "$TMP/V2.img" | tr -d ' '`
[ "$S2" -gt "$S1" ] || fail "image did not grow for larger packed data/free-space policy"

# Corrupted descriptor must be rejected.
cp "$TMP/V1.img" "$TMP/BADCRC.img"
printf '\000' | dd of="$TMP/BADCRC.img" bs=1 seek=512 count=1 conv=notrunc 2>/dev/null
if "$LAYCHK" "$TMP/BADCRC.img" >/dev/null 2>&1; then fail "corrupted descriptor accepted"; fi

# A descriptor whose kernel image exceeds the declared safe RAM window must fail.
part=768; kbytes=100000; ksecs=$(( (kbytes + 511) / 512 )); reserve=64; gap=$(( part-(2+ksecs+reserve) ))
imgbytes=`wc -c < "$TMP/V1.img" | tr -d ' '`; imgsecs=$((imgbytes/512)); fatsecs=$((imgsecs-part))
"$MKLAY" "$TEMPLATE" "$TMP/BADMEM.layout" 2 "$kbytes" "$ksecs" "$reserve" "$gap" "$part" "$fatsecs" "$imgsecs" 256 25 32256 65536
cp "$TMP/V1.img" "$TMP/BADMEM.img"
dd if="$TMP/BADMEM.layout" of="$TMP/BADMEM.img" bs=512 seek=1 count=1 conv=notrunc 2>/dev/null
if "$LAYCHK" "$TMP/BADMEM.img" >/dev/null 2>&1; then fail "unsafe kernel memory range accepted"; fi

echo "FIX53-LAYOUT-VARIANTS: PASS"
