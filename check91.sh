#!/bin/sh
set -eu
# v65.9: font plane 2 is saved before graphics mode and restored before B8000.
grep -q 'static uint8_t vga_saved_font\[256u\*16u\]' src/kernel.c
grep -q 'vga_save_font();' src/kernel.c
grep -q 'vga_restore_saved_font();' src/kernel.c
grep -q 'outb(VGA_SEQ,0x02);outb(VGA_SEQ_DATA,0x04);' src/kernel.c
grep -q 'outb(VGA_SEQ,0x04);outb(VGA_SEQ_DATA,0x07);' src/kernel.c
grep -q 'outb(VGA_GC,0x05);outb(VGA_GC_DATA,0x00);' src/kernel.c
grep -q 'outb(VGA_GC,0x06);outb(VGA_GC_DATA,0x00);' src/kernel.c
grep -q 'outb(VGA_SEQ,0x04);outb(VGA_SEQ_DATA,0x07);' src/kernel.c
python3 - <<'PY'
from pathlib import Path
s=Path('src/kernel.c').read_text()
a=s.index('case SYS_VIDEO_TEXT:')
b=s.index('case SYS_EXEC_ARG:',a)
part=s[a:b]
assert part.index('vga_restore_text_mode_hw()') < part.index('vga_restore_saved_font()') < part.index('console_clear()')
print('PASS: mode set -> font restore -> B8000 clear order')
PY
echo 'check91: PASS'
