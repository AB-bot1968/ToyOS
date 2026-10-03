#!/bin/sh
set -eu
# v65.10: после возврата из Mode 13h VGA должна быть именно в штатной
# текстовой адресации, а font plane 2 должен читаться/писаться через A0000.
grep -q 'static const uint8_t seq\[5\]={0x00,0x00,0x03,0x00,0x03};' src/kernel.c
# В font access SEQ04=07, GC04=02, GC06=00 (окно A0000).
python3 - <<'PY'
from pathlib import Path
s=Path('src/kernel.c').read_text()
start=s.index('static void vga_save_font(void)')
end=s.index('static void vga_restore_saved_font(void)', start)
part=s[start:end]
assert 'outb(VGA_SEQ,0x04);outb(VGA_SEQ_DATA,0x07);' in part
assert 'outb(VGA_GC,0x04);outb(VGA_GC_DATA,0x02);' in part
assert 'outb(VGA_GC,0x06);outb(VGA_GC_DATA,0x00);' in part
start=s.index('static void vga_restore_saved_font(void)')
end=s.index('/*\n * Возврат VGA', start)
part=s[start:end]
assert 'outb(VGA_SEQ,0x04);outb(VGA_SEQ_DATA,0x07);' in part
assert 'outb(VGA_GC,0x04);outb(VGA_GC_DATA,0x02);' in part
assert 'outb(VGA_GC,0x06);outb(VGA_GC_DATA,0x00);' in part
assert 'outb(VGA_SEQ,0x04);outb(VGA_SEQ_DATA,0x03);' in part
assert 'outb(VGA_GC,0x05);outb(VGA_GC_DATA,0x10);' in part
assert 'outb(VGA_GC,0x06);outb(VGA_GC_DATA,0x0e);' in part
# Строго проверяем порядок: mode-set -> font -> text mapping -> clear.
start=s.index('case SYS_VIDEO_TEXT:')
end=s.index('case SYS_EXEC_ARG:', start)
part=s[start:end]
assert part.index('vga_restore_text_mode_hw()') < part.index('vga_restore_saved_font()') < part.index('console_clear()')
print('check93: PASS')
PY
