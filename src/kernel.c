typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

typedef int int32_t;

#include "path.h"
#include "rt_priority.h"
#include "rt_deadline.h"

#define VGA ((volatile uint16_t*)0xB8000u)
#define W 80u
#define H 25u
#define ATTR 0x07u
#define PIC1 0x20u
#define PIC2 0xA0u
#define PIT 0x40u
#define KBD 0x60u
#define KBD_STATUS 0x64u
#define VGA_CRTC 0x3d4u
#define VGA_CRTC_DATA 0x3d5u
#define VGA_SEQ 0x3c4u
#define VGA_SEQ_DATA 0x3c5u
#define VGA_GC 0x3ceu
#define VGA_GC_DATA 0x3cfu
#define VGA_ATTR 0x3c0u
#define VGA_STATUS1 0x3dau
#define VGA_MISC_WRITE 0x3c2u
#define VGA_DAC_WRITE 0x3c8u
#define VGA_DAC_DATA 0x3c9u
#define ATA_DATA 0x1F0u
#define ATA_ERR 0x1F1u
#define ATA_NSECT 0x1F2u
#define ATA_LBA0 0x1F3u
#define ATA_LBA1 0x1F4u
#define ATA_LBA2 0x1F5u
#define ATA_DRIVE 0x1F6u
#define ATA_STATUS 0x1F7u
#define ATA_CTRL 0x3F6u
#define SECTOR_SIZE 512u
#define FAT16_EOC 0xfff8u
#define FAT16_BAD 0xfff7u
#define FAT16_FREE 0x0000u
#define FAT16_MAX_HANDLES 8u
#define FAT16_MODE_READ   0x01u
#define FAT16_MODE_WRITE  0x02u
#define FAT16_MODE_CREATE 0x04u
#define FAT16_MODE_TRUNC  0x08u
#define FAT16_MODE_APPEND 0x10u

#define SYS_CONSOLE_WRITE 1u
#define SYS_CONSOLE_READ  2u
#define SYS_TIMER_GET     3u
#define SYS_DISK_READ     4u
#define SYS_DISK_WRITE    5u
#define SYS_FILE_OPEN     6u
#define SYS_FILE_READ     7u
#define SYS_FILE_WRITE    8u
#define SYS_FILE_CLOSE    9u
#define SYS_FILE_DELETE  10u
#define SYS_EXEC          11u
#define SYS_EXIT          12u
#define SYS_PORT_OUT8    13u
#define SYS_PORT_IN8     14u
#define SYS_EXEC_QUEUE  15u
#define SYS_QUEUE_STOP  16u
#define SYS_FILE_SIZE   17u
#define SYS_MT_START    18u
#define SYS_MT_STOP     19u
#define SYS_SCHED_BLOCK 20u
#define SYS_SCHED_WAKE  21u
#define SYS_SCHED_EXIT  22u
#define SYS_EXECMT      23u
#define SYS_LS          24u
#define SYS_EXEC_ARGS   25u
#define SYS_EXEC_QUEUE_ARGS 26u
#define SYS_MKDIR          27u
#define SYS_RMDIR          28u
#define SYS_CHDIR          29u
#define SYS_GETCWD         30u
#define SYS_FILE_COPY      31u
#define SYS_VIDEO_MAP      32u
#define SYS_VIDEO_UNMAP    33u
#define SYS_EXEC_ARG       34u
#define SYS_VIDEO_TEXT     35u
#define SYS_CONSOLE_AT     36u
#define SYS_CONSOLE_POLL   37u
#define SYS_RT_START       38u
#define SYS_RT_INFO        39u
#define SYS_RT_WAIT        40u
#define SYS_RT_TRACE       41u
#define SYS_RT_DEADLINE_INFO 42u
#define SYS_RT_STATUS      43u
#define SYS_RT_JOB_INFO    44u
#define SYS_RT_TIME_GET    45u
#define SYS_RT_JITTER_INFO 46u
#define SYS_RT_EXEC_INFO   47u
#define SYS_RT_STATS       48u
#define SYS_RT_YIELD       49u
#define SYS_RT_DATA        50u
#define SYS_MT_STATUS      51u
#define SYS_MT_STOP_ONE    52u
#define SYS_MT_DATA        53u
#define SYS_MT_STATS       54u /* FIX22: read-only MT scheduler diagnostics */
#define SYS_PROCESS_INFO    55u /* FIX33: read-only EXE1 lifecycle metadata */
#define SYS_PROCESS_WAIT    56u /* FIX36: retained termination result by PID */
#define SYS_PROCESS_SPAWN   57u /* FIX37: detached MT-class process + argv */
#define SYS_RESOURCE_INFO    58u /* FIX38: process-owned FAT handle diagnostics */
#define SYS_PROCESS_RESULT  59u /* FIX39: non-blocking retained-result query */
#define SYS_PROCESS_HEARTBEAT 60u /* FIX40: publish/query liveness */
#define SYS_PROCESS_STOP_PID  61u /* FIX40: supervisor timeout stop */
#define SYS_EVENT_LOG         62u /* FIX41: bounded kernel lifecycle event ring */
#define SYS_SAFE_MODE         63u /* FIX44: NORMAL/DEGRADED/SAFE state foundation */
#define SYS_HW_WATCHDOG       64u /* FIX46: explicit hardware-watchdog control-plane foundation */
#define SYS_SYSTEM_HEALTH      65u /* FIX47: read-only consolidated system-health snapshot */
#define SYS_SAFE_POLICY        66u /* FIX48: SAFE admission policy + diagnostics */
#define SYS_RECOVERY_EVENT     67u /* FIX49: append recovery/safety marker to RAM event ring */
#define SYS_RECOVERY_MANAGER   68u /* FIX50: bounded centralized recovery policy */
#define SYS_LAYOUT_INFO         69u /* FIX53: read-only dynamic disk layout descriptor */
#define SYS_UART_TRANSPORT      70u /* FIX55: bounded IRQ-driven COM1 transport */
#define SYS_DATA_CHANNEL        71u /* FIX56: bounded kernel Sensor/Data Channel */
#define SYS_FILE_PREAD          72u /* FIX60: positional read, does not change handle position */
#define SYS_FILE_PWRITE         73u /* FIX60: bounded positional write, never grows file */
#define SYS_MT_YIELD            74u /* FIX60J: cooperative detached-MT return to foreground */
#define SYS_EXECMT_COMMIT       75u /* FIX60O: PREPARED -> RUNNABLE; no context switch */
#define SAFE_POLICY_DENIED 0xfffffffcu
#define EVENT_REC_NORMAL 100u
#define EVENT_REC_DEGRADED 101u
#define EVENT_REC_SAFE 102u
#define EVENT_REC_RESTART 103u
#define EVENT_REC_WATCHDOG 104u
#define EVENT_REC_BUDGET 105u
#define RT_STATUS_STOP     1u
#define RT_STOP_ALL        0xffffffffu
#define SCHED_TASKS      25u
#define EXECMT_MAX_TASKS 25u
#define RT_MAX_TASKS      8u
#define MT_QUANTUM_TICKS  2u /* 20 ms at the 100-Hz PIT */
#define RT_DATA_MAX       255u
#define MT_DATA_MAX       255u
#define MT_DATA_BUF_SIZE  256u
#define MT_DATA_PUBLISH   0u
#define MT_DATA_READ      1u
#define RT_DATA_BUF_SIZE  256u
#define QUEUE_MAX_FILES 25u
#define QUEUE_MAX_REPS  3000u
#define QUEUE_NAME_SIZE 13u
#define EXEC_ARG_SIZE 16u
#define EXEC_ARG_COUNT 3u
#define EXEC_ARG_BLOCK_SIZE (EXEC_ARG_SIZE*EXEC_ARG_COUNT)
#define EXEC_QUEUE_RECORD_SIZE (QUEUE_NAME_SIZE+EXEC_ARG_BLOCK_SIZE)

#define FAT_OK 0
#define FAT_ERR (-1)
#define FAT_EOF 0

static inline void outb(uint16_t p,uint8_t v){__asm__ volatile("outb %0,%1"::"a"(v),"Nd"(p));}
static inline uint8_t inb(uint16_t p){uint8_t v;__asm__ volatile("inb %1,%0":"=a"(v):"Nd"(p));return v;}
static inline void outw(uint16_t p,uint16_t v){__asm__ volatile("outw %0,%1"::"a"(v),"Nd"(p));}
static inline uint16_t inw(uint16_t p){uint16_t v;__asm__ volatile("inw %1,%0":"=a"(v):"Nd"(p));return v;}
static inline void io_delay(void){(void)inb(0x80);}
static inline void cpu_hlt(void){__asm__ volatile("hlt");}

static void mem_set(void *d,uint8_t v,uint32_t n){uint8_t *p=(uint8_t*)d;while(n--)*p++=v;}
static void mem_copy(void *d,const void*s,uint32_t n){uint8_t *a=(uint8_t*)d;const uint8_t*b=(const uint8_t*)s;while(n--)*a++=*b++;}
static uint32_t str_len(const char*s){uint32_t n=0;while(s[n])n++;return n;}
static uint32_t read_cr3(void);

/* Аппаратный курсор VGA должен следовать за программными координатами консоли. */
static uint32_t vga_x,vga_y;
static void vga_cursor_update(void){uint16_t pos=(uint16_t)(vga_y*W+vga_x);outb(VGA_CRTC,0x0f);outb(VGA_CRTC_DATA,(uint8_t)pos);outb(VGA_CRTC,0x0e);outb(VGA_CRTC_DATA,(uint8_t)(pos>>8));}
static void vga_cell(uint32_t x,uint32_t y,char c){if(x<W&&y<H)VGA[y*W+x]=((uint16_t)ATTR<<8)|(uint8_t)c;}
static void console_clear(void){uint32_t i;for(i=0;i<W*H;i++)VGA[i]=((uint16_t)ATTR<<8)|' ';vga_x=vga_y=0;vga_cursor_update();}

/*
 * VGA сохраняет шрифт текстового режима в plane 2. Mode 13h с Chain-4
 * использует все четыре plane и поэтому неизбежно затирает исходный шрифт.
 * Перед первым запуском графики kernel сохраняет первые 16 строк каждого
 * из 256 символов (256 * 16 = 4096 байт) в обычный RAM. После возврата в
 * text mode этот массив записывается обратно в plane 2. Без этого аппаратный
 * text mode может быть включён, но символы будут выглядеть как артефакты.
 */
static uint8_t vga_saved_font[256u*16u];
static void vga_save_font(void){
    volatile uint8_t *v=(volatile uint8_t*)0x000a0000u;
    uint8_t seq1,seq2,seq4,gc4,gc5,gc6;
    uint32_t ch,row;
    __asm__ volatile("cli":::"memory");
    outb(VGA_SEQ,0x01);seq1=inb(VGA_SEQ_DATA);
    outb(VGA_SEQ_DATA,(uint8_t)(seq1|0x20u));
    outb(VGA_SEQ,0x02);seq2=inb(VGA_SEQ_DATA);
    outb(VGA_SEQ,0x04);seq4=inb(VGA_SEQ_DATA);
    outb(VGA_GC,0x04);gc4=inb(VGA_GC_DATA);
    outb(VGA_GC,0x05);gc5=inb(VGA_GC_DATA);
    outb(VGA_GC,0x06);gc6=inb(VGA_GC_DATA);

    /*
     * Читаем именно VGA font plane 2. Для этого обязательны три условия:
     *   SEQ04 = 07h — последовательная адресация без Chain-4 и odd/even;
     *   GC04  = 02h — чтение именно plane 2;
     *   GC05  = 00h и GC06 = 00h — обычный режим чтения из окна A0000.
     * В предыдущей версии GC04 не переключался на plane 2, поэтому в RAM
     * сохранялось не то содержимое, а восстановление давало нечитаемые
     * символы после возврата из Mode 13h.
     */
    outb(VGA_SEQ,0x02);outb(VGA_SEQ_DATA,0x04);
    outb(VGA_SEQ,0x04);outb(VGA_SEQ_DATA,0x07);
    outb(VGA_GC,0x04);outb(VGA_GC_DATA,0x02);
    outb(VGA_GC,0x05);outb(VGA_GC_DATA,0x00);
    outb(VGA_GC,0x06);outb(VGA_GC_DATA,0x00);
    for(ch=0;ch<256u;ch++){
        for(row=0;row<16u;row++)vga_saved_font[ch*16u+row]=v[ch*32u+row];
    }

    outb(VGA_SEQ,0x02);outb(VGA_SEQ_DATA,seq2);
    outb(VGA_SEQ,0x04);outb(VGA_SEQ_DATA,seq4);
    outb(VGA_GC,0x04);outb(VGA_GC_DATA,gc4);
    outb(VGA_GC,0x05);outb(VGA_GC_DATA,gc5);
    outb(VGA_GC,0x06);outb(VGA_GC_DATA,gc6);
    outb(VGA_SEQ,0x01);outb(VGA_SEQ_DATA,seq1);
}

static void vga_restore_saved_font(void){
    volatile uint8_t *v=(volatile uint8_t*)0x000a0000u;
    uint32_t ch,row;
    uint8_t seq1;
    __asm__ volatile("cli":::"memory");
    outb(VGA_SEQ,0x01);seq1=inb(VGA_SEQ_DATA);
    outb(VGA_SEQ_DATA,(uint8_t)(seq1|0x20u));

    /*
     * Для записи font plane 2 используем канонический VGA-режим загрузки
     * шрифта: CPU пишет только в map 2 (SEQ02=04), адресация последовательная
     * (SEQ04=07), чтение/модификация памяти отключена, окно — A0000.
     */
    outb(VGA_SEQ,0x02);outb(VGA_SEQ_DATA,0x04);
    outb(VGA_SEQ,0x04);outb(VGA_SEQ_DATA,0x07);
    outb(VGA_GC,0x04);outb(VGA_GC_DATA,0x02);
    outb(VGA_GC,0x05);outb(VGA_GC_DATA,0x00);
    outb(VGA_GC,0x06);outb(VGA_GC_DATA,0x00);
    for(ch=0;ch<256u;ch++){
        for(row=0;row<16u;row++)v[ch*32u+row]=vga_saved_font[ch*16u+row];
        /* Следующие 16 байт каждого 32-байтного слота не используются
         * 16-полосочным шрифтом, поэтому оставляем их неизменёнными. */
    }

    /*
     * После записи шрифта обязательно возвращаем именно текстовую
     * odd/even-адресацию. Оставленный после загрузки font SEQ04=07h запрещает
     * odd/even и превращает B8000 в неподходящее для текстового режима окно.
     * Поэтому здесь канонически восстанавливаем SEQ04=03h и GC05=10h/GC06=0Eh.
     */
    outb(VGA_SEQ,0x02);outb(VGA_SEQ_DATA,0x03);
    outb(VGA_SEQ,0x04);outb(VGA_SEQ_DATA,0x03);
    outb(VGA_GC,0x04);outb(VGA_GC_DATA,0x00);
    outb(VGA_GC,0x05);outb(VGA_GC_DATA,0x10);
    outb(VGA_GC,0x06);outb(VGA_GC_DATA,0x0e);
    outb(VGA_SEQ,0x01);outb(VGA_SEQ_DATA,(uint8_t)(seq1&~0x20u));
}

/*
 * Возврат VGA в текстовый режим 80x25.
 *
 * В предыдущих версиях здесь сохранялось произвольное текущее состояние VGA
 * и затем оно пыталось побитно восстанавливаться. Для обычного VGA/Bochs/QEMU
 * это оказалось ненадёжно: режим 13h меняет не только CRTC, но и состояние
 * sequencer, graphics controller, attribute controller и DAC, а порядок записи
 * этих регистров имеет значение.
 *
 * Поэтому для Toy OS используется детерминированный возврат в тот же цветной
 * VGA-режим 03h (80x25), в котором kernel запускает shell. Это ровно тот
 * режим, который нам нужен после VGADRV. Таблица регистров соответствует
 * стандартной VGA BIOS-конфигурации 80x25 text mode.
 */
/* Флаг временного user-доступа к VGA VRAM 0xA0000..0xAFFFF. */
static volatile uint32_t video_user_mapped;

static int vga_restore_text_mode_hw(void){
    static const uint8_t seq[5]={0x00,0x00,0x03,0x00,0x03};
    static const uint8_t crtc[25]={
        0x5f,0x4f,0x50,0x82,0x55,0x81,0xbf,0x1f,
        0x00,0x4f,0x0d,0x0e,0x00,0x00,0x00,0x00,
        0x9c,0x8e,0x8f,0x28,0x1f,0x96,0xb9,0xa3,0xff
    };
    static const uint8_t gc[9]={0x00,0x00,0x00,0x00,0x00,0x10,0x0e,0x0f,0xff};
    static const uint8_t attr[21]={
        0x00,0x01,0x02,0x03,0x04,0x05,0x14,0x07,
        0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
        0x0c,0x00,0x0f,0x08,0x00
    };
    static const uint8_t dac[48]={
        /* Стандартная 16-цветная VGA/EGA palette, RGB в диапазоне 0..63. */
        0,0,0,   0,0,42,   0,42,0,   0,42,42,
        42,0,0,  42,0,42,  42,21,0,  42,42,42,
        21,21,21,21,21,63, 21,63,21, 21,63,63,
        63,21,21,63,21,63, 63,63,21, 63,63,63
    };
    uint32_t i;
    uint8_t v;

    /*
     * Мы повторяем порядок стандартного VGA mode-set, а не просто меняем
     * регистры по одному. Сначала гасим дисплей и ставим Sequencer в reset,
     * затем настраиваем Misc/SEQ/CRTC/GC/AC, и только в самом конце включаем
     * видеовыход. Такой порядок исключает промежуточный режим, в котором
     * CRTC уже текстовый, а Sequencer/GC ещё графические.
     */
    __asm__ volatile("cli" ::: "memory");

    /* Гасим экран через Sequencer Clocking Mode (бит Screen Off). */
    outb(VGA_SEQ,0x01u);
    v=inb(VGA_SEQ_DATA);
    outb(VGA_SEQ_DATA,(uint8_t)(v|0x20u));

    /* Цветной VGA CRTC: 0x3D4/0x3D5. */
    outb(VGA_MISC_WRITE,0x67u);

    /* Синхронный reset Sequencer. */
    outb(VGA_SEQ,0x00u);
    outb(VGA_SEQ_DATA,0x01u);

    /* Все четыре рабочих регистра Sequencer. */
    for(i=1u;i<5u;i++){
        outb(VGA_SEQ,(uint8_t)i);
        outb(VGA_SEQ_DATA,seq[i]);
    }

    /* Возвращаем CRTC в стандартный цветной режим 80x25 и снимаем lock. */
    outb(VGA_CRTC,0x03u);
    v=inb(VGA_CRTC_DATA);
    outb(VGA_CRTC_DATA,(uint8_t)(v|0x80u));
    outb(VGA_CRTC,0x11u);
    v=inb(VGA_CRTC_DATA);
    outb(VGA_CRTC_DATA,(uint8_t)(v&0x7fu));
    for(i=0u;i<25u;i++){
        outb(VGA_CRTC,(uint8_t)i);
        outb(VGA_CRTC_DATA,crtc[i]);
    }

    /* Graphics Controller: текстовая odd/even адресация с B8000. */
    for(i=0u;i<9u;i++){
        outb(VGA_GC,(uint8_t)i);
        outb(VGA_GC_DATA,gc[i]);
    }

    /* Attribute Controller: каждый индекс начинается с чтения 3DA. */
    for(i=0u;i<21u;i++){
        (void)inb(VGA_STATUS1);
        outb(VGA_ATTR,(uint8_t)i);
        outb(VGA_ATTR,attr[i]);
    }

    /* PEL mask 0xFF и возврат контроллера атрибутов в display-enable state. */
    outb(0x3c6u,0xffu);
    (void)inb(VGA_STATUS1);
    outb(VGA_ATTR,0x20u);

    /* Восстанавливаем стандартные 16 цветов текста. */
    outb(VGA_DAC_WRITE,0x00u);
    for(i=0u;i<48u;i++)outb(VGA_DAC_DATA,dac[i]);

    /* Снимаем Sequencer reset и одновременно включаем экран. */
    outb(VGA_SEQ,0x00u);
    outb(VGA_SEQ_DATA,0x03u);
    outb(VGA_SEQ,0x01u);
    v=inb(VGA_SEQ_DATA);
    outb(VGA_SEQ_DATA,(uint8_t)(v&~0x20u));

    return 1;
}

/* Scroll the text console by one row instead of wrapping back to row 0.
   Wrapping was the reason command output eventually appeared over old text:
   after Enter at the bottom of the screen the old implementation reset y=0. */
static void console_scroll(void){uint32_t x,y;for(y=1;y<H;y++){for(x=0;x<W;x++)VGA[(y-1u)*W+x]=VGA[y*W+x];}for(x=0;x<W;x++)VGA[(H-1u)*W+x]=((uint16_t)ATTR<<8)|' ';}
static void console_newline(void){vga_x=0;if(vga_y+1u>=H){console_scroll();vga_y=H-1u;}else{vga_y++;}vga_cursor_update();}
static void console_put(char c){if(c=='\n'){console_newline();return;}if(c=='\r'){vga_x=0;vga_cursor_update();return;}if(c=='\b'){if(vga_x)vga_x--;vga_cell(vga_x,vga_y,' ');vga_cursor_update();return;}vga_cell(vga_x,vga_y,c);if(++vga_x>=W){vga_x=0;if(vga_y+1u>=H){console_scroll();vga_y=H-1u;}else{vga_y++;}}vga_cursor_update();}
static void console_write_n(const char*s,uint32_t n){while(n--)console_put(*s++);}
/* A Ring-3 SYS_CONSOLE_WRITE is one logical console transaction.  PIT can
   preempt Ring-3 while this syscall is printing, so leaving IF enabled here
   allows sched_irq_tick() to insert another task's diagnostic/output between
   characters.  Preserve the incoming IF state and keep the whole write
   non-preemptible. */
static uint32_t console_write_atomic_n(const char*s,uint32_t n){
    uint32_t flags;
    __asm__ volatile("pushfl; popl %0; cli" : "=r"(flags) : : "memory");
    console_write_n(s,n);
    if(flags&0x200u)__asm__ volatile("sti" ::: "memory");
    return n;
}
static void console_write(const char*s){console_write_n(s,str_len(s));}
static void u32_dec(uint32_t v,char*b){static const uint32_t p[10]={1000000000u,100000000u,10000000u,1000000u,100000u,10000u,1000u,100u,10u,1u};uint32_t i,d,st=0;for(i=0;i<10;i++){d=0;while(v>=p[i]){v-=p[i];d++;}if(d||st||i==9){*b++=(char)('0'+d);st=1;}}*b=0;}
static void console_write_u32(uint32_t v){char b[11];u32_dec(v,b);console_write(b);}
static void console_put_attr(char c,uint8_t attr){if(vga_x<W&&vga_y<H)VGA[vga_y*W+vga_x]=((uint16_t)attr<<8)|(uint8_t)c;if(++vga_x>=W){vga_x=0;if(vga_y+1u>=H){console_scroll();vga_y=H-1u;}else vga_y++;}vga_cursor_update();}
static void console_write_color(const char*s,uint8_t attr){while(*s)console_put_attr(*s++,attr);}
/* v66 UDP telemetry: запись строки в фиксированную позицию VGA без изменения
   текущего курсора shell и без прокрутки экрана. Это нужно мастеру телеметрии:
   каждое новое значение полностью перезаписывает свой экранный слот. */
static uint32_t console_write_at(uint32_t row,uint32_t col,const char*s,uint32_t n){
    uint32_t i;
    if(row>=H||col>=W||n>W-col||n==0u)return 0xffffffffu;
    for(i=0;i<n;i++)VGA[row*W+col+i]=((uint16_t)ATTR<<8)|(uint8_t)s[i];
    return n;
}

struct idt_gate{uint16_t off_lo,sel;uint8_t zero,type;uint16_t off_hi;} __attribute__((packed));
static struct idt_gate idt[256];
struct idtr{uint16_t limit;uint32_t base;} __attribute__((packed));
struct gdtr{uint16_t limit;uint32_t base;} __attribute__((packed));
struct gdt_desc{uint16_t limit_lo,base_lo;uint8_t base_mid,access,gran,base_hi;} __attribute__((packed));
struct tss32{uint16_t link,res0;uint32_t esp0;uint16_t ss0,res1;uint32_t esp1;uint16_t ss1,res2;uint32_t esp2;uint16_t ss2,res3;uint32_t cr3,eip,eflags,eax,ecx,edx,ebx,esp,ebp,esi,edi;uint16_t es,res4,cs,res5,ss,res6,ds,res7,fs,res8,gs,res9,ldt,res10;uint16_t trap,iobase;} __attribute__((packed));
static struct gdt_desc gdt[6];
static struct tss32 tss;

/* Page directory and the first 4-MiB page table are used to make the
   CPL3 shell a real paged user area instead of relying only on segmentation. */
static uint32_t page_directory[1024] __attribute__((aligned(4096)));
static uint32_t page_table0[1024] __attribute__((aligned(4096)));
/* v35: A and B have separate CR3 roots and page tables.  The user virtual
   layout is intentionally identical; only the physical stack backing differs. */
static uint32_t mt_page_directory[SCHED_TASKS][1024] __attribute__((aligned(4096)));
static uint32_t mt_page_table[SCHED_TASKS][1024] __attribute__((aligned(4096)));
static uint32_t rt_page_directory[RT_MAX_TASKS][1024] __attribute__((aligned(4096)));
static uint32_t rt_page_table[RT_MAX_TASKS][1024] __attribute__((aligned(4096)));
extern char __user_text_start[];
extern char __user_text_end[];
extern char __user_rodata_start[];
extern char __user_rodata_end[];
extern char __user_data_start[];
extern char __user_data_end[];
extern void mt_task_a(void);
extern void mt_task_b(void);
extern void mt_task_c(void);
extern void mt_task_d(void);
#define PAGE_P 0x001u
#define PAGE_RW 0x002u
#define PAGE_US 0x004u
#define USER_STACK_PAGE 0x003ff000u
#define USER_STACK_TOP  0x003fffe0u
#define KERNEL_STACK_TOP 0x001f0000u
#define EXEC_LOAD_ADDR   0x00100000u
#define EXEC_MAX_SIZE    0x000e0000u
#define EXEC_STACK_PAGE  0x003fa000u
#define EXEC_STACK_TOP   0x003fc000u
#define EXEC_STACK_SIZE  0x00002000u
#define EXEC_FRAME_WORDS 19u
#define MT_STACK_PAGE    0x003fd000u
#define MT_STACK_TOP     0x003fdfe0u
#define MT_STACK_BASE_PHYS 0x003fd000u
/* v34/v38 compatibility names retained for the historical two/four-task tests. */
#define MT_STACK_A_PHYS MT_STACK_BASE_PHYS
#define MT_STACK_B_PHYS (MT_STACK_BASE_PHYS-0x1000u)
#define MT_STACK_C_PHYS (MT_STACK_BASE_PHYS-0x2000u)
#define MT_STACK_D_PHYS (MT_STACK_BASE_PHYS-0x3000u)
#define EXECMT_IMAGE_BASE_PHYS 0x00400000u
#define EXECMT_IMAGE_SLOT_SIZE 0x00100000u
#define RT_IMAGE_BASE_PHYS (EXECMT_IMAGE_BASE_PHYS+(SCHED_TASKS*EXECMT_IMAGE_SLOT_SIZE))
#define RT_STACK_BASE_PHYS 0x003e0000u
#define MT_CONTEXT_WORDS 19u
#define EXE_MAGIC        0x31455845u /* "EXE1" little-endian */
static void gdt_set(struct gdt_desc*d,uint32_t base,uint32_t limit,uint8_t access,uint8_t gran){d->limit_lo=(uint16_t)(limit&0xffffu);d->base_lo=(uint16_t)base;d->base_mid=(uint8_t)(base>>16);d->access=access;d->gran=(uint8_t)((limit>>16)&0x0f)|(uint8_t)(gran&0xf0);d->base_hi=(uint8_t)(base>>24);}
static void gdt_init_ring3(void){
    struct gdtr r;
    uint32_t base=(uint32_t)&tss;
    mem_set(gdt,0,sizeof(gdt));
    gdt_set(&gdt[1],0,0x3ffff,0x9a,0xc0);
    gdt_set(&gdt[2],0,0x3ffff,0x92,0xc0);
    gdt_set(&gdt[3],0,0x3ffff,0xfa,0xc0);
    gdt_set(&gdt[4],0,0x3ffff,0xf2,0xc0);
    gdt_set(&gdt[5],base,(uint32_t)sizeof(struct tss32)-1u,0x89,0x00);
    tss.esp0=KERNEL_STACK_TOP;
    tss.ss0=0x10u;
    tss.iobase=(uint16_t)sizeof(struct tss32);
    r.limit=(uint16_t)(sizeof(gdt)-1u); r.base=(uint32_t)gdt;
    __asm__ volatile("lgdtl %0"::"m"(r):"memory");
    __asm__ volatile("movw $0x10,%%ax;movw %%ax,%%ds;movw %%ax,%%es;movw %%ax,%%ss;movw %%ax,%%fs;movw %%ax,%%gs;ljmp $0x08,$1f;1:":: :"ax","memory");
    __asm__ volatile("ltr %w0"::"a"((uint16_t)0x28):"memory");
}
static void paging_mark_user_ro_table(uint32_t*pt,uint32_t start,uint32_t end){
    uint32_t p;
    start&=~0xfffu;
    end=(end+0xfffu)&~0xfffu;
    for(p=start;p<end&&p<0x00400000u;p+=0x1000u){pt[p>>12]|=PAGE_US;pt[p>>12]&=~PAGE_RW;}
}
static void paging_mark_user_ro(uint32_t start,uint32_t end){paging_mark_user_ro_table(page_table0,start,end);}
static void paging_mark_user_rw(uint32_t start,uint32_t end){
    uint32_t p;
    start&=~0xfffu;
    end=(end+0xfffu)&~0xfffu;
    for(p=start;p<end&&p<0x00400000u;p+=0x1000u)
        page_table0[p>>12]|=PAGE_P|PAGE_RW|PAGE_US;
}
static uint32_t sched_stack_phys(uint32_t id){
    return id<SCHED_TASKS ? MT_STACK_BASE_PHYS-(id*0x1000u) : 0u;
}
static uint32_t sched_image_phys(uint32_t id){
    return id<SCHED_TASKS ? EXECMT_IMAGE_BASE_PHYS+(id*EXECMT_IMAGE_SLOT_SIZE) : 0u;
}
static void paging_build_mt(void){
    uint32_t t,i,p;
    for(t=0;t<SCHED_TASKS;t++){
        mem_set(mt_page_directory[t],0,sizeof(mt_page_directory[t]));
        mem_set(mt_page_table[t],0,sizeof(mt_page_table[t]));
        for(i=0;i<1024u;i++)mt_page_table[t][i]=(i<<12)|PAGE_P|PAGE_RW;
        mt_page_directory[t][0]=(uint32_t)mt_page_table[t]|PAGE_P|PAGE_RW|PAGE_US;
        paging_mark_user_ro_table(mt_page_table[t],(uint32_t)__user_text_start,(uint32_t)__user_text_end);
        paging_mark_user_ro_table(mt_page_table[t],(uint32_t)__user_rodata_start,(uint32_t)__user_rodata_end);
        /* Every EXECMT task gets a private 1-MiB physical image slot.  The
           virtual address stays 0x00100000 so existing EXE1 binaries remain
           position-fixed and can execute unchanged under different CR3 roots.
           Initially the image is supervisor-only; the actual image range is
           granted US=1 after it has been validated and loaded. */
        for(p=0;p<EXEC_MAX_SIZE;p+=0x1000u)
            mt_page_table[t][(EXEC_LOAD_ADDR+p)>>12]=(sched_image_phys(t)+p)|PAGE_P|PAGE_RW;
        /* v38 compatibility: the first four slots still run the built-in
           scheduler regression functions from the kernel's user-text image.
           EXECMT-created slots replace this mapping with their private EXE1
           image when loaded. */
        if(t<4u){
            for(p=0;p<EXEC_MAX_SIZE;p+=0x1000u)
                mt_page_table[t][(EXEC_LOAD_ADDR+p)>>12]=(EXEC_LOAD_ADDR+p)|PAGE_P|PAGE_RW;
            paging_mark_user_ro_table(mt_page_table[t],(uint32_t)__user_text_start,(uint32_t)__user_text_end);
            paging_mark_user_ro_table(mt_page_table[t],(uint32_t)__user_rodata_start,(uint32_t)__user_rodata_end);
        }
        /* All tasks share the same virtual stack address, but each CR3 maps it
           to a different physical page. */
        mt_page_table[t][MT_STACK_PAGE>>12]=sched_stack_phys(t)|PAGE_P|PAGE_RW|PAGE_US;
    }
}
static void paging_set_task_image_user(uint32_t id,uint32_t start,uint32_t end){
    uint32_t p;
    if(id>=SCHED_TASKS)return;
    start&=~0xfffu;end=(end+0xfffu)&~0xfffu;
    if(end>EXEC_LOAD_ADDR+EXEC_MAX_SIZE)end=EXEC_LOAD_ADDR+EXEC_MAX_SIZE;
    for(p=start;p<end;p+=0x1000u)mt_page_table[id][p>>12]|=PAGE_US|PAGE_RW;
}
static void paging_prepare_execmt_task(uint32_t id){
    uint32_t p;
    if(id>=SCHED_TASKS)return;
    for(p=0;p<EXEC_MAX_SIZE;p+=0x1000u)
        mt_page_table[id][(EXEC_LOAD_ADDR+p)>>12]=(sched_image_phys(id)+p)|PAGE_P|PAGE_RW;
}
static uint32_t rt_stack_phys(uint32_t id){return id<RT_MAX_TASKS ? RT_STACK_BASE_PHYS-(id*0x1000u) : 0u;}
static uint32_t rt_image_phys(uint32_t id){return id<RT_MAX_TASKS ? RT_IMAGE_BASE_PHYS+(id*EXECMT_IMAGE_SLOT_SIZE) : 0u;}
static void paging_build_rt(void){
    uint32_t t,i,p;
    for(t=0;t<RT_MAX_TASKS;t++){
        mem_set(rt_page_directory[t],0,sizeof(rt_page_directory[t]));
        mem_set(rt_page_table[t],0,sizeof(rt_page_table[t]));
        for(i=0;i<1024u;i++)rt_page_table[t][i]=(i<<12)|PAGE_P|PAGE_RW;
        rt_page_directory[t][0]=(uint32_t)rt_page_table[t]|PAGE_P|PAGE_RW|PAGE_US;
        paging_mark_user_ro_table(rt_page_table[t],(uint32_t)__user_text_start,(uint32_t)__user_text_end);
        paging_mark_user_ro_table(rt_page_table[t],(uint32_t)__user_rodata_start,(uint32_t)__user_rodata_end);
        for(p=0;p<EXEC_MAX_SIZE;p+=0x1000u)rt_page_table[t][(EXEC_LOAD_ADDR+p)>>12]=(rt_image_phys(t)+p)|PAGE_P|PAGE_RW;
        rt_page_table[t][MT_STACK_PAGE>>12]=rt_stack_phys(t)|PAGE_P|PAGE_RW|PAGE_US;
    }
}
static void paging_prepare_rt_task(uint32_t id){uint32_t p;if(id>=RT_MAX_TASKS)return;for(p=0;p<EXEC_MAX_SIZE;p+=0x1000u)rt_page_table[id][(EXEC_LOAD_ADDR+p)>>12]=(rt_image_phys(id)+p)|PAGE_P|PAGE_RW;}
static void paging_set_rt_image_user(uint32_t id,uint32_t start,uint32_t end){uint32_t p;if(id>=RT_MAX_TASKS)return;start&=~0xfffu;end=(end+0xfffu)&~0xfffu;if(end>EXEC_LOAD_ADDR+EXEC_MAX_SIZE)end=EXEC_LOAD_ADDR+EXEC_MAX_SIZE;for(p=start;p<end;p+=0x1000u)rt_page_table[id][p>>12]|=PAGE_US|PAGE_RW;}
static void paging_init(void){
    uint32_t i;
    mem_set(page_directory,0,sizeof(page_directory));
    mem_set(page_table0,0,sizeof(page_table0));
    for(i=0;i<1024u;i++)page_table0[i]=(i<<12)|PAGE_P|PAGE_RW;
    page_directory[0]=(uint32_t)page_table0|PAGE_P|PAGE_RW|PAGE_US;
    paging_mark_user_ro((uint32_t)__user_text_start,(uint32_t)__user_text_end);
    paging_mark_user_ro((uint32_t)__user_rodata_start,(uint32_t)__user_rodata_end);
    paging_mark_user_rw((uint32_t)__user_data_start,(uint32_t)__user_data_end);
    page_table0[USER_STACK_PAGE>>12]=USER_STACK_PAGE|PAGE_P|PAGE_RW|PAGE_US;
    paging_build_mt();
    paging_build_rt();
    __asm__ volatile("movl %0,%%cr3"::"r"((uint32_t)page_directory):"memory");
    __asm__ volatile("movl %%cr0,%%eax; orl $0x80000000,%%eax; movl %%eax,%%cr0":: :"eax","memory");
}
struct exe_header { uint32_t magic, entry, image_size, bss_size; } __attribute__((packed));
static uint32_t exe_active;
static uint32_t exe_end;
/* FIX33: observational lifecycle identity.  PID is identity, never a scheduler slot. */
static uint32_t process_next_pid=1u,exe_pid,exe_proc_state;
static char exe_proc_name[QUEUE_NAME_SIZE];
/* FIX35: termination diagnostics are metadata only; scheduler policy never reads them. */
#define PROCESS_EXIT_NONE   0u
#define PROCESS_EXIT_NORMAL 1u
#define PROCESS_EXIT_FAULT  2u
#define PROCESS_EXIT_WATCHDOG 3u
#define PROCESS_EXIT_STOPPED 4u /* FIX54: explicit administrative MT stop */
static uint32_t exe_exit_reason,exe_exit_status,exe_fault_vector,exe_fault_error,exe_fault_eip,exe_fault_cr2;
/* FIX36: termination results outlive RT/MT slot reuse.  This is metadata only:
   it is never consulted by the scheduler.  Oldest entries are overwritten. */
#define PROCESS_RESULT_MAX 64u
struct process_result { uint32_t pid,type,reason,status,vector,error,eip,cr2; };
static struct process_result process_results[PROCESS_RESULT_MAX];
static uint32_t process_result_pos;
#define PROCESS_EVENT_MAX 64u
static volatile uint32_t timer_ticks; /* PIT monotonic clock; definition below */
struct process_event { uint32_t seq,tick,pid,type,reason,status,vector,error; };
static struct process_event process_events[PROCESS_EVENT_MAX];
static uint32_t process_event_pos;
static uint32_t process_event_seq;
/* FIX44: bounded in-kernel operational safety state. Policy enforcement is deliberately deferred. */
static uint32_t safe_mode=0u; /* 0 NORMAL, 1 DEGRADED, 2 SAFE */
/* FIX48: SAFE is admission control, not scheduler priority. Existing tasks are not
   killed implicitly. New detached SPAWN/EXECMT/RT starts are denied in SAFE. */
static uint32_t safe_deny_spawn=0u,safe_deny_mt=0u,safe_deny_rt=0u;
/* FIX46: backend=1 is an emulated control-plane backend for QEMU/regression.
   It never resets hardware and PIT never feeds it. A future board backend must
   implement the physical arm/feed/disarm operations behind this ABI. */
static uint32_t hwwd_backend=1u,hwwd_armed=0u,hwwd_timeout=0u,hwwd_last_feed=0u,hwwd_feed_seq=0u;
static uint32_t safe_reason=0u;
static uint32_t safe_seq=0u;
static uint32_t safe_tick=0u;
/* FIX51 Recovery Manager: bounded policy plus observable managed-restart lifecycle.
   Supervisor remains the executor; kernel owns authoritative lifecycle state only. */
static uint32_t recovery_attempts=0u,recovery_last_pid=0u,recovery_last_kind=0u,recovery_last_action=0u,recovery_seq=0u;
static uint32_t recovery_state=0u,recovery_old_pid=0u,recovery_new_pid=0u,recovery_result=0u,recovery_detail=0u,recovery_tick=0u;
#define RECOVERY_STATE_IDLE 0u
#define RECOVERY_STATE_DETECTED 1u
#define RECOVERY_STATE_STOPPED 2u
#define RECOVERY_STATE_VERIFYING 3u
#define RECOVERY_STATE_RECOVERED 4u
#define RECOVERY_STATE_FAILED 5u
#define RECOVERY_STATE_SAFE 6u
#define RECOVERY_RESULT_NONE 0u
#define RECOVERY_RESULT_SUCCESS 1u
#define RECOVERY_RESULT_STOP_FAILED 2u
#define RECOVERY_RESULT_SPAWN_FAILED 3u
#define RECOVERY_RESULT_VERIFY_TIMEOUT 4u
#define RECOVERY_RESULT_PROCESS_FAILED 5u
#define RECOVERY_KIND_FAULT 1u
#define RECOVERY_KIND_WATCHDOG 2u
#define RECOVERY_ACTION_RESTART 1u
#define RECOVERY_ACTION_SAFE 2u
#define RECOVERY_RESTART_LIMIT 3u
static void process_event_store(uint32_t pid,uint32_t type,uint32_t reason,uint32_t status,uint32_t vector,uint32_t error){
    struct process_event*e=&process_events[process_event_pos%PROCESS_EVENT_MAX];
    process_event_pos++;process_event_seq++;e->seq=process_event_seq;e->tick=timer_ticks;e->pid=pid;e->type=type;e->reason=reason;e->status=status;e->vector=vector;e->error=error;
}
/* FIX40: bounded heartbeat registry.  Entries are keyed by PID; no scheduler
   policy depends on this table.  timer_ticks is the legacy 50-Hz monotonic clock. */
#define PROCESS_HEARTBEAT_SLOTS 64u
struct process_heartbeat {uint32_t pid,last_tick,seq;};
static struct process_heartbeat process_heartbeats[PROCESS_HEARTBEAT_SLOTS];
static struct process_heartbeat* process_heartbeat_find(uint32_t pid){uint32_t i;for(i=0;i<PROCESS_HEARTBEAT_SLOTS;i++)if(process_heartbeats[i].pid==pid)return &process_heartbeats[i];return 0;}
static struct process_heartbeat* process_heartbeat_get(uint32_t pid){uint32_t i;struct process_heartbeat*h=process_heartbeat_find(pid);if(h)return h;for(i=0;i<PROCESS_HEARTBEAT_SLOTS;i++)if(!process_heartbeats[i].pid){process_heartbeats[i].pid=pid;process_heartbeats[i].last_tick=0;process_heartbeats[i].seq=0;return &process_heartbeats[i];}i=pid%PROCESS_HEARTBEAT_SLOTS;process_heartbeats[i].pid=pid;process_heartbeats[i].last_tick=0;process_heartbeats[i].seq=0;return &process_heartbeats[i];}
static void process_heartbeat_clear(uint32_t pid){struct process_heartbeat*h=process_heartbeat_find(pid);if(h){h->pid=0;h->last_tick=0;h->seq=0;}}

static void process_result_store(uint32_t pid,uint32_t type,uint32_t reason,uint32_t status,uint32_t vector,uint32_t error,uint32_t eip,uint32_t cr2){
    struct process_result*r;if(!pid||reason==PROCESS_EXIT_NONE)return;process_heartbeat_clear(pid);
    r=&process_results[process_result_pos%PROCESS_RESULT_MAX];process_result_pos++;
    r->pid=pid;r->type=type;r->reason=reason;r->status=status;r->vector=vector;r->error=error;r->eip=eip;r->cr2=cr2;
    process_event_store(pid,type,reason,status,vector,error);
}
static struct process_result*process_result_find(uint32_t pid){uint32_t i,n=process_result_pos<PROCESS_RESULT_MAX?process_result_pos:PROCESS_RESULT_MAX;for(i=0u;i<n;i++){uint32_t pos=(process_result_pos-1u-i)%PROCESS_RESULT_MAX;if(process_results[pos].pid==pid)return &process_results[pos];}return 0;}
static uint32_t process_alloc_pid(void){uint32_t p=process_next_pid++;if(p==0u){p=process_next_pid++;}if(process_next_pid==0u)process_next_pid=1u;return p;}
static uint32_t saved_user_frame[EXEC_FRAME_WORDS];
/* Аргументы текущего EXE. Они копируются в память ядра до передачи управления
   Ring-3, чтобы указатели shell/queue не зависели от времени жизни старого
   пользовательского адресного пространства. */
static char exe_args[EXEC_ARG_COUNT][EXEC_ARG_SIZE];
/* Sequential EXE1 scheduler. One EXE occupies the existing 1-MiB process
   slot at a time; SYS_EXIT advances the FIFO ring to the next entry.
   Nested queues are kept on a small kernel-side stack. A queue started by an
   EXE becomes the child of the currently active queue; when the child ends,
   the parent queue is restored and continues with its next member. */
#define QUEUE_MAX_DEPTH 4u
struct queue_state {
    char names[QUEUE_MAX_FILES][QUEUE_NAME_SIZE];
    char args[QUEUE_MAX_FILES][EXEC_ARG_COUNT][EXEC_ARG_SIZE];
    uint32_t count,index,rounds,repetitions,forever;
};
static char queue_names[QUEUE_MAX_FILES][QUEUE_NAME_SIZE];
static char queue_args[QUEUE_MAX_FILES][EXEC_ARG_COUNT][EXEC_ARG_SIZE];
static uint32_t queue_count,queue_index,queue_rounds,queue_repetitions,queue_forever;
static struct queue_state queue_stack[QUEUE_MAX_DEPTH];
static uint32_t queue_depth;
static volatile uint32_t queue_active,queue_stop_requested;

/* v37: small preemptive scheduler.  Each task owns a complete 19-word
   architectural context plus its own CR3.  The scheduler never embeds CR3 in
   the interrupt frame: it is task metadata, just like state and task id. */
#define TASK_CREATE  0u
#define TASK_READY   1u
#define TASK_RUNNING 2u
#define TASK_BLOCKED 3u
#define TASK_STOPPED 4u
#define TASK_EXIT    5u
struct sched_task {
    uint32_t words[MT_CONTEXT_WORDS];
    uint32_t cr3;
    uint32_t state;
    uint32_t id;
    uint32_t switches;
    uint32_t image_end;
    char name[QUEUE_NAME_SIZE];
    uint32_t announced;
    /* FIX33: observational process identity; scheduler policy never reads this field. */
    uint32_t process_pid;
    /* FIX35: retained diagnostic reason for the last process instance in this slot. */
    uint32_t exit_reason,exit_status,fault_vector,fault_error,fault_eip,fault_cr2;
    /* RT metadata.  Stage 1 stores the requested policy without changing the
       existing EXECMT scheduler semantics. */
    uint32_t rt_period_ms;
    uint32_t rt_deadline_ms;
    uint32_t rt_priority;
    uint32_t rt_period_ticks;
    uint32_t rt_next_release_tick;
    uint32_t rt_deadline_tick;
    uint32_t rt_job_release_tick;
    uint32_t rt_job_active;
    uint32_t rt_job_missed;
    uint32_t rt_missed_deadlines;
    uint32_t rt_runtime_ticks;
    uint32_t rt_runtime_budget;
    uint32_t rt_dispatch_seq;
    uint32_t rt_job_sequence;
    uint32_t rt_skipped_releases;
    /* Stage 6.2: measured release/dispatch timing.  All values are RT ticks;
       one RT tick is 10 ms.  These fields are diagnostic only and do not
       alter release, deadline or scheduler policy. */
    uint32_t rt_release_observed_tick;
    uint32_t rt_release_last_tick;
    uint32_t rt_release_interval_ticks;
    uint32_t rt_release_min_interval;
    uint32_t rt_release_max_interval;
    uint32_t rt_release_samples;
    uint32_t rt_release_late_ticks;
    uint32_t rt_release_max_late;
    uint32_t rt_job_dispatch_tick;
    uint32_t rt_job_dispatch_latency;
    uint32_t rt_job_dispatched;
    /* Stage 6.2 diagnostic: cumulative CPU-running ticks consumed by the
       current RT job. This is separate from rt_runtime_ticks, which is the
       per-budget accounting counter used by the scheduler. */
    uint32_t rt_job_cpu_ticks;
};
#define RT_STATS_BUCKET_TICKS 30000u /* 5 minutes at the 100 Hz RT clock */
#define RT_STATS_BUCKETS 72u        /* one rolling 6-hour window */
#define RT_ANOMALY_EVENTS 16u
struct rt_stat_bucket {
    uint32_t start_tick,jobs,interval_sum,interval_min,interval_max;
    int32_t jitter_sum,jitter_min,jitter_max;
    uint32_t dispatch_sum,dispatch_min,dispatch_max,cpu_sum,cpu_min,cpu_max;
    uint32_t wall_sum,wall_min,wall_max,misses,skips;
};
struct rt_monitor_stat {
    uint32_t last_seq,last_interval;
    int32_t last_jitter;
    uint32_t last_dispatch,last_cpu,last_wall,last_miss,last_skips,ring_pos,ring_count,diag_verbose;
    uint32_t lifetime_misses,lifetime_skips;
    uint32_t event_pos,event_count;
    uint32_t ring[8u][8u];
    uint32_t events[RT_ANOMALY_EVENTS][5u]; /* tick,seq,miss,skip_delta,lateness */
    struct rt_stat_bucket buckets[RT_STATS_BUCKETS];
};
static struct rt_monitor_stat rt_mon[RT_MAX_TASKS];
static uint32_t rt_sat_add(uint32_t a,uint32_t b){return (0xffffffffu-a<b)?0xffffffffu:a+b;}
/* FIX12: application data published by detached RT tasks.  The kernel treats
   the payload as opaque bytes. seq is deliberately uint32_t and may wrap;
   consumers detect a new snapshot by inequality, never by ordering. */
struct rt_data_snapshot {
    char data[RT_DATA_BUF_SIZE];
    uint32_t len;
    uint32_t seq;
    uint32_t valid;
};
static struct rt_data_snapshot rt_data[RT_MAX_TASKS];
/* FIX15: per-position snapshots for one EXECMT session.  Unlike RTDATA, the
   last MT snapshot survives task EXIT/MTSTOP MTn and is cleared only when the
   whole session is closed with MTSTOP ALL (or before a future new session).
   session and seq are change markers; consumers compare by !=, not ordering. */
struct mt_data_snapshot {
    char data[MT_DATA_BUF_SIZE];
    uint32_t len;
    uint32_t seq;
    uint32_t valid;
};
struct mt_data_request {
    uint32_t op;
    uint32_t mt_id;
    uint32_t buffer;
    uint32_t length;
    uint32_t seq;
    uint32_t session;
};
static struct mt_data_snapshot mt_data[SCHED_TASKS];
static uint32_t mt_session_id;
static uint32_t mt_session_count;
static volatile uint32_t mt_session_active;
static void mt_data_clear_all(void){
    uint32_t i;
    for(i=0u;i<SCHED_TASKS;i++){
        mt_data[i].data[0]=0;mt_data[i].len=0u;mt_data[i].seq=0u;mt_data[i].valid=0u;
    }
}
static void rt_data_clear(uint32_t slot){
    if(slot>=RT_MAX_TASKS)return;
    rt_data[slot].data[0]=0;
    rt_data[slot].len=0u;
    rt_data[slot].seq=0u;
    rt_data[slot].valid=0u;
}
static struct sched_task sched_tasks[SCHED_TASKS];
static struct sched_task rt_tasks[RT_MAX_TASKS];
static uint32_t process_pid_known(uint32_t pid){uint32_t i;if(!pid)return 0u;if(exe_active&&exe_pid==pid)return 1u;for(i=0u;i<SCHED_TASKS;i++)if(sched_tasks[i].process_pid==pid&&(sched_tasks[i].state==TASK_READY||sched_tasks[i].state==TASK_RUNNING||sched_tasks[i].state==TASK_BLOCKED))return 1u;for(i=0u;i<RT_MAX_TASKS;i++)if(rt_tasks[i].process_pid==pid&&(rt_tasks[i].state==TASK_READY||rt_tasks[i].state==TASK_RUNNING||rt_tasks[i].state==TASK_BLOCKED))return 1u;return 0u;}
static uint32_t mt_quantum_used;
static uint32_t mt_shell_turn;
static uint32_t sched_current;
static volatile uint32_t sched_active;
static uint32_t sched_saved_shell[MT_CONTEXT_WORDS];
static uint32_t sched_saved_shell_cr3;
static uint32_t sched_switches;
/* FIX22: MT scheduler diagnostics.  These counters are observational only:
   they never participate in task selection, priorities or quantum policy. */
static uint32_t mt_stat_dispatch[SCHED_TASKS];
static uint32_t mt_stat_quanta[SCHED_TASKS];
static uint32_t mt_stat_cpu_ticks[SCHED_TASKS];
static uint32_t mt_stat_partial_ticks[SCHED_TASKS];
static void mt_stats_reset_all(void){
    uint32_t i;
    for(i=0u;i<SCHED_TASKS;i++){mt_stat_dispatch[i]=0u;mt_stat_quanta[i]=0u;mt_stat_cpu_ticks[i]=0u;mt_stat_partial_ticks[i]=0u;}
}
static void mt_stats_dispatch(uint32_t id){if(id<SCHED_TASKS)mt_stat_dispatch[id]++;}
static void mt_stats_cpu_tick(uint32_t id){
    if(id>=SCHED_TASKS)return;
    mt_stat_cpu_ticks[id]++;
    mt_stat_partial_ticks[id]++;
    if(mt_stat_partial_ticks[id]>=MT_QUANTUM_TICKS){mt_stat_partial_ticks[id]=0u;mt_stat_quanta[id]++;}
}
/* FIX23: classify an MT caller by the address space that is actually
   executing, not merely by sched_active.  Foreground EXE/RTD shares the shell
   address space while an EXECMT session is active and must never be mistaken
   for sched_current. */
/* FIX34: one authoritative resolver for the Ring-3 execution identity.
   It observes the already-established foreground/MT/RT ownership rules; it
   does not select tasks and is not part of scheduler policy. */
#define PROCESS_TYPE_NONE 0u
#define PROCESS_TYPE_FG   1u
#define PROCESS_TYPE_MT   2u
#define PROCESS_TYPE_RT   3u
struct process_identity { uint32_t type,slot,pid,cr3; };
static struct process_identity process_current_identity(void){
    struct process_identity r;uint32_t cr3=read_cr3(),i;
    r.type=PROCESS_TYPE_NONE;r.slot=0xffffffffu;r.pid=0u;r.cr3=cr3;
    /* FIX34A: identity follows the address space that is ACTUALLY loaded in
       CR3, not only the scheduler state flag.  The timer path may change an
       executing RT/MT slot from RUNNING to READY before rt_switch_to() has
       replaced the interrupted Ring-3 frame/CR3.  Requiring RUNNING here made
       that still-current RT frame look like shell and overwrote
       rt_shell_frame, hanging the console.  Keep the proven FIX33 ownership
       semantics: an active RT slot with matching private CR3 is the caller. */
    for(i=0u;i<RT_MAX_TASKS;i++)if((rt_tasks[i].state==TASK_READY||rt_tasks[i].state==TASK_RUNNING||rt_tasks[i].state==TASK_BLOCKED)&&rt_tasks[i].rt_period_ms!=0u&&rt_tasks[i].process_pid&&rt_tasks[i].cr3==cr3){r.type=PROCESS_TYPE_RT;r.slot=i;r.pid=rt_tasks[i].process_pid;return r;}
    /* The same rule applies to MT hand-off windows: CR3 is authoritative for
       the context that is still executing; state may already be READY. */
    if(sched_active&&sched_current<SCHED_TASKS&&sched_tasks[sched_current].state!=TASK_STOPPED&&sched_tasks[sched_current].state!=TASK_EXIT&&sched_tasks[sched_current].process_pid&&sched_tasks[sched_current].cr3==cr3){r.type=PROCESS_TYPE_MT;r.slot=sched_current;r.pid=sched_tasks[sched_current].process_pid;return r;}
    if(exe_active&&exe_pid&&cr3==(uint32_t)page_directory){r.type=PROCESS_TYPE_FG;r.pid=exe_pid;return r;}
    return r;
}
static int mt_current_running(void){return process_current_identity().type==PROCESS_TYPE_MT;}
/* FIX60ZEG: background console output must not leave the interactive shell
   visually stranded after a transient MT/RT process exits.  This is only
   redraw bookkeeping: it never selects a task or changes scheduler ownership.
   A per-slot dirty bit is set when a detached process writes to the console;
   normal/forced termination converts that bit into one pending shell redraw. */
static volatile uint32_t console_redraw_pending;
static uint32_t console_mt_dirty_mask;
static uint32_t console_rt_dirty_mask;
static void console_background_write_note(void){
    struct process_identity id=process_current_identity();
    if(id.type==PROCESS_TYPE_MT&&id.slot<SCHED_TASKS)console_mt_dirty_mask|=(1u<<id.slot);
    else if(id.type==PROCESS_TYPE_RT&&id.slot<RT_MAX_TASKS)console_rt_dirty_mask|=(1u<<id.slot);
}
static void console_background_done(uint32_t type,uint32_t slot){
    uint32_t bit;
    if(slot>=32u)return;
    bit=1u<<slot;
    if(type==PROCESS_TYPE_MT){if(console_mt_dirty_mask&bit){console_mt_dirty_mask&=~bit;console_redraw_pending=1u;}}
    else if(type==PROCESS_TYPE_RT){if(console_rt_dirty_mask&bit){console_rt_dirty_mask&=~bit;console_redraw_pending=1u;}}
}
/* Stage 1: one detached RT task may run in parallel with the Ring-3 shell.
   It uses the existing per-task CR3/image/stack machinery; the current shell
   remains the foreground context and is resumed on the next timer tick. */
static volatile uint32_t rt_background_active;
/* Stage 3: up to four detached RT tasks share the existing isolated
   scheduler slots. rt_stop_requested is a per-task bit mask. */
static volatile uint32_t rt_stop_requested;
static volatile uint32_t rt_task_count;
/* RT owns a private copy of the foreground shell context.  It must not reuse
   the legacy EXE1 saved frame after RTD has returned, otherwise an ordinary
   EXE1 exit/queue operation can overwrite the context used by RT preemption. */
static uint32_t rt_shell_frame[MT_CONTEXT_WORDS];
static uint32_t rt_return_cr3;
static uint32_t rt_shell_frame_valid;
static volatile uint32_t rt_shell_waiting;
/* Foreground RTSTAT WATCH owns ESC even while an RT task is temporarily running. */
static volatile uint32_t rt_watch_active;
/* FIX36B/FIX60ZEE: synchronous shell WAIT owns ESC while a background RT/MT
   task is temporarily executing.  The extra PID/phase state guarantees that a
   wait under permanently-ready RT load cannot starve the MT task being waited
   for: after one RT service burst, one MT dispatch opportunity is reserved. */
static volatile uint32_t process_wait_active;
static volatile uint32_t process_wait_pid;
static volatile uint32_t process_wait_mt_turn;
/* FIX24: F10 starts a batch through short-lived RTD.EXE foreground helpers.
   While that batch is being constructed, do not context-switch into RT/MT: an
   old 10-ms RT release could otherwise preempt the next RTD helper before it
   completed SYS_RT_START/SYS_EXIT and corrupt the foreground return chain.
   IRQ0 still advances clocks/releases; only background dispatch is deferred. */
static volatile uint32_t rt_launch_guard;
/* FIX27: slots already active when an F10 batch begins.  Tasks created while
   the launch guard is held are committed to the RT release grid only when the
   whole batch becomes runnable. */
static volatile uint32_t rt_launch_guard_start_mask;
/* Legacy FIX60ZG hold flag.  FIX60ZEE no longer asserts it: MT is dispatched
   from the clean RT_WAIT return boundary instead of sacrificing the next
   10-ms RT release.  Keep the zero-valued field only to minimize unrelated
   state-layout churn in this release. */
static volatile uint32_t rt_shell_hold;
/* FIX60ZEF: bounded MT slack ownership.  A foreground shell continuation may
   hand the remainder of the current PIT interval to one MT task only from an
   explicit safe boundary (idle console wait or SYS_RT_YIELD).  The very next
   PIT returns that exact foreground continuation after servicing any released
   RT sweep.  This prevents RT->MT chains from retaining ownership of an old
   rtstat/command frame while still allowing MT to use sub-period slack. */
static volatile uint32_t rt_mt_slack_active;
/* Explicit ownership marker for synchronous SYS_RT_YIELD; never infer this from EAX of an arbitrary PIT-interrupted frame. */
static volatile uint32_t rt_explicit_yield_active;
/* FIX60ZED: one RT burst is a sweep of distinct completed RT jobs.
   A task that immediately reaches its next 10-ms release after SYS_RT_WAIT
   must not consume the whole burst again before peers have run.  The mask is
   reset whenever control returns to shell/MT and records slots whose current
   job completed in this burst. */
static uint32_t rt_burst_done_mask;
static uint32_t rt_chain_count;
static uint32_t rt_task_id;
static uint32_t rt_dispatch_sequence;
static volatile uint32_t rt_started_tick;
static volatile uint32_t rt_runtime_ticks;
static int user_range(uint32_t,uint32_t);
static int user_rw_range(uint32_t,uint32_t);
static void paging_set_video_user(uint32_t user){
    /*
     * VGA Mode 13h использует физический framebuffer 0xA0000..0xAFFFF.
     * Базовая таблица страниц ядра уже identity-mapped для первых 4 MiB,
     * поэтому здесь меняется только бит US у 64-KiB области VGA.
     *
     * Важное ограничение: доступ включается только для текущего обычного
     * EXE1-процесса. После SYS_VIDEO_UNMAP или завершения EXE бит US
     * снимается, поэтому следующий процесс не получает старый доступ.
     */
    uint32_t p;
    for(p=0x000a0000u;p<0x000b0000u;p+=0x1000u){
        if(user)page_table0[p>>12]|=PAGE_P|PAGE_RW|PAGE_US;
        else page_table0[p>>12]&=~PAGE_US;
    }
    __asm__ volatile("movl %%cr3,%%eax;movl %%eax,%%cr3":::"eax","memory");
    video_user_mapped=user?1u:0u;
}

static void paging_set_exec_user(uint32_t start,uint32_t end,uint32_t user){
    uint32_t p;
    start&=~0xfffu;
    end=(end+0xfffu)&~0xfffu;
    if(end>0x00400000u)end=0x00400000u;
    for(p=start;p<end;p+=0x1000u){
        if(user) page_table0[p>>12]|=PAGE_US|PAGE_RW;
        else page_table0[p>>12]&=~PAGE_US;
    }
    /* The first PUSH/CALL in Ring 3 decrements ESP from 0x003ff000 to
       0x003feffc.  Therefore the separate EXE stack page itself must be
       present with US=1 before IRET enters the program. */
    if(user){
        uint32_t sp;
        for(sp=EXEC_STACK_PAGE;sp<EXEC_STACK_PAGE+EXEC_STACK_SIZE;sp+=0x1000u)
            page_table0[sp>>12]|=PAGE_P|PAGE_RW|PAGE_US;
    }else{
        uint32_t sp;
        for(sp=EXEC_STACK_PAGE;sp<EXEC_STACK_PAGE+EXEC_STACK_SIZE;sp+=0x1000u)
            page_table0[sp>>12]&=~PAGE_US;
    }
    __asm__ volatile("movl %%cr3,%%eax;movl %%eax,%%cr3":: :"eax","memory");
}

static void enter_user_shell(void){
    /* Build only the architectural IRET frame while still using kernel segments.
       User data segments are loaded by _user_entry after the CPL3 transition.
       IF is enabled in the IRET frame; CPL3 cannot use STI when IOPL=0.
       The ISR path is safe even if an IRQ arrives before segment setup completes. */
    __asm__ volatile("cli;pushl $0x23;pushl $0x003fffe0;pushfl;orl $0x200,(%%esp);pushl $0x1b;pushl $_user_entry;iret":: :"memory");
}
static void idt_set(uint8_t n,void(*fn)(void),uint8_t dpl){uint32_t a=(uint32_t)fn;idt[n].off_lo=(uint16_t)a;idt[n].sel=8;idt[n].zero=0;idt[n].type=(uint8_t)(0x8e|(dpl?0x60:0));idt[n].off_hi=(uint16_t)(a>>16);}

/* The common stub pushes PUSHA after the four segment registers.
   Therefore memory at the dispatcher argument starts with EDI..EAX,
   followed by GS..DS, then INT number/error and the CPU frame. */
struct frame{uint32_t edi,esi,ebp,oes,ebx,edx,ecx,eax;uint32_t gs,fs,es,ds;uint32_t int_no,error;uint32_t eip,cs,eflags;} __attribute__((packed));
static void sched_exit(struct frame*);
static volatile uint32_t timer_ticks;
/* Stage 6.1: hardware PIT IRQ0 runs at 100 Hz for a real 10 ms RT timebase.
   timer_ticks remains a logical 50 Hz system clock for legacy SYS_TIMER_GET. */
static volatile uint32_t rt_time_ticks;
static uint32_t rt_time_phase;
static volatile uint8_t ata_irq_seen;
static int disk_read(uint32_t,void*,uint32_t); static int disk_write(uint32_t,const void*,uint32_t);
static uint8_t keybuf[256];static volatile uint8_t khead,ktail;static uint8_t shift_state;static uint8_t keyboard_e0;
/* FIX55: COM1 IRQ transport.  The IRQ path is intentionally bounded: it only
   moves bytes/status between the 16550 and fixed kernel rings.  Protocols,
   console and FAT I/O are forbidden here. */
#define UART1_BASE 0x3f8u
#define UART_RX_CAP 256u
#define UART_TX_CAP 256u
static volatile uint16_t uart_rx_head,uart_rx_tail,uart_tx_head,uart_tx_tail;
static uint8_t uart_rx_buf[UART_RX_CAP],uart_tx_buf[UART_TX_CAP];
static volatile uint32_t uart_rx_bytes,uart_tx_bytes,uart_rx_overrun,uart_line_errors,uart_irq_count;
static volatile uint32_t uart_last_rx_us,uart_last_irq_us;
static volatile uint32_t uart_transport_enabled;
static volatile uint32_t uart_hw_poll_count;
/* FIX60P: physical-path diagnostics are intentionally counters only. They
   never print, allocate, touch FAT, or alter protocol decisions. */
static volatile uint32_t uart_hw_poll_calls,uart_hw_rx_bytes,uart_hw_tx_bytes;
static volatile uint32_t uart_lsr_oe,uart_lsr_pe,uart_lsr_fe,uart_lsr_bi;
static volatile uint32_t uart_rx_max_queued,uart_flush_hw_bytes,uart_last_lsr;
static uint16_t pit_reload;
static uint32_t serial_time_us(void){
    uint32_t t1,t2,elapsed;uint16_t c;
    /* IRQ0 may advance rt_time_ticks between the samples; retry until the
       coarse epoch is stable. PIT channel 0 counts down at 1.193182 MHz. */
    do{t1=rt_time_ticks;outb(0x43u,0x00u);c=(uint16_t)inb(0x40u);c|=(uint16_t)((uint16_t)inb(0x40u)<<8);t2=rt_time_ticks;}while(t1!=t2);
    elapsed=(uint32_t)((pit_reload-c)%pit_reload);
    return t1*10000u+(elapsed*1000000u)/1193182u;
}
static void uart1_set_tx_irq(uint8_t on){uint8_t ier=inb(UART1_BASE+1u);if(on)ier|=2u;else ier&=(uint8_t)~2u;outb(UART1_BASE+1u,ier);}
/* FIX60E: IRQ4 must never touch/latch PIT channel 0.  The scheduler owns PIT0.
   IRQ timestamps use the already-maintained 100 Hz epoch; high-resolution
   serial_time_us() remains available to Ring-3 through syscall 70/op4. */
static uint32_t uart_irq_time_us(void){return rt_time_ticks*10000u;}
static void uart_note_lsr(uint8_t lsr){uart_last_lsr=lsr;if(lsr&0x02u)uart_lsr_oe++;if(lsr&0x04u)uart_lsr_pe++;if(lsr&0x08u)uart_lsr_fe++;if(lsr&0x10u)uart_lsr_bi++;if(lsr&0x1eu)uart_line_errors++;}
static void uart1_rx_push(uint8_t v){uint16_t n=(uint16_t)((uart_rx_head+1u)%UART_RX_CAP);uint32_t q;if(n==uart_rx_tail){uart_rx_overrun++;return;}uart_rx_buf[uart_rx_head]=v;uart_rx_head=n;uart_rx_bytes++;uart_last_rx_us=uart_irq_time_us();q=(uint32_t)((uart_rx_head+UART_RX_CAP-uart_rx_tail)%UART_RX_CAP);if(q>uart_rx_max_queued)uart_rx_max_queued=q;}
static uint32_t uart1_rx_pop(uint8_t*dst,uint32_t n){uint32_t k=0;while(k<n&&uart_rx_tail!=uart_rx_head){dst[k++]=uart_rx_buf[uart_rx_tail];uart_rx_tail=(uint16_t)((uart_rx_tail+1u)%UART_RX_CAP);}return k;}
/* FIX60G: physical RX is deliberately polled from the Ring-3 transport read
   syscall. QEMU's host serial backend repeatedly hard-hung the whole guest on
   the first real RX IRQ4 although deterministic ring tests passed. Keep RX
   bounded and out of IRQ4; TX remains interrupt driven. */
#define UART_POLL_BURST 32u
static uint32_t uart1_poll_rx_hw(void){uint32_t k=0u;uart_hw_poll_calls++;while(k<UART_POLL_BURST){uint8_t lsr=inb(UART1_BASE+5u);uart_last_lsr=lsr;if(lsr&0x1eu)uart_note_lsr(lsr);if(!(lsr&1u))break;uart1_rx_push(inb(UART1_BASE));uart_hw_rx_bytes++;k++;}if(k)uart_hw_poll_count++;return k;}
/* FIX56: bounded Sensor/Data Channel.  Kernel owns generation, sequence and timestamp. */
#define DATA_CH_MAX 4u
#define DATA_CH_DEPTH 8u
#define DATA_CH_PAYLOAD 32u
#define DATA_READER_MAX 8u
#define DATA_READ_SAMPLE 1u
#define DATA_READ_OVERRUN 2u
#define DATA_READ_GENERATION 3u
struct data_sample{uint32_t generation,sequence,timestamp_us,status,length;uint8_t payload[DATA_CH_PAYLOAD];};
struct data_channel{uint32_t generation,next_sequence,count,published,overwrites;struct data_sample ring[DATA_CH_DEPTH];};
struct data_reader{uint32_t used,channel,generation,next_sequence,overruns,owner_pid;};
static struct data_channel data_channels[DATA_CH_MAX];
static struct data_reader data_readers[DATA_READER_MAX];
static void data_reset_all(void){uint32_t i,j;for(i=0;i<DATA_CH_MAX;i++){data_channels[i].generation=0u;data_channels[i].next_sequence=1u;data_channels[i].count=0u;data_channels[i].published=0u;data_channels[i].overwrites=0u;for(j=0;j<DATA_CH_DEPTH;j++)data_channels[i].ring[j].length=0u;}for(i=0;i<DATA_READER_MAX;i++){data_readers[i].used=0u;data_readers[i].owner_pid=0u;}}
static uint32_t data_oldest_sequence(struct data_channel*c){return c->next_sequence-c->count;}
static uint32_t data_begin(uint32_t ch){struct data_channel*c;uint32_t i;if(ch>=DATA_CH_MAX)return 0xffffffffu;c=&data_channels[ch];c->generation++;if(c->generation==0u)c->generation=1u;c->next_sequence=1u;c->count=0u;c->published=0u;c->overwrites=0u;for(i=0;i<DATA_CH_DEPTH;i++)c->ring[i].length=0u;return c->generation;}
static uint32_t data_reader_open(uint32_t ch,uint32_t owner_pid){uint32_t i;if(ch>=DATA_CH_MAX||data_channels[ch].generation==0u)return 0xffffffffu;for(i=0;i<DATA_READER_MAX;i++)if(!data_readers[i].used){data_readers[i].used=1u;data_readers[i].channel=ch;data_readers[i].generation=data_channels[ch].generation;data_readers[i].next_sequence=data_oldest_sequence(&data_channels[ch]);data_readers[i].overruns=0u;data_readers[i].owner_pid=owner_pid;return i+1u;}return 0xffffffffu;}
static uint32_t data_reader_close(uint32_t h){if(h==0u||h>DATA_READER_MAX||!data_readers[h-1u].used)return 0xffffffffu;data_readers[h-1u].used=0u;data_readers[h-1u].owner_pid=0u;return 0u;}
/* FIX60R: Data Channel readers are process resources just like FAT handles.
   Forced MT stop/exit cannot run Ring-3 cleanup, so lifecycle cleanup must be
   kernel-owned or repeated SONARLOG restarts eventually exhaust all readers. */
static void data_readers_close_pid(uint32_t pid){uint32_t i;if(!pid)return;for(i=0u;i<DATA_READER_MAX;i++)if(data_readers[i].used&&data_readers[i].owner_pid==pid){data_readers[i].used=0u;data_readers[i].owner_pid=0u;}}
static uint32_t uart1_tx_push(const uint8_t*src,uint32_t n){uint32_t k=0;while(k<n){uint16_t nx=(uint16_t)((uart_tx_head+1u)%UART_TX_CAP);if(nx==uart_tx_tail)break;uart_tx_buf[uart_tx_head]=src[k++];uart_tx_head=nx;}return k;}
/* FIX60H: physical SONAR transport is fully polled.  QEMU/com0com repeatedly
   hard-hung after the second real transaction while THRE IRQ was re-enabled.
   Service a bounded number of queued TX bytes from Ring0 syscall context; IER
   stays zero, so neither RX nor TX can generate IRQ4 for this transport. */
static uint32_t uart1_poll_tx_hw(void){uint32_t k=0u;while(k<UART_POLL_BURST&&uart_tx_tail!=uart_tx_head){uint8_t lsr=inb(UART1_BASE+5u);uart_last_lsr=lsr;if(!(lsr&0x20u))break;outb(UART1_BASE,uart_tx_buf[uart_tx_tail]);uart_tx_tail=(uint16_t)((uart_tx_tail+1u)%UART_TX_CAP);uart_tx_bytes++;uart_hw_tx_bytes++;k++;}return k;}
/* FIX60F: service exactly one 16550 interrupt reason per IRQ entry.  A physical
   backend is allowed to reassert IRQ4 for remaining FIFO work after EOI; the
   kernel must not spin in Ring0 trying to drain a continuously changing IIR.
   RX/TX work is capped per entry so PIT/scheduler/keyboard always regain CPU. */
#define UART_IRQ_BURST 8u
static void uart1_irq(void){
    uint8_t iir=inb(UART1_BASE+2u),lsr;uint32_t k=0u;
    uart_irq_count++;uart_last_irq_us=uart_irq_time_us();
    if(iir&1u)return;
    switch(iir&0x0eu){
    case 0x06u:
        lsr=inb(UART1_BASE+5u);uart_note_lsr(lsr);
        if(lsr&1u)uart1_rx_push(inb(UART1_BASE));
        break;
    case 0x04u:case 0x0cu:
        while(k<UART_IRQ_BURST){lsr=inb(UART1_BASE+5u);uart_note_lsr(lsr);if(!(lsr&1u))break;uart1_rx_push(inb(UART1_BASE));k++;}
        break;
    case 0x02u:
        while(k<UART_IRQ_BURST&&uart_tx_tail!=uart_tx_head&&(inb(UART1_BASE+5u)&0x20u)){outb(UART1_BASE,uart_tx_buf[uart_tx_tail]);uart_tx_tail=(uint16_t)((uart_tx_tail+1u)%UART_TX_CAP);uart_tx_bytes++;k++;}
        if(uart_tx_tail==uart_tx_head)uart1_set_tx_irq(0u);
        break;
    case 0x00u:(void)inb(UART1_BASE+6u);break;
    default:(void)inb(UART1_BASE+5u);break;
    }
}
static void uart1_reset(void){uint32_t f;__asm__ volatile("pushfl;popl %0;cli":"=r"(f)::"memory");uart_rx_head=uart_rx_tail=uart_tx_head=uart_tx_tail=0u;uart_rx_bytes=uart_tx_bytes=uart_rx_overrun=uart_line_errors=uart_irq_count=uart_hw_poll_count=0u;uart_hw_poll_calls=uart_hw_rx_bytes=uart_hw_tx_bytes=0u;uart_lsr_oe=uart_lsr_pe=uart_lsr_fe=uart_lsr_bi=0u;uart_rx_max_queued=uart_flush_hw_bytes=uart_last_lsr=0u;uart_last_rx_us=uart_last_irq_us=0u;outb(UART1_BASE+1u,0u);if(f&0x200u)__asm__ volatile("sti":::"memory");}
static void uart1_init(void){uart_transport_enabled=0u;outb(UART1_BASE+1u,0u);outb(UART1_BASE+3u,0x80u);outb(UART1_BASE,1u);outb(UART1_BASE+1u,0u);outb(UART1_BASE+3u,0x03u);outb(UART1_BASE+2u,0xc7u);outb(UART1_BASE+4u,0x0bu);uart1_reset();}

/* PS/2 Set-1: индекс строки = непосредственно scancode; 0x1C = Enter. */
static const char keymap[128]="\0\0331234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;'`\0\\zxcvbnm,./\0\0\0 \0";
static const char keymap_shift[128]="\0\033!@#$%^&*()_+\b\tQWERTYUIOP{}\n\0ASDFGHJKL:\"~\0|ZXCVBNM<>?\0\0\0 \0";
#define KEY_EVENT_F10 0xf0u
#define KEY_EVENT_UP  0xf1u
static void key_push(uint8_t c){uint8_t n=(uint8_t)(khead+1);if(n!=ktail){keybuf[khead]=c;khead=n;}}
static uint8_t key_pop(void){uint8_t c;if(khead==ktail)return 0;c=keybuf[ktail];ktail++;return c;}

static void pic_remap(void){
    /* Keep both 8259A chips completely masked during the whole ICW sequence.
       Do not inherit BIOS masks: a stale/unexpected IRQ must not reach an
       exception vector while protected mode is being prepared. */
    outb(PIC1+1,0xff);
    outb(PIC2+1,0xff);
    io_delay();

    /* ICW1: cascaded PICs, ICW4 follows. */
    outb(PIC1,0x11); io_delay();
    outb(PIC2,0x11); io_delay();
    /* ICW2: IRQ0..7 -> INT 32..39, IRQ8..15 -> INT 40..47. */
    outb(PIC1+1,0x20); io_delay();
    outb(PIC2+1,0x28); io_delay();
    /* ICW3: master has slave on IRQ2; slave identity is 2. */
    outb(PIC1+1,0x04); io_delay();
    outb(PIC2+1,0x02); io_delay();
    /* ICW4: 8086/88 mode, normal EOI. */
    outb(PIC1+1,0x01); io_delay();
    outb(PIC2+1,0x01); io_delay();

    /* Known-safe mask: only IRQ0 (timer) and IRQ1 (keyboard) may later be
       enabled explicitly by irq_enable().  IRQ2 remains masked because the
       slave PIC is not used by this toy kernel; ATA IRQ14 also stays masked. */
    outb(PIC1+1,0xfc);
    outb(PIC2+1,0xff);
    io_delay();
}
static void irq_enable(uint8_t irq){uint16_t p=irq<8?PIC1+1:PIC2+1;uint8_t m=inb(p);if(irq<8)m&=(uint8_t)~(1u<<irq);else m&=(uint8_t)~(1u<<(irq-8));outb(p,m);}
static void irq_eoi(uint32_t n){if(n>=40)outb(PIC2,0x20);if(n>=32&&n<48)outb(PIC1,0x20);}
#define PIT_HZ 50u
#define RT_TIME_HZ 100u
static void pit_init(void){uint32_t div=(1193182u+(RT_TIME_HZ/2u))/RT_TIME_HZ;pit_reload=(uint16_t)div;outb(0x43,0x36);outb(PIT,(uint8_t)div);outb(PIT,(uint8_t)(div>>8));}

static void keyboard_init(void){
    uint32_t i;
    /* 8042 command 0xAE enables the keyboard interface.  Drain any stale
       bytes left by the BIOS before accepting IRQ1 input. */
    outb(KBD_STATUS,0xae);
    io_delay();
    for(i=0;i<32u;i++){if(!(inb(KBD_STATUS)&1u))break;(void)inb(KBD);}
}
static void keyboard_irq(void){
    uint8_t st=inb(KBD_STATUS);
    uint8_t s;
    if(!(st&1u))return;
    s=inb(KBD);
    if(s==0xe0){keyboard_e0=1u;return;}
    if(s==0xe1){keyboard_e0=0u;return;}
    if(keyboard_e0){
        keyboard_e0=0u;
        if(s==0x48){key_push(KEY_EVENT_UP);return;}
        if(s==0xc8)return;
        return;
    }
    if(s==0x01){
        if(queue_active)queue_stop_requested=1;
        else if(rt_watch_active||process_wait_active)key_push(0x1bu);
        else if(rt_background_active){
            /* ESC belongs to the foreground shell when the shell is the
               current Ring-3 context (not to an arbitrary RT task).  This is
               required by rtstat watch: ESC exits watch without terminating
               SENSOR1..SENSOR8.  An RT task itself still receives ESC as its
               private stop request. */
            if(read_cr3()==sched_saved_shell_cr3)key_push(0x1bu);
            else {
                uint32_t id=rt_task_id;
                if(id<RT_MAX_TASKS&&rt_tasks[id].state!=TASK_STOPPED&&rt_tasks[id].state!=TASK_EXIT){
                    rt_stop_requested|=(1u<<id);
                    if(rt_tasks[id].state==TASK_BLOCKED)rt_tasks[id].state=TASK_READY;
                }
            }
        }else key_push(0x1bu);
        return;
    }
    if(s==0x2a||s==0x36){shift_state=1;return;}
    if(s==0xaa||s==0xb6){shift_state=0;return;}
    if(s==0x44){key_push(KEY_EVENT_F10);return;}
    if(!(s&0x80)&&s<128){
        char c=shift_state?keymap_shift[s]:keymap[s];
        if(c)key_push((uint8_t)c);
    }
}
static void ata_irq(void){(void)inb(ATA_STATUS);ata_irq_seen=1;}

/* ---------- ATA PIO ---------- */
static void ata_400ns(void){uint32_t i;for(i=0;i<4;i++)io_delay();}
static int ata_ready(void){uint32_t i;uint8_t s;for(i=0;i<100000u;i++){s=inb(ATA_STATUS);if(!(s&0x80)&&(s&0x40))return 1;io_delay();}return 0;}
static int ata_wait_drq(void){uint32_t i;uint8_t s;for(i=0;i<100000u;i++){s=inb(ATA_STATUS);if(s&1)return 0;if(s&0x20)return 0;if(!(s&0x80)&&(s&8))return 1;io_delay();}return 0;}
static int ata_read(uint32_t lba,void*buf){uint32_t i;if(!ata_ready())return 0;outb(ATA_CTRL,0);outb(ATA_DRIVE,(uint8_t)(0xe0|((lba>>24)&15)));outb(ATA_NSECT,1);outb(ATA_LBA0,(uint8_t)lba);outb(ATA_LBA1,(uint8_t)(lba>>8));outb(ATA_LBA2,(uint8_t)(lba>>16));outb(ATA_STATUS,0x20);if(!ata_wait_drq())return 0;for(i=0;i<256;i++)((uint16_t*)buf)[i]=inw(ATA_DATA);ata_400ns();return 1;}
static int ata_write(uint32_t lba,const void*buf){uint32_t i;if(!ata_ready())return 0;outb(ATA_CTRL,0);outb(ATA_DRIVE,(uint8_t)(0xe0|((lba>>24)&15)));outb(ATA_NSECT,1);outb(ATA_LBA0,(uint8_t)lba);outb(ATA_LBA1,(uint8_t)(lba>>8));outb(ATA_LBA2,(uint8_t)(lba>>16));outb(ATA_STATUS,0x30);if(!ata_wait_drq())return 0;for(i=0;i<256;i++)outw(ATA_DATA,((const uint16_t*)buf)[i]);outb(ATA_STATUS,0xe7);return ata_ready();}
static int disk_read(uint32_t lba,void*buf,uint32_t n){uint32_t i;uint8_t*p=(uint8_t*)buf;for(i=0;i<n;i++,lba++,p+=SECTOR_SIZE)if(!ata_read(lba,p))return 0;return 1;}
static int disk_write(uint32_t lba,const void*buf,uint32_t n){uint32_t i;const uint8_t*p=(const uint8_t*)buf;for(i=0;i<n;i++,lba++,p+=SECTOR_SIZE)if(!ata_write(lba,p))return 0;return 1;}


/* FIX53 dynamic disk layout.  LBA1 is copied by the BIOS loader to 0x0600.
   All active disk-layout consumers use this descriptor rather than a compiled
   FAT LBA constant.  Descriptor DWORD checksum is two's-complement: sum=0. */
#define LAYOUT_MAGIC 0x3159414cu /* "LAY1" */
#define LAYOUT_WORDS 16u
static uint32_t layout_valid=0u;
static uint32_t layout_word(uint32_t i){return ((volatile uint32_t*)0x00000600u)[i];}
static uint32_t layout_validate(void){uint32_t i,sum=0u,ksecs,kend,disk_end,reserve_end,min_fat;
    if(layout_word(0)!=LAYOUT_MAGIC||layout_word(1)!=1u||layout_word(2)!=64u)return 0u;
    for(i=0u;i<LAYOUT_WORDS;i++)sum+=layout_word(i);if(sum)return 0u;
    ksecs=layout_word(6);
    if(layout_word(4)<2u||!layout_word(5)||!ksecs||ksecs!=1u+(layout_word(5)-1u)/512u||ksecs>0xffffu)return 0u;
    if(!layout_word(7)||!layout_word(8)||!layout_word(10)||!layout_word(12)||layout_word(13)>75u||layout_word(9)%layout_word(12))return 0u;
    disk_end=layout_word(4)+ksecs;if(disk_end<layout_word(4))return 0u;
    reserve_end=disk_end+layout_word(7);if(reserve_end<disk_end)return 0u;
    min_fat=reserve_end+layout_word(8);if(min_fat<reserve_end||layout_word(9)<min_fat)return 0u;
    disk_end=layout_word(9)+layout_word(10);if(disk_end<layout_word(9)||disk_end!=layout_word(11))return 0u;
    if(layout_word(14)!=0x00007e00u||layout_word(15)<=layout_word(14)||layout_word(15)>0x000a0000u)return 0u;
    if(ksecs>(0xffffffffu-layout_word(14))/512u)return 0u;kend=layout_word(14)+ksecs*512u;if(kend>layout_word(15))return 0u;
    return 1u;
}
/* ---------- FAT16 ---------- */
struct fat16{uint32_t part,fat_start,root_start,data_start,total_sectors,data_sectors;uint16_t reserved,fats,spf,root_entries,max_cluster;uint8_t spc,shift,ok;} fat;
static uint8_t sec[SECTOR_SIZE];
/* v61: текущий рабочий каталог shell. Формат FAT16 не меняется: cwd хранится
 * только в ядре как канонический текстовый путь. После загрузки тома cwd=ROOT. */
static char fat_cwd[128]="/";
static uint16_t le16(const uint8_t*p){return (uint16_t)p[0]|((uint16_t)p[1]<<8);}
static uint32_t le32(const uint8_t*p){return (uint32_t)le16(p)|((uint32_t)le16(p+2)<<16);}
static void st16(uint8_t*p,uint16_t v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);}
static void st32(uint8_t*p,uint32_t v){st16(p,(uint16_t)v);st16(p+2,(uint16_t)(v>>16));}
static int fat_read_sector(uint32_t lba){return ata_read(lba,sec);}
static int fat_write_sector(uint32_t lba){return ata_write(lba,sec);}

static int fat_mount(uint32_t part){uint32_t total,rootsec,used,datasec,clusters,hidden;uint8_t spc,shift=0;uint16_t bps,res,fats,root,spf;
    if(!fat_read_sector(part))return 0;
    if(sec[510]!=0x55||sec[511]!=0xaa)return 0;
    bps=le16(sec+11);spc=sec[13];res=le16(sec+14);fats=sec[16];root=le16(sec+17);spf=le16(sec+22);hidden=le32(sec+28);
    total=le16(sec+19);if(!total)total=le32(sec+32);
    if(bps!=SECTOR_SIZE||!spc||spc>128||(spc&(spc-1))||!res||!fats||!spf||!root||!total||hidden!=part)return 0;
    rootsec=((uint32_t)root+15u)>>4;
    used=(uint32_t)res+(uint32_t)fats*spf+rootsec;
    if(total<=used)return 0;
    datasec=total-used;
    while(((uint8_t)1u<<shift)<spc)shift++;
    clusters=datasec>>shift;
    if(clusters<4085u||clusters>=65525u)return 0;
    fat.part=part;fat.reserved=res;fat.fats=fats;fat.spf=spf;fat.root_entries=root;
    fat.fat_start=part+res;fat.root_start=fat.fat_start+(uint32_t)fats*spf;fat.data_start=part+used;
    fat.total_sectors=total;fat.data_sectors=datasec;fat.spc=spc;fat.shift=(uint8_t)(9u+shift);fat.max_cluster=(uint16_t)(clusters+1u);fat.ok=1;fat_cwd[0]='/';fat_cwd[1]=0;return 1;
}

/*
 * Преобразует один компонент пути в классическое имя FAT 8.3.
 *
 * Основной файловый слой использует классические имена FAT 8.3. Единственное
 * специальное системное имя AUTOSTART.SH обрабатывается через короткий alias
 * AUTOST~1.SH; build-утилита дополнительно записывает LFN-entry для внешней
 * читаемости имени. Все обычные файлы и каталоги по-прежнему работают строго
 * по 8.3.
 * Вход должен содержать один компонент без '/' и '\\'.
 */
static int make83(const char*in,uint8_t out[11]){
    uint32_t i=0,j=0;uint32_t dot=0;
    if(!in||!in[0])return 0;
    /* AUTOSTART.SH не является классическим 8.3 (9 символов в base),
       поэтому build-time LFN использует короткий внутренний alias. Все
       операции ядра при этом могут обращаться к фиксированному имени. */
    if(str_len(in)==12u&&in[0]=='A'&&in[1]=='U'&&in[2]=='T'&&in[3]=='O'&&
       in[4]=='S'&&in[5]=='T'&&in[6]=='A'&&in[7]=='R'&&in[8]=='T'&&
       in[9]=='.'&&in[10]=='S'&&in[11]=='H'){
        const uint8_t alias[11]={'A','U','T','O','S','T','~','1','S','H',' '};
        mem_copy(out,alias,11);
        return 1;
    }
    mem_set(out,' ',11);
    while(in[i]&&in[i]!='.'){
        char c=in[i++];
        if(c=='/'||c=='\\'||c==' '||c=='\t'||j>=8u)return 0;
        if(c>='a'&&c<='z')c-=32;
        out[j++]=(uint8_t)c;
    }
    if(j==0)return 0;
    if(in[i]=='.'){
        dot=1;i++;j=8;
        if(!in[i])return 0;
        while(in[i]){
            char c=in[i++];
            if(c=='/'||c=='\\'||c==' '||c=='\t'||j>=11u)return 0;
            if(c=='.')return 0;
            if(c>='a'&&c<='z')c-=32;
            out[j++]=(uint8_t)c;
        }
    }
    (void)dot;
    return 1;
}

/*
 * В FAT16 корневой каталог имеет фиксированный диапазон секторов, а любой
 * обычный каталог является цепочкой кластеров. Для унификации функции ниже
 * используют first_cluster=0 для ROOT и ненулевой кластер для подкаталога.
 */
static uint32_t fat_root_sectors(void){return ((uint32_t)fat.root_entries+15u)>>4;}

/* Взаимные объявления: функции работы с FAT используются и слоем каталогов. */
static int fat_get(uint16_t cluster,uint16_t*out);
static int fat_set(uint16_t cluster,uint16_t value);
static int fat_alloc_cluster(uint16_t*out);
static uint32_t fat_cluster_lba(uint16_t cluster);
static int fat_zero_cluster(uint16_t cluster);
static int fat_chain_last(uint16_t first,uint16_t*out_last,uint32_t*count);

static int fat_dir_entry_match(const uint8_t*d,const uint8_t n[11]){
    uint32_t i;
    if(d[0]==0x00||d[0]==0xe5||d[11]==0x0f||(d[11]&0x08u))return 0;
    for(i=0;i<11u;i++)if(d[i]!=n[i])return 0;
    return 1;
}

/*
 * Ищет один компонент только внутри указанного каталога.
 * Возвращает физический адрес directory entry: LBA сектора + смещение.
 */
static int fat_dir_lookup(uint16_t first_cluster,const uint8_t n[11],uint32_t*dir_lba,uint16_t*off){
    uint32_t s,e,idx,ci;uint16_t cl;
    if(first_cluster==0){
        s=fat.root_start;e=s+fat_root_sectors();
        for(;s<e;s++){
            if(!fat_read_sector(s))return 0;
            for(idx=0;idx<16u;idx++){
                uint8_t*d=sec+idx*32u;
                if(d[0]==0x00)return 0;
                if(fat_dir_entry_match(d,n)){*dir_lba=s;*off=(uint16_t)(idx*32u);return 1;}
            }
        }
        return 0;
    }
    cl=first_cluster;ci=0;
    for(;;){
        uint32_t base=fat_cluster_lba(cl);
        for(idx=0;idx<(uint32_t)fat.spc*16u;idx++){
            uint32_t sector_index=idx/16u,entry_index=idx%16u;
            if(!fat_read_sector(base+sector_index))return 0;
            {uint8_t*d=sec+entry_index*32u;
                if(d[0]==0x00)return 0;
                if(fat_dir_entry_match(d,n)){*dir_lba=base+sector_index;*off=(uint16_t)(entry_index*32u);return 1;}
            }
        }
        {uint16_t next;if(!fat_get(cl,&next))return 0;if(next>=FAT16_EOC)return 0;if(next<2u||next>fat.max_cluster||next==FAT16_BAD)return 0;cl=next;}
        if(++ci>fat.data_sectors/fat.spc+1u)return 0;
    }
}

/*
 * Находит свободную directory entry. Для ROOT свободны только его фиксированные
 * 512 записей. Для подкаталога при нехватке места автоматически добавляется
 * новый кластер и он предварительно обнуляется.
 */
static int fat_dir_find_free(uint16_t first_cluster,uint32_t*dir_lba,uint16_t*off){
    uint32_t s,e,idx;uint16_t cl,last,next;
    if(first_cluster==0){
        s=fat.root_start;e=s+fat_root_sectors();
        for(;s<e;s++){
            if(!fat_read_sector(s))return 0;
            for(idx=0;idx<16u;idx++){uint8_t*d=sec+idx*32u;if(d[0]==0x00||d[0]==0xe5){*dir_lba=s;*off=(uint16_t)(idx*32u);return 1;}}
        }
        return 0;
    }
    cl=first_cluster;
    for(;;){
        uint32_t base=fat_cluster_lba(cl);
        for(idx=0;idx<(uint32_t)fat.spc*16u;idx++){
            uint32_t sector_index=idx/16u,entry_index=idx%16u;
            if(!fat_read_sector(base+sector_index))return 0;
            {uint8_t*d=sec+entry_index*32u;if(d[0]==0x00||d[0]==0xe5){*dir_lba=base+sector_index;*off=(uint16_t)(entry_index*32u);return 1;}}
        }
        /* Existing multi-cluster directory: scan the next cluster before
         * extending the chain.  Extending early can place a new entry after
         * an earlier 0x00 end marker, making that entry unreachable by lookup. */
        if(!fat_get(cl,&next))return 0;
        if(next<FAT16_EOC){
            if(next<2u||next>fat.max_cluster||next==FAT16_BAD)return 0;
            cl=next;
            continue;
        }
        last=cl;
        if(!fat_alloc_cluster(&next))return 0;
        if(!fat_zero_cluster(next))return 0;
        if(!fat_set(last,next))return 0;
        if(!fat_set(next,FAT16_EOC))return 0;
        *dir_lba=fat_cluster_lba(next);*off=0;return 1;
    }
}

/*
 * v61: нормализует путь до канонического абсолютного вида.
 *
 * Правила намеренно простые и полностью совместимы с FAT16 8.3:
 *   - абсолютный путь начинается с '/';
 *   - относительный путь добавляется к fat_cwd;
 *   - повторные '/' удаляются;
 *   - '.' удаляется;
 *   - '..' поднимает на один уровень, а в ROOT остаётся в ROOT;
 *   - остальные компоненты обязаны быть допустимыми FAT16 8.3;
 *   - результат не длиннее 127 символов.
 *
 * Никакого изменения структуры каталога на диске здесь нет. Это только
 * преобразование строки перед обычным fat_lookup_path/fat_resolve_parent.
 */
static int fat_lookup_path(const char*path,uint32_t*dir_lba,uint16_t*off){
    char abs[128],part[13];uint8_t n[11];uint16_t dir=0;uint32_t pos=0,len;
    if(!fat.ok||!path_normalize(fat_cwd,path,abs,sizeof(abs)))return 0;
    len=str_len(abs);
    while(pos<len&&abs[pos]=='/')pos++;
    if(pos==len)return 0; /* корень не является directory entry */
    for(;;){
        uint32_t i=0;
        while(pos<len&&abs[pos]=='/')pos++;
        if(pos==len)return 0;
        while(pos<len&&abs[pos]!='/'){if(i>=12u)return 0;part[i++]=abs[pos++];}
        part[i]=0;
        if(!make83(part,n))return 0;
        if(!fat_dir_lookup(dir,n,dir_lba,off))return 0;
        if(pos>=len)return 1;
        if(!fat_read_sector(*dir_lba))return 0;
        {uint8_t attr=sec[*off+11];uint16_t next=le16(sec+*off+26);if(!(attr&0x10u)||next<2u||next>fat.max_cluster)return 0;dir=next;}
    }
}

/*
 * Находит родительский каталог и последний компонент. Буфер component должен
 * иметь минимум 13 байт. Для пути "/BIN/HELLO.EXE" результатом будут ROOT/BIN
 * как parent_dir и HELLO.EXE как component.
 */
static int fat_resolve_parent(const char*path,uint16_t*parent_dir,char component[13]){
    char tmp[128],part[13];uint8_t n[11];uint16_t dir=0;uint32_t len,pos=0,i;
    if(!path_normalize(fat_cwd,path,tmp,sizeof(tmp)))return 0;
    len=str_len(tmp);
    while(len&&tmp[len-1]=='/')tmp[--len]=0;
    if(!len)return 0;
    while(pos<len&&tmp[pos]=='/')pos++;
    if(pos==len)return 0;
    for(;;){
        i=0;
        while(pos<len&&tmp[pos]!='/'){if(i>=12u)return 0;part[i++]=tmp[pos++];}
        part[i]=0;
        while(pos<len&&tmp[pos]=='/')pos++;
        if(pos==len){
            if(!make83(part,n))return 0;
            mem_copy(component,part,sizeof(part));
            *parent_dir=dir;return 1;
        }
        if(!make83(part,n))return 0;
        {uint32_t lba;uint16_t off;
            if(!fat_dir_lookup(dir,n,&lba,&off))return 0;
            if(!fat_read_sector(lba))return 0;
            if(!(sec[off+11]&0x10u))return 0;
            dir=le16(sec+off+26);if(dir<2u||dir>fat.max_cluster)return 0;
        }
    }
}

/* v61: проверяет, что путь обозначает каталог. ROOT считается каталогом. */
static int fat_is_dir(const char*path){
    char abs[128];uint32_t lba;uint16_t off;
    if(!fat.ok||!path_normalize(fat_cwd,path,abs,sizeof(abs)))return 0;
    if(abs[0]=='/'&&abs[1]==0)return 1;
    if(!fat_lookup_path(abs,&lba,&off))return 0;
    if(!fat_read_sector(lba))return 0;
    return (sec[off+11]&0x10u)!=0;
}

/* v61: меняет только kernel-side cwd после успешной проверки каталога. */
static int fat_chdir(const char*path){
    char abs[128];
    if(!path_normalize(fat_cwd,path,abs,sizeof(abs))||!fat_is_dir(abs))return 0;
    mem_copy(fat_cwd,abs,str_len(abs)+1u);
    return 1;
}

/* Совместимость со старым API: поиск теперь понимает и вложенные пути. */
static int fat_find(const char*name,uint32_t*dir_lba,uint16_t*off){return fat_lookup_path(name,dir_lba,off);}

static void fat_name_from_dir(const uint8_t*d,char*out){
    static const uint8_t alias[11]={'A','U','T','O','S','T','~','1','S','H',' '};
    uint32_t i,n=0;
    for(i=0;i<11u;i++)if(d[i]!=alias[i])break;
    if(i==11u){
        const char long_name[]="AUTOSTART.SH";
        mem_copy(out,long_name,sizeof(long_name));
        return;
    }
    for(i=0;i<8;i++){if(d[i]==' ')break;out[n++]=(char)d[i];}
    if(d[8]!=' '){out[n++]='.';for(i=8;i<11;i++){if(d[i]==' ')break;out[n++]=(char)d[i];}}out[n]=0;
}

/*
 * Печатает содержимое указанного каталога. Вызов без пути означает ROOT.
 * В выводе v60 директории показываются с суффиксом '/'. Это не часть имени,
 * а визуальный признак для пользователя shell.
 */
static int fat_ls(const char*path){
    uint16_t dir=0,cl,next;uint32_t s,e,idx;uint32_t first=1;char name[13],abs[128];
    /*
     * ВАЖНО: shell передаёт для команды `ls` относительный путь ".".
     * path_normalize() превращает его в "/", но старый код проверял
     * исходную строку и затем пытался искать "." как directory entry.
     * У ROOT нет directory entry, поэтому fat_lookup_path(".") закономерно
     * возвращал ошибку и shell печатал "ls: FAIL".
     *
     * Теперь сначала один раз канонизируем путь относительно текущего cwd,
     * а ниже работаем только с каноническим абсолютным путём. Благодаря этому
     * одинаково корректно обрабатываются `ls`, `ls .`, `ls ..`, абсолютные
     * пути и обычные относительные пути.
     */
    if(!fat.ok)return 0;
    if(!path||!path[0])path="/";
    if(!path_normalize(fat_cwd,path,abs,sizeof(abs)))return 0;
    if(abs[0]!='/'||abs[1]!=0){
        uint32_t lba;uint16_t off;
        if(!fat_lookup_path(abs,&lba,&off))return 0;
        if(!fat_read_sector(lba))return 0;
        if(!(sec[off+11]&0x10u))return 0;
        dir=le16(sec+off+26);
        if(dir<2u||dir>fat.max_cluster)return 0;
    }
    if(dir==0){s=fat.root_start;e=s+fat_root_sectors();for(;s<e;s++){if(!fat_read_sector(s))return 0;for(idx=0;idx<16u;idx++){uint8_t*d=sec+idx*32u;if(d[0]==0x00){s=e;break;}if(d[0]==0xe5||d[11]==0x0f||(d[11]&0x08u))continue;fat_name_from_dir(d,name);if(!first)console_put(' ');if(d[11]&0x10u)console_write_color(name,0x02u);else console_write_color(name,0x01u);if(d[11]&0x10u)console_put('/');first=0;}}}
    else{cl=dir;for(;;){uint32_t base=fat_cluster_lba(cl);for(idx=0;idx<(uint32_t)fat.spc*16u;idx++){uint32_t si=idx/16u,ei=idx%16u;if(!fat_read_sector(base+si))return 0;{uint8_t*d=sec+ei*32u;if(d[0]==0x00){console_put('\n');return 1;}if(d[0]==0xe5||d[11]==0x0f||(d[11]&0x08u))continue;fat_name_from_dir(d,name);if(!first)console_put(' ');if((d[11]&0x10u)&&name[0]=='.')console_write_color(name,0x02u);else if(d[11]&0x10u)console_write_color(name,0x02u);else console_write_color(name,0x01u);if(d[11]&0x10u&&name[0]!='.')console_put('/');first=0;}}if(!fat_get(cl,&next))return 0;if(next>=FAT16_EOC)break;if(next<2u||next>fat.max_cluster||next==FAT16_BAD)return 0;cl=next;}}
    console_put('\n');return 1;
}

/* Ищет свободную запись ROOT — старое имя функции оставлено для регрессии. */
static int fat_find_free_dir(uint32_t*dir_lba,uint16_t*off){return fat_dir_find_free(0,dir_lba,off);}

/* Базовые операции FAT, общие для файлов и каталогов. */
static int fat_get(uint16_t cluster,uint16_t*out){
    uint32_t o=(uint32_t)cluster*2u,l=fat.fat_start+(o>>9);
    if(cluster<2u||cluster>fat.max_cluster)return 0;
    if(!fat_read_sector(l))return 0;
    *out=le16(sec+(o&511u));return 1;
}
static int fat_set_one(uint32_t fat_base,uint16_t cluster,uint16_t value){
    uint32_t o=(uint32_t)cluster*2u,l=fat_base+(o>>9),off=o&511u;
    if(!fat_read_sector(l))return 0;
    st16(sec+off,value);
    return fat_write_sector(l);
}
static int fat_set(uint16_t cluster,uint16_t value){
    uint16_t i;
    if(cluster<2u||cluster>fat.max_cluster)return 0;
    for(i=0;i<fat.fats;i++)if(!fat_set_one(fat.fat_start+(uint32_t)i*fat.spf,cluster,value))return 0;
    return 1;
}
static int fat_alloc_cluster(uint16_t*out){
    uint32_t c;uint16_t v;
    for(c=2;c<=fat.max_cluster;c++){if(!fat_get((uint16_t)c,&v))return 0;if(v==FAT16_FREE){if(!fat_set((uint16_t)c,FAT16_EOC))return 0;*out=(uint16_t)c;return 1;}}
    return 0;
}
static uint32_t fat_cluster_lba(uint16_t cluster){return fat.data_start+(uint32_t)(cluster-2u)*fat.spc;}
static int fat_zero_cluster(uint16_t cluster){uint32_t i;mem_set(sec,0,SECTOR_SIZE);for(i=0;i<fat.spc;i++)if(!fat_write_sector(fat_cluster_lba(cluster)+i))return 0;return 1;}
static int fat_chain_last(uint16_t first,uint16_t*out_last,uint32_t*count){
    uint16_t cur=first,next;uint32_t n=1;
    if(cur<2u||cur>fat.max_cluster)return 0;
    for(;;){if(!fat_get(cur,&next))return 0;if(next>=FAT16_EOC){*out_last=cur;if(count)*count=n;return 1;}if(next<2u||next>fat.max_cluster||next==FAT16_BAD)return 0;cur=next;if(++n>fat.data_sectors/fat.spc+1u)return 0;}
}
static int fat_chain_nth(uint16_t first,uint32_t index,uint16_t*out){
    uint16_t cur=first,next;
    while(index--){if(!fat_get(cur,&next))return 0;if(next>=FAT16_EOC||next<2u||next>fat.max_cluster||next==FAT16_BAD)return 0;cur=next;}
    *out=cur;return 1;
}
static int fat_extend(uint16_t first,uint32_t needed,uint16_t*last_out){
    uint16_t last,newc;uint32_t count=0;
    if(!fat_chain_last(first,&last,&count))return 0;
    while(count<needed){if(!fat_alloc_cluster(&newc))return 0;if(!fat_set(last,newc))return 0;if(!fat_set(newc,FAT16_EOC))return 0;if(!fat_zero_cluster(newc))return 0;last=newc;count++;}
    if(last_out)*last_out=last;
    return 1;
}

struct fat_handle{uint8_t used,mode,type;uint16_t start;uint32_t size,pos;uint32_t dir_lba;uint16_t dir_off;uint32_t owner_pid;};
static struct fat_handle handles[FAT16_MAX_HANDLES];

static int fat_sync_dir(struct fat_handle*h){if(!fat_read_sector(h->dir_lba))return 0;st16(sec+h->dir_off+26,h->start);st32(sec+h->dir_off+28,h->size);return fat_write_sector(h->dir_lba);}

/* Освобождает всю цепочку кластеров файла/каталога, начиная с first. */
static int fat_free_chain(uint16_t first){
    uint16_t cur=first,next;
    if(first==0)return 1;
    if(first<2u||first>fat.max_cluster)return 0;
    for(;;){
        if(!fat_get(cur,&next))return 0;
        if(!fat_set(cur,FAT16_FREE))return 0;
        if(next>=FAT16_EOC)return 1;
        if(next<2u||next>fat.max_cluster||next==FAT16_BAD)return 0;
        cur=next;
    }
}

/*
 * Создаёт обычный файл в конкретном родительском каталоге. В v60 путь уже
 * разрешён через fat_resolve_parent(), поэтому эта функция не знает о '/'.
 */
static int fat_create_in_dir(uint16_t parent,const char*name,uint32_t mode,struct fat_handle*h){
    uint32_t lba;uint16_t off;uint8_t n[11];
    if(!make83(name,n))return 0;
    if(fat_dir_lookup(parent,n,&lba,&off)){
        if(!(mode&FAT16_MODE_TRUNC))return 0;
        if(!fat_read_sector(lba))return 0;
        if(sec[off+11]&0x10u)return 0; /* нельзя TRUNC каталог */
        if(!fat_free_chain(le16(sec+off+26)))return 0;
        if(!fat_read_sector(lba))return 0;
        st16(sec+off+26,0);st32(sec+off+28,0);
        if(!fat_write_sector(lba))return 0;
        h->dir_lba=lba;h->dir_off=off;h->start=0;h->size=0;h->pos=0;return 1;
    }
    if(!(mode&FAT16_MODE_CREATE))return 0;
    if(!fat_dir_find_free(parent,&lba,&off))return 0;
    if(!fat_read_sector(lba))return 0;
    mem_set(sec+off,0,32);mem_copy(sec+off,n,11);sec[off+11]=0x20;st16(sec+off+26,0);st32(sec+off+28,0);
    if(!fat_write_sector(lba))return 0;
    h->dir_lba=lba;h->dir_off=off;h->start=0;h->size=0;h->pos=0;return 1;
}

/*
 * Открывает файл по полному пути. Если файла нет, CREATE разрешает его
 * создание в существующем родительском каталоге.
 */
static int fat_open(const char*name,uint32_t mode){
    uint32_t i,lba;uint16_t off,parent;char leaf[13];struct fat_handle*h;uint8_t exists;
    if(!fat.ok||!name||!name[0])return -1;
    for(i=0;i<FAT16_MAX_HANDLES;i++)if(!handles[i].used)break;
    if(i==FAT16_MAX_HANDLES)return -1;
    h=&handles[i];mem_set(h,0,sizeof(*h));h->mode=(uint8_t)mode;
    exists=(uint8_t)fat_find(name,&lba,&off);
    if(!exists){
        if(!(mode&FAT16_MODE_CREATE)||!(mode&FAT16_MODE_WRITE)||!fat_resolve_parent(name,&parent,leaf))return -1;
        if(!fat_create_in_dir(parent,leaf,mode,h))return -1;
    }else{
        if(!fat_read_sector(lba))return -1;
        if(sec[off+11]&0x10u)return -1; /* SYS_FILE_OPEN открывает только файлы */
        h->dir_lba=lba;h->dir_off=off;h->start=le16(sec+off+26);h->size=le32(sec+off+28);h->pos=(mode&FAT16_MODE_APPEND)?h->size:0;
        if((mode&FAT16_MODE_TRUNC)&&!(mode&FAT16_MODE_WRITE))return -1;
        if(mode&FAT16_MODE_TRUNC){if(!fat_resolve_parent(name,&parent,leaf)||!fat_create_in_dir(parent,leaf,mode,h))return -1;}
    }
    if(!(mode&FAT16_MODE_APPEND)&&!(mode&FAT16_MODE_WRITE))h->pos=0;
    h->used=1;return (int)i;
}

/* Удаляет только обычный файл. Каталоги защищены до SYS_RMDIR. */
/*
 * Удаляет обычный файл по пути, приведённому к каноническому виду.
 *
 * v63: SYS_FILE_DELETE теперь использует тот же path_normalize(), что и
 * ls/cd/mkdir/rmdir. Это важно для команды rm: относительный путь должен
 * разрешаться относительно текущего каталога, а варианты с '.', '..' и
 * повторными '/' должны обозначать тот же объект, что и остальные команды.
 * Каталог намеренно не удаляется через этот интерфейс: для каталогов
 * существует отдельный SYS_RMDIR с проверкой cwd и проверкой пустоты.
 */
/*
 * v67.7: надёжное удаление directory entry.
 *
 * Ранее после освобождения FAT-цепочки изменялся только первый байт записи
 * каталога. Для обычного FAT16 этого достаточно, но при повторном создании
 * файлов и при наличии старых/дублирующихся записей каталога это оставляло
 * вторую видимую запись с тем же 8.3-именем. Поэтому `rm/delete` мог
 * сообщить OK, а последующий `ls` всё ещё показывал имя.
 *
 * Теперь после освобождения цепочки: 
 *   1) удаляется вся directory entry (все 32 байта обнуляются, первый байт
 *      получает стандартный FAT marker 0xE5);
 *   2) повторно просматривается родительский каталог и удаляются возможные
 *      дубликаты того же 8.3-имени;
 *   3) это выполняется и для ROOT, и для обычного каталога.
 *
 * FAT16 hard links в ToyOS не поддерживаются, поэтому наличие нескольких
 * обычных записей с одним и тем же именем является состоянием, которое при
 * удалении следует полностью устранить.
 */
static int fat_mark_deleted_entry(uint32_t lba,uint16_t off){
    if(!fat_read_sector(lba))return 0;
    mem_set(sec+off,0,32);
    sec[off]=0xe5;
    return fat_write_sector(lba);
}

static int fat_delete_duplicate_entries(uint16_t parent,const uint8_t n[11]){
    uint32_t s,e,idx;uint16_t cl;
    if(parent==0){
        s=fat.root_start;e=s+fat_root_sectors();
        for(;s<e;s++){
            if(!fat_read_sector(s))return 0;
            for(idx=0;idx<16u;idx++){
                uint8_t*d=sec+idx*32u;
                if(d[0]==0x00)break;
                if(d[0]==0xe5||d[11]==0x0f||(d[11]&0x08u))continue;
                if((d[11]&0x10u)==0u&&fat_dir_entry_match(d,n)){
                    mem_set(d,0,32);d[0]=0xe5;
                    if(!fat_write_sector(s))return 0;
                    if(!fat_read_sector(s))return 0;
                }
            }
        }
        return 1;
    }
    cl=parent;
    for(;;){
        uint32_t base=fat_cluster_lba(cl);
        for(idx=0;idx<(uint32_t)fat.spc*16u;idx++){
            uint32_t si=idx/16u,ei=idx%16u;
            if(!fat_read_sector(base+si))return 0;
            {
                uint8_t*d=sec+ei*32u;
                if(d[0]==0x00)return 1;
                if(d[0]==0xe5||d[11]==0x0f||(d[11]&0x08u))continue;
                if((d[11]&0x10u)==0u&&fat_dir_entry_match(d,n)){
                    mem_set(d,0,32);d[0]=0xe5;
                    if(!fat_write_sector(base+si))return 0;
                }
            }
        }
        {uint16_t next;if(!fat_get(cl,&next))return 0;if(next>=FAT16_EOC)return 1;if(next<2u||next>fat.max_cluster||next==FAT16_BAD)return 0;cl=next;}
    }
}

static int fat_delete(const char*name){
    char abs[128],leaf[13];
    uint8_t n[11];
    uint32_t lba;uint16_t off,parent;uint16_t first;
    if(!fat.ok||!name||!name[0])return 0;
    if(!path_normalize(fat_cwd,name,abs,sizeof(abs)))return 0;
    if(!fat_find(abs,&lba,&off))return 0;
    if(!fat_read_sector(lba))return 0;
    if(sec[off+11]&0x10u)return 0;
    first=le16(sec+off+26);
    if(!fat_free_chain(first))return 0;
    if(!fat_mark_deleted_entry(lba,off))return 0;
    if(!fat_resolve_parent(abs,&parent,leaf)||!make83(leaf,n))return 0;
    /* Remove stale duplicate entries, if an older image contains them. */
    if(!fat_delete_duplicate_entries(parent,n))return 0;
    return 1;
}
static int eq_path_for_copy(const char*a,const char*b){uint32_t i=0;if(!a||!b)return 0;while(a[i]&&b[i]&&a[i]==b[i])i++;return a[i]==0&&b[i]==0;}


/*
 * v64: копирует обычный файл, не разрушая существующий DEST при ошибке.
 *
 * Алгоритм намеренно не использует fat_open(...TRUNC) для целевого файла:
 * такой вызов сразу освобождает старую цепочку DEST. Если после этого не
 * хватит места или произойдёт ошибка диска, исходное содержимое уже нельзя
 * восстановить. Вместо этого v64 сначала строит полностью новую цепочку
 * кластеров и записывает туда все данные SOURCE. Только после успешного
 * копирования directory entry переключается на новую цепочку; старая
 * цепочка освобождается последней.
 *
 * DEST может быть как именем нового файла, так и существующим каталогом.
 * Во втором случае копия получает basename SOURCE, как ожидает обычная
 * команда cp. Каталоги SOURCE копировать этим интерфейсом нельзя.
 */
static int fat_copy_file(const char*source,const char*dest){
    char src_abs[128],dst_abs[128],target_abs[128],base[13],leaf[13];
    uint8_t n[11];
    uint32_t src_lba,dst_lba,remaining,clusters,bytes,src_sector,dst_sector;
    uint16_t src_off,dst_off,parent,src_first,new_first=0,old_first=0,src_cl,new_cl;
    int dst_exists;

    if(!fat.ok||!source||!source[0]||!dest||!dest[0])return 0;
    if(!path_normalize(fat_cwd,source,src_abs,sizeof(src_abs)))return 0;
    if(!path_normalize(fat_cwd,dest,dst_abs,sizeof(dst_abs)))return 0;
    if(src_abs[0]=='/'&&src_abs[1]==0)return 0;

    /* SOURCE обязан существовать и быть обычным файлом. */
    if(!fat_find(src_abs,&src_lba,&src_off))return 0;
    if(!fat_read_sector(src_lba))return 0;
    if(sec[src_off+11]&0x10u)return 0;
    src_first=le16(sec+src_off+26);
    remaining=le32(sec+src_off+28);
    if(remaining&& (src_first<2u||src_first>fat.max_cluster))return 0;

    /*
     * Если DEST — каталог, формируем DEST/basename(SOURCE). ROOT «/» тоже
     * является каталогом, хотя у него нет собственной directory entry.
     */
    dst_exists=0;
    if(dst_abs[0]=='/'&&dst_abs[1]==0){
        if(!path_basename(src_abs,base,sizeof(base)))return 0;
        if(!path_join(dst_abs,base,target_abs,sizeof(target_abs)))return 0;
    }else if(fat_find(dst_abs,&dst_lba,&dst_off)){
        dst_exists=1;
        if(!fat_read_sector(dst_lba))return 0;
        if(sec[dst_off+11]&0x10u){
            if(!path_basename(src_abs,base,sizeof(base)))return 0;
            if(!path_join(dst_abs,base,target_abs,sizeof(target_abs)))return 0;
        }else{
            mem_copy(target_abs,dst_abs,str_len(dst_abs)+1u);
        }
    }else{
        mem_copy(target_abs,dst_abs,str_len(dst_abs)+1u);
    }

    /* После разрешения DEST ещё раз ищем конечную directory entry. */
    if(eq_path_for_copy(src_abs,target_abs))return 0;
    if(!fat_resolve_parent(target_abs,&parent,leaf)||!make83(leaf,n))return 0;
    if(fat_dir_lookup(parent,n,&dst_lba,&dst_off)){
        dst_exists=1;
        if(!fat_read_sector(dst_lba))return 0;
        if(sec[dst_off+11]&0x10u)return 0;
        old_first=le16(sec+dst_off+26);
    }else{
        dst_exists=0;
        old_first=0;
    }

    /* Полностью собрать новую цепочку. Для пустого файла цепочка не нужна. */
    if(remaining){
        uint32_t cluster_bytes=(uint32_t)fat.spc*SECTOR_SIZE;
        clusters=(remaining+cluster_bytes-1u)/cluster_bytes;
        if(!fat_alloc_cluster(&new_first))return 0;
        if(!fat_zero_cluster(new_first)){fat_free_chain(new_first);return 0;}
        if(clusters>1u && !fat_extend(new_first,clusters,0)){fat_free_chain(new_first);return 0;}

        bytes=remaining;
        src_cl=src_first;
        new_cl=new_first;
        src_sector=0;dst_sector=0;
        while(bytes){
            uint32_t chunk=bytes<SECTOR_SIZE?bytes:SECTOR_SIZE;
            if(!fat_read_sector(fat_cluster_lba(src_cl)+src_sector)){
                fat_free_chain(new_first);return 0;
            }
            /* Последний сектор всё равно записываем целиком: размер файла
               в directory entry ограничивает видимую длину данных. */
            if(!fat_write_sector(fat_cluster_lba(new_cl)+dst_sector)){
                fat_free_chain(new_first);return 0;
            }
            bytes-=chunk;
            src_sector++;dst_sector++;
            if(src_sector>=fat.spc && bytes){
                uint16_t next;
                if(!fat_get(src_cl,&next)||next>=FAT16_EOC||next<2u||next>fat.max_cluster||next==FAT16_BAD){fat_free_chain(new_first);return 0;}
                src_cl=next;src_sector=0;
            }
            if(dst_sector>=fat.spc && bytes){
                uint16_t next;
                if(!fat_get(new_cl,&next)||next>=FAT16_EOC||next<2u||next>fat.max_cluster||next==FAT16_BAD){fat_free_chain(new_first);return 0;}
                new_cl=next;dst_sector=0;
            }
        }
    }

    /*
     * Только теперь меняем directory entry. При ошибке записи старая запись
     * остаётся нетронутой; новая цепочка освобождается как временная.
     */
    if(dst_exists){
        if(!fat_read_sector(dst_lba)){if(new_first)fat_free_chain(new_first);return 0;}
        st16(sec+dst_off+26,new_first);st32(sec+dst_off+28,remaining);
        if(!fat_write_sector(dst_lba)){if(new_first)fat_free_chain(new_first);return 0;}
        if(old_first&&!fat_free_chain(old_first))return 0;
    }else{
        if(!fat_dir_find_free(parent,&dst_lba,&dst_off)){if(new_first)fat_free_chain(new_first);return 0;}
        if(!fat_read_sector(dst_lba)){if(new_first)fat_free_chain(new_first);return 0;}
        mem_set(sec+dst_off,0,32);mem_copy(sec+dst_off,n,11);sec[dst_off+11]=0x20;
        st16(sec+dst_off+26,new_first);st32(sec+dst_off+28,remaining);
        if(!fat_write_sector(dst_lba)){if(new_first)fat_free_chain(new_first);return 0;}
    }
    return 1;
}


/*
 * Создаёт каталог по пути. Новый каталог получает один кластер и стандартные
 * записи '.' и '..'. Для '..' ROOT кодируется нулевым start cluster, как того
 * требует FAT directory convention.
 */
static int fat_mkdir(const char*path){
    uint16_t parent,newcl;char leaf[13];uint8_t n[11];uint32_t lba;uint16_t off;
    if(!fat.ok||!path||!path[0])return 0;
    if(!fat_resolve_parent(path,&parent,leaf)||!make83(leaf,n))return 0;
    if(fat_dir_lookup(parent,n,&lba,&off))return 0;
    if(!fat_alloc_cluster(&newcl))return 0;
    if(!fat_zero_cluster(newcl))return 0;
    if(!fat_dir_find_free(parent,&lba,&off)){fat_set(newcl,FAT16_FREE);return 0;}
    if(!fat_read_sector(lba))return 0;
    mem_set(sec+off,0,32);mem_copy(sec+off,n,11);sec[off+11]=0x10;st16(sec+off+26,newcl);st32(sec+off+28,0);
    if(!fat_write_sector(lba)){fat_set(newcl,FAT16_FREE);return 0;}
    /* '.' */
    if(!fat_read_sector(fat_cluster_lba(newcl)))return 0;
    mem_set(sec,0,SECTOR_SIZE);mem_set(sec,' ',32);sec[0]='.';sec[11]=0x10;st16(sec+26,newcl);
    /* '..' */
    mem_set(sec+32,' ',32);sec[32]='.';sec[33]='.';sec[43]=0x10;st16(sec+32+26,parent);
    if(!fat_write_sector(fat_cluster_lba(newcl)))return 0;
    return 1;
}

/*
 * Проверяет опасный случай для SYS_RMDIR: нельзя удалить текущий каталог
 * или любой его родительский каталог.
 *
 * Почему это обязательно: fat_cwd хранит канонический путь, а удаление
 * каталога физически не обновляет эту строку. Если разрешить `rmdir .`,
 * `rmdir ..` или удаление любого предка текущего каталога, cwd начнёт
 * ссылаться на уже удалённый объект. Последующие `ls`, `cd`, `cat` и другие
 * операции будут работать с несуществующим путём и состояние ядра станет
 * неконсистентным.
 *
 * Оба пути уже нормализуются через общий path-модуль, поэтому проверка
 * выполняется по каноническим абсолютным строкам и корректно учитывает
 * варианты с `.`, `..` и повторными `/`. ROOT отдельно запрещён как объект
 * удаления.
 */
static int fat_rmdir_would_break_cwd(const char*target){
    uint32_t i=0;
    if(!target||!target[0])return 1;
    if(target[0]=='/'&&target[1]==0)return 1;
    while(target[i]&&fat_cwd[i]&&target[i]==fat_cwd[i])i++;
    if(target[i]==0&&fat_cwd[i]==0)return 1;
    /* target является строгим родителем cwd только если после target
       в cwd начинается новый компонент пути, а не совпадающий префикс. */
    if(target[i]==0&&fat_cwd[i]=='/')return 1;
    return 0;
}

/*
 * Удаляет каталог только если он пуст. '.' и '..' не считаются пользовательскими
 * записями. Рекурсивное удаление намеренно не поддерживается в v60.
 *
 * v62.3: перед физическим удалением путь нормализуется и проверяется против
 * текущего cwd. Нельзя удалить ROOT, сам cwd или его родителя.
 */
static int fat_rmdir(const char*path){
    char abs[128],leaf[13];uint8_t n[11];
    uint16_t parent,cl,next,off;uint32_t lba,idx,base;
    if(!fat.ok||!path||!path[0])return 0;

    /*
     * Сначала получаем единый канонический абсолютный путь. Это принципиально
     * важно для относительного `rmdir B` из каталога `/A`: фактической целью
     * должна стать `/A/B`, а не `/B`.
     */
    if(!path_normalize(fat_cwd,path,abs,sizeof(abs)))return 0;
    if(fat_rmdir_would_break_cwd(abs))return 0;

    /*
     * ROOT не имеет собственной directory entry. Поэтому для удаления любого
     * обычного каталога надёжнее сначала отдельно определить его родителя и
     * последний компонент. Это также устраняет неоднозначность между поиском
     * полного пути и поиском записи, которую затем надо пометить 0xE5.
     */
    if(!fat_resolve_parent(abs,&parent,leaf))return 0;
    if(!make83(leaf,n))return 0;
    if(!fat_dir_lookup(parent,n,&lba,&off))return 0;
    if(!fat_read_sector(lba))return 0;
    if(!(sec[off+11]&0x10u))return 0;
    cl=le16(sec+off+26);
    if(cl<2u||cl>fat.max_cluster)return 0;

    /*
     * Каталог считается пустым, если в нём нет пользовательских записей.
     * Стандартные `.` и `..` игнорируются. Удаление непустого каталога
     * запрещено: v60/v61/v62.3 намеренно не поддерживают рекурсивный rmdir.
     */
    {
        uint16_t cur=cl;
        for(;;){
            base=fat_cluster_lba(cur);
            for(idx=0;idx<(uint32_t)fat.spc*16u;idx++){
                uint32_t si=idx/16u,ei=idx%16u;
                if(!fat_read_sector(base+si))return 0;
                {
                    uint8_t*d=sec+ei*32u;
                    if(d[0]==0x00)goto empty;
                    if(d[0]==0xe5||d[11]==0x0f||(d[11]&0x08u))continue;
                    if(d[0]=='.'&&(d[1]==' '||d[1]=='.'))continue;
                    return 0;
                }
            }
            if(!fat_get(cur,&next))return 0;
            if(next>=FAT16_EOC)break;
            if(next<2u||next>fat.max_cluster||next==FAT16_BAD)return 0;
            cur=next;
        }
    }
empty:
    if(!fat_free_chain(cl))return 0;
    if(!fat_read_sector(lba))return 0;
    sec[off]=0xe5;
    return fat_write_sector(lba);
}

static int fat_close(int fd){if(fd<0||fd>=(int)FAT16_MAX_HANDLES||!handles[fd].used)return 0;handles[fd].used=0;handles[fd].owner_pid=0u;return 1;}

/* FIX38: FAT descriptors opened by a Ring-3 process are owned by that PID.
   owner_pid==0 is reserved for the shell/kernel legacy context.  Cleanup is
   lifecycle-only and never participates in scheduling decisions. */
static uint32_t process_handle_owner(void){struct process_identity id=process_current_identity();return id.pid;}
static void process_handles_close_pid(uint32_t pid){uint32_t i;if(!pid)return;for(i=0u;i<FAT16_MAX_HANDLES;i++)if(handles[i].used&&handles[i].owner_pid==pid)(void)fat_close((int)i);}
static uint32_t process_handles_count(uint32_t pid,uint32_t process_only){uint32_t i,n=0u;for(i=0u;i<FAT16_MAX_HANDLES;i++)if(handles[i].used&&(!process_only||handles[i].owner_pid!=0u)&&(pid==0xffffffffu||handles[i].owner_pid==pid))n++;return n;}
static int process_handle_access(int fd,uint32_t owner){return fd>=0&&fd<(int)FAT16_MAX_HANDLES&&handles[fd].used&&handles[fd].owner_pid==owner;}

static int fat_close(int fd);

/* Return exact FAT16 directory-entry file size without consuming a handle. */
static int fat_file_size(const char*name){
    int fd=fat_open(name,FAT16_MODE_READ);
    uint32_t size;
    if(fd<0)return -1;
    size=handles[fd].size;
    fat_close(fd);
    if(size>0x7fffffffu)return -1;
    return (int)size;
}

static int fat_read_file_fd(int fd,void*dst,uint32_t count){struct fat_handle*h;uint8_t*out=(uint8_t*)dst;uint32_t done=0;uint32_t cluster_size=(uint32_t)fat.spc*SECTOR_SIZE;if(fd<0||fd>=(int)FAT16_MAX_HANDLES||!handles[fd].used||!dst)return -1;h=&handles[fd];if(h->pos>=h->size)return FAT_EOF;if(count>h->size-h->pos)count=h->size-h->pos;while(done<count){uint32_t cluster_index=h->pos/cluster_size,within=h->pos%cluster_size,sector_index=within/SECTOR_SIZE,sector_off=within%SECTOR_SIZE,n=SECTOR_SIZE-sector_off;uint16_t cl;if(n>count-done)n=count-done;if(!fat_chain_nth(h->start,cluster_index,&cl))return -1;if(!fat_read_sector(fat_cluster_lba(cl)+sector_index))return -1;mem_copy(out+done,sec+sector_off,n);done+=n;h->pos+=n;}return (int)done;}

static int fat_write_file_fd(int fd,const void*src,uint32_t count){struct fat_handle*h;const uint8_t*in=(const uint8_t*)src;uint32_t done=0,cluster_size=(uint32_t)fat.spc*SECTOR_SIZE,need_clusters;uint16_t first,last;
    if(fd<0||fd>=(int)FAT16_MAX_HANDLES||!handles[fd].used||!src)return -1;
    h=&handles[fd];
    if(!(h->mode&FAT16_MODE_WRITE))return -1;
    if(!count)return 0;
    if(h->pos>0xffffffffu-count)return -1;
    if(h->pos+count>(uint32_t)fat.data_sectors*SECTOR_SIZE)return -1;
    if(h->start==0){if(!fat_alloc_cluster(&first))return -1;if(!fat_zero_cluster(first))return -1;h->start=first;if(!fat_sync_dir(h))return -1;}
    need_clusters=(h->pos+count+cluster_size-1u)/cluster_size;if(!fat_extend(h->start,need_clusters,&last))return -1;
    while(done<count){uint32_t ci=h->pos/cluster_size,within=h->pos%cluster_size,si=within/SECTOR_SIZE,so=within%SECTOR_SIZE,n=SECTOR_SIZE-so;uint16_t cl;if(n>count-done)n=count-done;if(!fat_chain_nth(h->start,ci,&cl))return -1;
        if(so!=0||n!=SECTOR_SIZE){if(!fat_read_sector(fat_cluster_lba(cl)+si))return -1;}else mem_set(sec,0,SECTOR_SIZE);mem_copy(sec+so,in+done,n);if(!fat_write_sector(fat_cluster_lba(cl)+si))return -1;done+=n;h->pos+=n;if(h->pos>h->size)h->size=h->pos;}
    if(!fat_sync_dir(h))return -1;
    return (int)done;
}


/* FIX60 positional I/O. These operations preserve h->pos. PWRITE is deliberately
   bounded by the existing directory-entry size: cyclic telemetry can overwrite a
   preallocated file but cannot silently grow it during long autonomous operation. */
static int fat_pread_file_fd(int fd,void*dst,uint32_t count,uint32_t off){
    struct fat_handle*h;uint32_t save;int r;
    if(fd<0||fd>=(int)FAT16_MAX_HANDLES||!handles[fd].used||!dst)return -1;
    h=&handles[fd];save=h->pos;h->pos=off;r=fat_read_file_fd(fd,dst,count);h->pos=save;return r;
}
static int fat_pwrite_file_fd(int fd,const void*src,uint32_t count,uint32_t off){
    struct fat_handle*h;uint32_t save;int r;
    if(fd<0||fd>=(int)FAT16_MAX_HANDLES||!handles[fd].used||!src)return -1;
    h=&handles[fd];if(!(h->mode&FAT16_MODE_WRITE))return -1;
    if(off>h->size||count>h->size-off)return -1;
    save=h->pos;h->pos=off;r=fat_write_file_fd(fd,src,count);h->pos=save;return r;
}

/* Convenience read-only API retained for simple kernel tests. */
static int fat_read_file(const char*name,void*dst,uint32_t cap,uint32_t*got){
    int fd=fat_open(name,FAT16_MODE_READ);int n;
    if(fd<0)return 0;
    n=fat_read_file_fd(fd,dst,cap);
    fat_close(fd);
    if(n<0)return 0;
    *got=(uint32_t)n;
    return 1;
}

#define EXE_OK              0
#define EXE_ERR_BUSY       -1
#define EXE_ERR_NAME       -2
#define EXE_ERR_OPEN       -3
#define EXE_ERR_HEADER     -4
#define EXE_ERR_MAGIC      -5
#define EXE_ERR_IMAGE_SIZE -6
#define EXE_ERR_ENTRY      -7
#define EXE_ERR_BSS_SIZE   -8
#define EXE_ERR_IMAGE_READ -9
#define EXE_ERR_TRAILING   -10
#define EXE_ERR_QUEUE      -11
#define EXE_ERR_QUEUE_STOP -12
#define EXE_ERR_MEMORY      -13

/* FIX60A: executable launchers historically carry a single FAT 8.3 name in a
   13-byte ABI field. After cwd/subdirectories were added, a command such as
   `execmt SONARDRV.EXE` stopped finding root-installed system executables when
   cwd was not ROOT, while `/SONARDRV.EXE` cannot fit that legacy field. Keep
   normal cwd lookup first, then fall back to ROOT only for a simple 8.3 name. */
static int fat_open_executable(const char*name){
    int fd;uint32_t i,n=0u;char root_name[14];
    if(!name||!name[0])return -1;
    fd=fat_open(name,FAT16_MODE_READ);
    if(fd>=0)return fd;
    for(i=0u;name[i];i++){if(name[i]=='/'||name[i]=='\\')return -1;if(i>=12u)return -1;}
    if(i==0u)return -1;
    root_name[n++]='/';for(i=0u;name[i]&&n+1u<sizeof(root_name);i++)root_name[n++]=name[i];root_name[n]=0;
    return fat_open(root_name,FAT16_MODE_READ);
}

static int exe_load_args(const char*name,struct frame*f,const char args[EXEC_ARG_COUNT][EXEC_ARG_SIZE]){
    struct exe_header h;
    int fd;
    int n;
    uint32_t image_end,bss_end,file_size;
    if(exe_active)return EXE_ERR_BUSY;
    if(!name||!name[0])return EXE_ERR_NAME;
    fd=fat_open_executable(name);
    if(fd<0)return EXE_ERR_OPEN;
    /* fat_open already read the directory entry; use the exact on-disk file size
       to reject truncated headers and the old class of header/image-size mismatch. */
    file_size=handles[fd].size;
    if(file_size<sizeof(h)){fat_close(fd);return EXE_ERR_HEADER;}
    n=fat_read_file_fd(fd,&h,sizeof(h));
    if(n!=(int)sizeof(h)){fat_close(fd);return EXE_ERR_HEADER;}
    if(h.magic!=EXE_MAGIC){fat_close(fd);return EXE_ERR_MAGIC;}
    if(h.image_size==0||h.image_size>EXEC_MAX_SIZE){fat_close(fd);return EXE_ERR_IMAGE_SIZE;}
    if(h.image_size>file_size-sizeof(h)){fat_close(fd);return EXE_ERR_IMAGE_SIZE;}
    if(h.image_size!=file_size-sizeof(h)){fat_close(fd);return EXE_ERR_TRAILING;}
    if(h.entry<EXEC_LOAD_ADDR||h.entry>=EXEC_LOAD_ADDR+h.image_size){fat_close(fd);return EXE_ERR_ENTRY;}
    image_end=EXEC_LOAD_ADDR+h.image_size;
    if(h.bss_size>EXEC_MAX_SIZE-h.image_size){fat_close(fd);return EXE_ERR_BSS_SIZE;}
    bss_end=image_end+h.bss_size;
    n=fat_read_file_fd(fd,(void*)EXEC_LOAD_ADDR,h.image_size);
    fat_close(fd);
    if(n!=(int)h.image_size)return EXE_ERR_IMAGE_READ;
    mem_set((void*)image_end,0,h.bss_size);
    exe_end=bss_end;
    if(exe_end<EXEC_LOAD_ADDR+0x1000u)exe_end=EXEC_LOAD_ADDR+0x1000u;
    paging_set_exec_user(EXEC_LOAD_ADDR,exe_end,1);
    if(!queue_active){
        /* A CPL3 interrupt has two CPU words after the 17-word C frame: user ESP and SS.
           Keep them together with the visible frame so SYS_EXIT can restore shell exactly. */
        mem_set(saved_user_frame,0,sizeof(saved_user_frame));
        mem_copy(saved_user_frame,(const void*)f,sizeof(saved_user_frame));
        /* SYS_EXEC is still in progress at this point, so the saved EAX is 11.
           After HELLO.EXE calls SYS_EXIT we restore this frame and return to the
           shell.  The restored value must be the SYS_EXEC return value (0), not
           the original syscall number. */
        saved_user_frame[7]=EXE_OK;
    }
    exe_pid=process_alloc_pid();exe_proc_state=TASK_RUNNING;exe_exit_reason=PROCESS_EXIT_NONE;exe_exit_status=0u;exe_fault_vector=exe_fault_error=exe_fault_eip=exe_fault_cr2=0u;
    {uint32_t pi=0u;while(pi<QUEUE_NAME_SIZE-1u&&name[pi]){exe_proc_name[pi]=name[pi];pi++;}exe_proc_name[pi]=0;}
    exe_active=1;
    /* Передача трёх строковых параметров EXE1:
       EBX -> порт, ECX -> направление, EDX -> имя файла.
       Данные кладутся в отдельную область user stack до IRET. */
    mem_set((void*)(EXEC_STACK_PAGE+0x100u),0,EXEC_ARG_BLOCK_SIZE);
    mem_copy((void*)(EXEC_STACK_PAGE+0x100u),args[0],EXEC_ARG_SIZE);
    mem_copy((void*)(EXEC_STACK_PAGE+0x100u+EXEC_ARG_SIZE),args[1],EXEC_ARG_SIZE);
    mem_copy((void*)(EXEC_STACK_PAGE+0x100u+2u*EXEC_ARG_SIZE),args[2],EXEC_ARG_SIZE);
    f->eip=h.entry;
    f->cs=0x1bu;
    ((uint32_t*)f)[4]=EXEC_STACK_PAGE+0x100u;
    ((uint32_t*)f)[6]=EXEC_STACK_PAGE+0x100u+EXEC_ARG_SIZE;
    ((uint32_t*)f)[5]=EXEC_STACK_PAGE+0x100u+2u*EXEC_ARG_SIZE;
    ((uint32_t*)f)[17]=EXEC_STACK_TOP;
    ((uint32_t*)f)[18]=0x23u;
    return EXE_OK;
}

static int exe_load(const char*name,struct frame*f){
    static const char empty_args[EXEC_ARG_COUNT][EXEC_ARG_SIZE]={{0},{0},{0}};
    return exe_load_args(name,f,empty_args);
}

static void queue_start_next(struct frame*);
static void queue_clear_current(void){
    queue_count=0;queue_index=0;queue_rounds=0;queue_repetitions=0;queue_forever=0;queue_stop_requested=0;
}
static void queue_clear(void){queue_active=0;queue_depth=0;queue_clear_current();}

static void queue_save_parent(void){
    uint32_t i;
    struct queue_state*q=&queue_stack[queue_depth++];
    for(i=0;i<QUEUE_MAX_FILES;i++){mem_copy(q->names[i],queue_names[i],QUEUE_NAME_SIZE);mem_copy(q->args[i],queue_args[i],EXEC_ARG_BLOCK_SIZE);}
    q->count=queue_count;q->index=queue_index;q->rounds=queue_rounds;q->repetitions=queue_repetitions;q->forever=queue_forever;
}
static void queue_restore_parent(void){
    uint32_t i;
    struct queue_state*q=&queue_stack[--queue_depth];
    for(i=0;i<QUEUE_MAX_FILES;i++){mem_copy(queue_names[i],q->names[i],QUEUE_NAME_SIZE);mem_copy(queue_args[i],q->args[i],EXEC_ARG_BLOCK_SIZE);}
    queue_count=q->count;queue_index=q->index;queue_rounds=q->rounds;queue_repetitions=q->repetitions;queue_forever=q->forever;queue_stop_requested=0;
}

static void queue_finish(struct frame*f,uint32_t result){
    if(exe_active){if(video_user_mapped)paging_set_video_user(0);exe_active=0;paging_set_exec_user(EXEC_LOAD_ADDR,exe_end,0);}
    if(queue_depth){
        /* The child queue is complete. Restore the parent queue and continue
           with its next member. The child result is deliberately local: the
           parent queue is an independent execution sequence. */
        queue_restore_parent();
        queue_active=1;
        queue_start_next(f);
        return;
    }
    queue_clear();
    mem_copy(f,saved_user_frame,sizeof(saved_user_frame));
    f->eax=result;
}

static void queue_start_next(struct frame*f){
    int r;
    if(queue_stop_requested){queue_finish(f,0xffffffffu);return;}
    if(queue_index>=queue_count){
        queue_index=0;
        if(!queue_forever){
            queue_rounds++;
            if(queue_rounds>=queue_repetitions){queue_finish(f,EXE_OK);return;}
        }
    }
    exe_active=0;
    paging_set_exec_user(EXEC_LOAD_ADDR,exe_end,0);
    r=exe_load_args(queue_names[queue_index],f,queue_args[queue_index]);
    if(r!=EXE_OK){
        queue_finish(f,(uint32_t)r);
        return;
    }
    queue_index++;
}

static int queue_prepare(uint32_t p,uint32_t count,uint32_t repetitions,struct frame*f,uint32_t from_exe){
    uint32_t i,n;
    const char*src=(const char*)p;
    if(!from_exe && queue_active)return EXE_ERR_BUSY;
    if(from_exe && queue_active && queue_depth>=QUEUE_MAX_DEPTH)return EXE_ERR_BUSY;
    if(count==0u||count>QUEUE_MAX_FILES)return EXE_ERR_NAME;
    if(repetitions>QUEUE_MAX_REPS)return EXE_ERR_QUEUE;
    if(exe_active&&!from_exe)return EXE_ERR_BUSY;
    /* The queue only reads the packed names. A caller may keep the names in
       read-only .urodata, so this argument uses the readable-user-range check. */
    if(!user_range(p,count*QUEUE_NAME_SIZE))return EXE_ERR_NAME;
    /* Validate every slot before changing queue state. */
    for(i=0;i<count;i++){
        n=0;
        while(n<QUEUE_NAME_SIZE-1u&&src[i*QUEUE_NAME_SIZE+n])n++;
        if(n==0)return EXE_ERR_NAME;
    }
    /* Save the currently active parent before replacing queue_names with the
       child list. This is the essential part of nested execq semantics. */
    if(from_exe && queue_active)queue_save_parent();
    for(i=0;i<count;i++){
        n=0;
        while(n<QUEUE_NAME_SIZE-1u&&src[i*QUEUE_NAME_SIZE+n]){
            queue_names[i][n]=src[i*QUEUE_NAME_SIZE+n];
            n++;
        }
        queue_names[i][n]=0;
    }
    if(!queue_active && !exe_active){
        /* A queue started by the Ring-3 shell needs a shell-return frame.
           A standalone EXE has already saved that shell frame in exe_load();
           overwriting it with the EXE's own frame would return into an image
           whose pages are immediately revoked when the queue member exits.
           A nested EXE queue also keeps the original shell frame and only
           pushes the active parent queue state above. */
        mem_copy(saved_user_frame,(const void*)f,sizeof(saved_user_frame));
        saved_user_frame[7]=EXE_OK;
    }
    queue_count=count;queue_index=0;queue_rounds=0;queue_repetitions=repetitions;
    queue_forever=(repetitions==0u);queue_stop_requested=0;queue_active=1;
    return EXE_OK;
}

static int copy_exec_args(uint32_t p){
    uint32_t i,j;
    const char*src=(const char*)p;
    if(!user_range(p,EXEC_ARG_BLOCK_SIZE))return 0;
    for(i=0;i<EXEC_ARG_COUNT;i++){
        for(j=0;j<EXEC_ARG_SIZE;j++){
            char c=src[i*EXEC_ARG_SIZE+j];
            exe_args[i][j]=c;
            if(c==0)break;
        }
        if(j==EXEC_ARG_SIZE)return 0;
    }
    return 1;
}

static int queue_prepare_args(uint32_t p,uint32_t count,uint32_t repetitions,struct frame*f,uint32_t from_exe){
    uint32_t i,j,k;
    const uint8_t*src=(const uint8_t*)p;
    /* v57: LOADER.EXE may create a nested argument queue while its EXE1 image is active.
       A shell-originated queue keeps the old single-queue guard; an EXE-originated
       queue is permitted and, when necessary, saves the parent queue state. */
    if(!from_exe&&(queue_active||exe_active))return EXE_ERR_BUSY;
    if(from_exe&&queue_active&&queue_depth>=QUEUE_MAX_DEPTH)return EXE_ERR_BUSY;
    if(count==0u||count>QUEUE_MAX_FILES||repetitions>QUEUE_MAX_REPS)return EXE_ERR_QUEUE;
    if(!user_range(p,count*EXEC_QUEUE_RECORD_SIZE))return EXE_ERR_NAME;
    for(i=0;i<count;i++){
        const uint8_t*rec=src+i*EXEC_QUEUE_RECORD_SIZE;
        j=0; while(j<QUEUE_NAME_SIZE-1u&&rec[j])j++;
        if(j==0||j>=QUEUE_NAME_SIZE)return EXE_ERR_NAME;
        mem_set(queue_names[i],0,QUEUE_NAME_SIZE);mem_copy(queue_names[i],rec,j);
        for(k=0;k<EXEC_ARG_COUNT;k++){
            const char*a=(const char*)(rec+QUEUE_NAME_SIZE+k*EXEC_ARG_SIZE);
            j=0; while(j<EXEC_ARG_SIZE&&a[j])j++;
            if(j>=EXEC_ARG_SIZE)return EXE_ERR_NAME;
            mem_set(queue_args[i][k],0,EXEC_ARG_SIZE);mem_copy(queue_args[i][k],a,j);
        }
    }
    /* v57: если аргументная очередь создаётся из уже работающего элемента
       другой очереди, сохраняем родительскую последовательность. */
    if(from_exe&&queue_active)queue_save_parent();
    if(!queue_active&&!exe_active){
        /* Корневая очередь shell сохраняет shell frame. Для LOADER.EXE этот
           frame уже сохранён обычным SYS_EXEC/SYS_EXEC_ARGS. */
        mem_copy(saved_user_frame,(const void*)f,sizeof(saved_user_frame));
        saved_user_frame[7]=EXE_OK;
    }
    queue_count=count;queue_index=0;queue_rounds=0;queue_repetitions=repetitions;
    queue_forever=(repetitions==0u);queue_stop_requested=0;queue_active=1;
    return EXE_OK;
}

static void exe_exit(struct frame*f){
    uint32_t status=f->ebx;
    if(exe_exit_reason==PROCESS_EXIT_NONE){exe_exit_reason=PROCESS_EXIT_NORMAL;exe_exit_status=status;}
    __asm__ volatile("cli" ::: "memory");
    if(!exe_active){f->eax=0xffffffffu;return;}
    process_result_store(exe_pid,PROCESS_TYPE_FG,exe_exit_reason,exe_exit_status,exe_fault_vector,exe_fault_error,exe_fault_eip,exe_fault_cr2);
    process_handles_close_pid(exe_pid);data_readers_close_pid(exe_pid);
    if(queue_active){
        exe_proc_state=TASK_EXIT;
        exe_active=0;
        paging_set_exec_user(EXEC_LOAD_ADDR,exe_end,0);
        /* v57: очередь прекращается при ненулевом SYS_EXIT(EBX).
           Это важно для LOADER: FF.EXE не должен стартовать после
           неудачного приёма COMDRV. */
        if(status!=0u){queue_finish(f,status);return;}
        queue_start_next(f);
        return;
    }
    exe_proc_state=TASK_EXIT;
    exe_active=0;
    paging_set_exec_user(EXEC_LOAD_ADDR,exe_end,0);
    mem_copy(f,saved_user_frame,sizeof(saved_user_frame));
}

/* ---------- Syscalls ---------- */
static uint32_t syscall3(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}
uint32_t sys_console_write(const char*s,uint32_t n){return syscall3(SYS_CONSOLE_WRITE,(uint32_t)s,n,0);}uint32_t sys_console_read(char*b,uint32_t n){return syscall3(SYS_CONSOLE_READ,(uint32_t)b,n,0);}uint32_t sys_timer_get(void){return syscall3(SYS_TIMER_GET,0,0,0);}uint32_t sys_disk_read(uint32_t lba,void*b,uint32_t n){return syscall3(SYS_DISK_READ,lba,(uint32_t)b,n);}uint32_t sys_disk_write(uint32_t lba,const void*b,uint32_t n){return syscall3(SYS_DISK_WRITE,lba,(uint32_t)b,n);}
uint32_t sys_file_open(const char*name,uint32_t mode){return syscall3(SYS_FILE_OPEN,(uint32_t)name,mode,0);}uint32_t sys_file_read(uint32_t fd,void*b,uint32_t n){return syscall3(SYS_FILE_READ,fd,(uint32_t)b,n);}uint32_t sys_file_write(uint32_t fd,const void*b,uint32_t n){return syscall3(SYS_FILE_WRITE,fd,(uint32_t)b,n);}uint32_t sys_file_close(uint32_t fd){return syscall3(SYS_FILE_CLOSE,fd,0,0);}

#define DECL_ISR(n) extern void isr##n(void)
DECL_ISR(0);DECL_ISR(1);DECL_ISR(2);DECL_ISR(3);DECL_ISR(4);DECL_ISR(5);DECL_ISR(6);DECL_ISR(7);DECL_ISR(8);DECL_ISR(9);DECL_ISR(10);DECL_ISR(11);DECL_ISR(12);DECL_ISR(13);DECL_ISR(14);DECL_ISR(15);DECL_ISR(16);DECL_ISR(17);DECL_ISR(18);DECL_ISR(19);DECL_ISR(20);DECL_ISR(21);DECL_ISR(22);DECL_ISR(23);DECL_ISR(24);DECL_ISR(25);DECL_ISR(26);DECL_ISR(27);DECL_ISR(28);DECL_ISR(29);DECL_ISR(30);DECL_ISR(31);DECL_ISR(32);DECL_ISR(33);DECL_ISR(34);DECL_ISR(35);DECL_ISR(36);DECL_ISR(37);DECL_ISR(38);DECL_ISR(39);DECL_ISR(40);DECL_ISR(41);DECL_ISR(42);DECL_ISR(43);DECL_ISR(44);DECL_ISR(45);DECL_ISR(46);DECL_ISR(47);DECL_ISR(128);
static void idt_init(void){uint32_t i;void(*v[48])(void)={isr0,isr1,isr2,isr3,isr4,isr5,isr6,isr7,isr8,isr9,isr10,isr11,isr12,isr13,isr14,isr15,isr16,isr17,isr18,isr19,isr20,isr21,isr22,isr23,isr24,isr25,isr26,isr27,isr28,isr29,isr30,isr31,isr32,isr33,isr34,isr35,isr36,isr37,isr38,isr39,isr40,isr41,isr42,isr43,isr44,isr45,isr46,isr47};struct idtr r;for(i=0;i<48;i++)idt_set((uint8_t)i,v[i],0);idt_set(0x80,isr128,1);r.limit=sizeof(idt)-1;r.base=(uint32_t)idt;__asm__ volatile("lidtl %0"::"m"(r));}

static int user_range(uint32_t p,uint32_t n){
    uint32_t end;
    if(p>=0x00400000u)return 0;
    if(n>0x00400000u-p)return 0;
    end=p+n;
    if(n==0)return 1;
    /* User memory is limited to the explicitly mapped shell/process areas. */
    if(p>=(uint32_t)__user_text_start&&end<=(uint32_t)__user_text_end)return 1;
    if(p>=(uint32_t)__user_rodata_start&&end<=(uint32_t)__user_rodata_end)return 1;
    if(p>=(uint32_t)__user_data_start&&end<=(uint32_t)__user_data_end)return 1;
    if(sched_active&&p>=EXEC_LOAD_ADDR&&end<=sched_tasks[sched_current].image_end)return 1;
    if(rt_background_active){uint32_t i;for(i=0;i<RT_MAX_TASKS;i++)if(rt_tasks[i].state!=TASK_STOPPED&&rt_tasks[i].state!=TASK_EXIT&&p>=EXEC_LOAD_ADDR&&end<=rt_tasks[i].image_end)return 1;}
    if(p>=EXEC_LOAD_ADDR&&end<=exe_end&&exe_active)return 1;
    if(p>=USER_STACK_PAGE&&end<=USER_STACK_PAGE+0x1000u)return 1;
    if(p>=MT_STACK_PAGE&&end<=MT_STACK_PAGE+0x1000u&&(sched_active||rt_background_active))return 1;
    if(p>=EXEC_STACK_PAGE&&end<=EXEC_STACK_PAGE+EXEC_STACK_SIZE&&exe_active)return 1;
    return 0;
}
static int user_rw_range(uint32_t p,uint32_t n){
    uint32_t end;
    if(n>0x1000u)return 0;
    end=p+n;
    if(p>=(uint32_t)__user_data_start&&end<=(uint32_t)__user_data_end)return 1;
    if(p>=USER_STACK_PAGE&&end<=USER_STACK_PAGE+0x1000u)return 1;
    if((sched_active||rt_background_active)&&p>=MT_STACK_PAGE&&end<=MT_STACK_PAGE+0x1000u)return 1;
    if(sched_active&&p>=EXEC_LOAD_ADDR&&end<=sched_tasks[sched_current].image_end)return 1;
    if(rt_background_active){uint32_t i;for(i=0;i<RT_MAX_TASKS;i++)if(rt_tasks[i].state!=TASK_STOPPED&&rt_tasks[i].state!=TASK_EXIT&&p>=EXEC_LOAD_ADDR&&end<=rt_tasks[i].image_end)return 1;}
    if(exe_active&&p>=EXEC_STACK_PAGE&&end<=EXEC_STACK_PAGE+EXEC_STACK_SIZE)return 1;
    if(exe_active&&p>=EXEC_LOAD_ADDR&&end<=exe_end)return 1;
    if(exe_active&&video_user_mapped&&p>=0x000a0000u&&end<=0x000b0000u)return 1;
    return 0;
}
static int user_cstr(uint32_t p){uint32_t i;if(!user_range(p,1))return 0;for(i=0;i<128u;i++){if(!user_range(p+i,1))return 0;if(((const char*)p)[i]==0)return 1;}return 0;}
static uint32_t read_cr3(void){uint32_t v;__asm__ volatile("movl %%cr3,%0":"=r"(v));return v;}
static void load_cr3(uint32_t v){__asm__ volatile("movl %0,%%cr3"::"r"(v):"memory");}
static uint32_t boot_memory_size_value(void){return *(volatile uint32_t*)0xB8FF0u;}
static void sched_load_initial(struct sched_task*t,uint32_t entry,uint32_t esp,uint32_t cr3,uint32_t id){
    mem_set(t,0,sizeof(*t));
    t->cr3=cr3;
    t->state=TASK_CREATE;
    t->id=id;
    /* A scheduler-created Ring-3 context must contain valid user data
       segments as well as CS/SS.  The interrupt frame restores GS/FS/ES/DS
       before IRET, so leaving words 8..11 at zero makes the first data access
       in a standalone EXECMT image fault with #GP before SYS_CONSOLE_WRITE. */
    t->words[8]=0x23u;
    t->words[9]=0x23u;
    t->words[10]=0x23u;
    t->words[11]=0x23u;
    t->words[14]=entry;
    t->words[15]=0x1bu;
    t->words[16]=0x202u;
    t->words[17]=esp;
    t->words[18]=0x23u;
}
/* FIX60I: EXECMT is detached/background.  Scheduler transitions must never
   write asynchronously into the foreground shell console.  State remains
   observable through MTSTATUS/MTSTAT/PS instead of console diagnostics. */
static void sched_announce(struct sched_task*t){
    if(t->announced)return;
    t->announced=1;
}
static int sched_pick_next(void){
    uint32_t i;
    for(i=1u;i<=SCHED_TASKS;i++){
        uint32_t n=(sched_current+i)%SCHED_TASKS;
        if(sched_tasks[n].state==TASK_READY)return (int)n;
    }
    return -1;
}
static int sched_pick_any_ready(void){
    uint32_t i;
    for(i=0;i<SCHED_TASKS;i++){
        uint32_t n=(sched_current+i)%SCHED_TASKS;
        if(sched_tasks[n].state==TASK_READY)return (int)n;
    }
    return -1;
}
static int task_load_one(const char*name,uint32_t id,uint32_t is_rt,uint32_t*entry_out,uint32_t*image_end_out){
    struct exe_header h;
    int fd,n;
    uint32_t image_end,bss_end,file_size;
    uint32_t saved_cr3=read_cr3();
    if((is_rt?(id>=RT_MAX_TASKS):(id>=SCHED_TASKS))||!name||!name[0])return EXE_ERR_NAME;
    fd=fat_open_executable(name);
    if(fd<0)return EXE_ERR_OPEN;
    file_size=handles[fd].size;
    if(file_size<sizeof(h)){fat_close(fd);return EXE_ERR_HEADER;}
    n=fat_read_file_fd(fd,&h,sizeof(h));
    if(n!=(int)sizeof(h)){fat_close(fd);return EXE_ERR_HEADER;}
    if(h.magic!=EXE_MAGIC){fat_close(fd);return EXE_ERR_MAGIC;}
    if(h.image_size==0||h.image_size>EXEC_MAX_SIZE){fat_close(fd);return EXE_ERR_IMAGE_SIZE;}
    if(h.image_size>file_size-sizeof(h)||h.image_size!=file_size-sizeof(h)){fat_close(fd);return EXE_ERR_TRAILING;}
    if(h.entry<EXEC_LOAD_ADDR||h.entry>=EXEC_LOAD_ADDR+h.image_size){fat_close(fd);return EXE_ERR_ENTRY;}
    image_end=EXEC_LOAD_ADDR+h.image_size;
    if(h.bss_size>EXEC_MAX_SIZE-h.image_size){fat_close(fd);return EXE_ERR_BSS_SIZE;}
    bss_end=image_end+h.bss_size;
    /* The task page table maps 0x00100000 to this task's private physical
       image slot.  FAT I/O itself uses only kernel-visible low memory, so it
       is safe to switch CR3 for the actual image transfer. */
    load_cr3(is_rt?(uint32_t)rt_page_directory[id]:(uint32_t)mt_page_directory[id]);
    n=fat_read_file_fd(fd,(void*)EXEC_LOAD_ADDR,h.image_size);
    if(n==(int)h.image_size)mem_set((void*)image_end,0,h.bss_size);
    load_cr3(saved_cr3);
    fat_close(fd);
    if(n!=(int)h.image_size)return EXE_ERR_IMAGE_READ;
    if(bss_end<EXEC_LOAD_ADDR+0x1000u)bss_end=EXEC_LOAD_ADDR+0x1000u;
    if(is_rt)paging_set_rt_image_user(id,EXEC_LOAD_ADDR,bss_end);else paging_set_task_image_user(id,EXEC_LOAD_ADDR,bss_end);
    /* The caller initializes the task record after loading.  Do not rely on
       sched_tasks[id].image_end surviving sched_load_initial(), because that
       function clears the whole record.  Return the validated user-image end
       explicitly so SYS_CONSOLE_WRITE can validate a Ring-3 buffer immediately. */
    *image_end_out=bss_end;
    *entry_out=h.entry;
    return EXE_OK;
}
/* RT launcher: load one ordinary EXE1 into an isolated scheduler slot while
   keeping the shell as the foreground context. RT policy fields are stored
   in the same task record used by release/deadline/priority scheduling. */
struct rt_start_request {
    char name[QUEUE_NAME_SIZE];
    uint32_t period_ms;
    uint32_t deadline_ms;
    uint32_t priority;
};

/* Stage 3 RT manager.
 *
 * The FIX2 detached-RT context model remains the base. Stage 3 adds several
 * independent RT tasks while preserving the FIX2 invariant that rt_task_id
 * identifies the task whose CR3/context is currently installed for RT code.
 * The shell remains a separate foreground context captured by SYS_CONSOLE_READ.
 * Stage 5.1 adds an explicit absolute release tick for the current job and
 * keeps deadline_tick anchored to that release rather than dispatch time.
 */
/* Stage 5.1: a job keeps an explicit absolute release tick. The deadline
   is anchored to that release and never shifted by dispatch latency. */
static void rt_begin_job(struct sched_task*t,uint32_t release_tick){
    uint32_t dt=rt_deadline_ms_to_ticks(t->rt_deadline_ms);
    t->rt_job_sequence++;
    if(t->rt_job_sequence==0u)t->rt_job_sequence=1u;
    t->rt_job_release_tick=release_tick;
    t->rt_deadline_tick=rt_deadline_absolute(release_tick,dt);
    t->rt_job_active=1u;
    t->rt_job_missed=0u;
    t->rt_runtime_ticks=0u;
    t->rt_job_dispatched=0u;
    t->rt_job_dispatch_tick=0u;
    t->rt_job_dispatch_latency=0u;
    t->rt_job_cpu_ticks=0u;
}
/* Stage 6.2: record the instant at which the kernel actually serviced a
   nominal release.  The scheduled release tick is kept separately, so a
   late IRQ0/Ring-0 path is visible as positive release latency.  The interval
   is measured between successive serviced releases, not inferred from the
   requested period. */
static void rt_record_release_observation(struct sched_task*t,uint32_t release_tick,uint32_t observed_tick){
    uint32_t interval,late;
    if(t->rt_release_samples!=0u){
        interval=(uint32_t)(observed_tick-t->rt_release_last_tick);
        t->rt_release_interval_ticks=interval;
        if(t->rt_release_min_interval==0u||interval<t->rt_release_min_interval)t->rt_release_min_interval=interval;
        if(interval>t->rt_release_max_interval)t->rt_release_max_interval=interval;
    }
    late=rt_deadline_reached(observed_tick,release_tick)?(uint32_t)(observed_tick-release_tick):0u;
    t->rt_release_late_ticks=late;
    if(late>t->rt_release_max_late)t->rt_release_max_late=late;
    t->rt_release_last_tick=observed_tick;
    t->rt_release_observed_tick=observed_tick;
    t->rt_release_samples++;
}
static void rt_stats_reset(struct sched_task*t){
    uint32_t i,j,slot=(uint32_t)(t-rt_tasks);struct rt_monitor_stat*m;if(slot>=RT_MAX_TASKS)return;m=&rt_mon[slot];
    m->last_seq=0u;m->last_interval=0u;m->last_jitter=0;m->last_dispatch=0u;m->last_cpu=0u;m->last_wall=0u;m->last_miss=0u;
    /* Baseline the cumulative scheduler skip counter so RESET never turns old
       skips into new window/lifetime events on the next completed job. */
    m->last_skips=t->rt_skipped_releases;m->ring_pos=0u;m->ring_count=0u;m->lifetime_misses=0u;m->lifetime_skips=0u;m->event_pos=0u;m->event_count=0u;
    for(i=0u;i<8u;i++)for(j=0u;j<8u;j++)m->ring[i][j]=0u;
    for(i=0u;i<RT_ANOMALY_EVENTS;i++)for(j=0u;j<5u;j++)m->events[i][j]=0u;
    for(i=0u;i<RT_STATS_BUCKETS;i++)mem_set(&m->buckets[i],0,sizeof(m->buckets[i]));
}
static struct rt_stat_bucket*rt_stats_bucket(struct rt_monitor_stat*m,uint32_t now){
    uint32_t start=now-(now%RT_STATS_BUCKET_TICKS),idx=(start/RT_STATS_BUCKET_TICKS)%RT_STATS_BUCKETS;
    struct rt_stat_bucket*b=&m->buckets[idx];
    if(b->start_tick!=start){mem_set(b,0,sizeof(*b));b->start_tick=start;}
    return b;
}
static void rt_stats_event(struct rt_monitor_stat*m,uint32_t now,uint32_t seq,uint32_t miss,uint32_t skip_delta,uint32_t late){
    uint32_t i=m->event_pos;if(!miss&&!skip_delta)return;
    m->events[i][0]=now;m->events[i][1]=seq;m->events[i][2]=miss;m->events[i][3]=skip_delta;m->events[i][4]=late;
    m->event_pos=(i+1u)%RT_ANOMALY_EVENTS;if(m->event_count<RT_ANOMALY_EVENTS)m->event_count++;
}
static void rt_stats_record(struct sched_task*t,uint32_t now,uint32_t miss){
    struct rt_monitor_stat*m=&rt_mon[t->id];struct rt_stat_bucket*b=rt_stats_bucket(m,now);
    uint32_t i=m->ring_pos,interval=t->rt_release_interval_ticks,dispatch=t->rt_job_dispatch_latency,cpu=t->rt_job_cpu_ticks;
    uint32_t wall=(t->rt_job_dispatched&&rt_deadline_reached(now,t->rt_job_dispatch_tick))?(uint32_t)(now-t->rt_job_dispatch_tick):0u,skips=t->rt_skipped_releases;
    uint32_t skip_delta=(uint32_t)(skips-m->last_skips),late=rt_deadline_before(t->rt_deadline_tick,now)?(uint32_t)(now-t->rt_deadline_tick):0u;
    int32_t jitter=(int32_t)interval-(int32_t)t->rt_period_ticks;
    b->jobs++;b->interval_sum+=interval;b->jitter_sum+=jitter;b->dispatch_sum+=dispatch;b->cpu_sum+=cpu;b->wall_sum+=wall;b->misses+=miss;b->skips+=skip_delta;
    if(b->jobs==1u){b->interval_min=interval;b->interval_max=interval;b->jitter_min=jitter;b->jitter_max=jitter;b->dispatch_min=dispatch;b->dispatch_max=dispatch;b->cpu_min=cpu;b->cpu_max=cpu;b->wall_min=wall;b->wall_max=wall;}
    else{if(interval<b->interval_min)b->interval_min=interval;if(interval>b->interval_max)b->interval_max=interval;if(jitter<b->jitter_min)b->jitter_min=jitter;if(jitter>b->jitter_max)b->jitter_max=jitter;if(dispatch<b->dispatch_min)b->dispatch_min=dispatch;if(dispatch>b->dispatch_max)b->dispatch_max=dispatch;if(cpu<b->cpu_min)b->cpu_min=cpu;if(cpu>b->cpu_max)b->cpu_max=cpu;if(wall<b->wall_min)b->wall_min=wall;if(wall>b->wall_max)b->wall_max=wall;}
    m->lifetime_misses=rt_sat_add(m->lifetime_misses,miss);m->lifetime_skips=rt_sat_add(m->lifetime_skips,skip_delta);
    rt_stats_event(m,now,t->rt_job_sequence,miss,skip_delta,late);
    m->last_seq=t->rt_job_sequence;m->last_interval=interval;m->last_jitter=jitter;m->last_dispatch=dispatch;m->last_cpu=cpu;m->last_wall=wall;m->last_miss=miss;m->last_skips=skips;
    /* Keep the existing last-eight-job DUMP as a short diagnostic trace. */
    m->ring[i][0]=t->rt_job_sequence;m->ring[i][1]=interval;m->ring[i][2]=(uint32_t)jitter;m->ring[i][3]=dispatch;m->ring[i][4]=cpu;m->ring[i][5]=wall;m->ring[i][6]=m->lifetime_misses;m->ring[i][7]=m->lifetime_skips;
    m->ring_pos=(i+1u)%8u;if(m->ring_count<8u)m->ring_count++;
}
static void rt_stats_window(struct rt_monitor_stat*m,uint32_t now,uint32_t*q){
    uint32_t i,age;int have=0;mem_set(q,0,18u*4u);
    for(i=0u;i<RT_STATS_BUCKETS;i++){struct rt_stat_bucket*b=&m->buckets[i];if(!b->jobs)continue;age=(uint32_t)(now-b->start_tick);if(age>=RT_STATS_BUCKET_TICKS*RT_STATS_BUCKETS)continue;
        q[0]+=b->jobs;q[1]+=b->interval_sum;q[4]=(uint32_t)((int32_t)q[4]+b->jitter_sum);q[7]+=b->dispatch_sum;q[10]+=b->cpu_sum;q[13]+=b->wall_sum;q[16]+=b->misses;q[17]+=b->skips;
        if(!have){q[2]=b->interval_min;q[3]=b->interval_max;q[5]=(uint32_t)b->jitter_min;q[6]=(uint32_t)b->jitter_max;q[8]=b->dispatch_min;q[9]=b->dispatch_max;q[11]=b->cpu_min;q[12]=b->cpu_max;q[14]=b->wall_min;q[15]=b->wall_max;have=1;}
        else{if(b->interval_min<q[2])q[2]=b->interval_min;if(b->interval_max>q[3])q[3]=b->interval_max;if(b->jitter_min<(int32_t)q[5])q[5]=(uint32_t)b->jitter_min;if(b->jitter_max>(int32_t)q[6])q[6]=(uint32_t)b->jitter_max;if(b->dispatch_min<q[8])q[8]=b->dispatch_min;if(b->dispatch_max>q[9])q[9]=b->dispatch_max;if(b->cpu_min<q[11])q[11]=b->cpu_min;if(b->cpu_max>q[12])q[12]=b->cpu_max;if(b->wall_min<q[14])q[14]=b->wall_min;if(b->wall_max>q[15])q[15]=b->wall_max;}
    }
}
static uint32_t rt_advance_release(struct sched_task*t,uint32_t now){
    uint32_t pt=rt_deadline_ms_to_ticks(t->rt_period_ms);
    uint32_t next=t->rt_next_release_tick+pt;
    uint32_t skipped=0u;
    next=rt_next_release_after(next,now,pt,&skipped);
    t->rt_next_release_tick=next;
    t->rt_skipped_releases+=skipped;
    return skipped;
}
static uint32_t rt_recount_active(void);
static uint32_t rt_stop_slot(uint32_t slot){
    if(slot>=RT_MAX_TASKS)return 0u;
    if(rt_tasks[slot].rt_period_ms==0u)return 0u;
    if(rt_tasks[slot].state==TASK_STOPPED||rt_tasks[slot].state==TASK_EXIT)return 0u;
    /* The stop request comes from the Ring-3 shell, so the target RT task
       cannot be the currently executing context of this syscall. Marking the
       saved task STOPPED is therefore atomic with respect to the scheduler. */
    process_handles_close_pid(rt_tasks[slot].process_pid);data_readers_close_pid(rt_tasks[slot].process_pid);
    console_background_done(PROCESS_TYPE_RT,slot);
    rt_tasks[slot].state=TASK_STOPPED;
    rt_tasks[slot].rt_job_active=0u;
    rt_tasks[slot].rt_job_missed=0u;
    rt_data_clear(slot);
    rt_stop_requested&=~(1u<<slot);
    return 1u;
}
static uint32_t rt_stop_tasks(uint32_t target){
    uint32_t i,n=0u;
    if(target==RT_STOP_ALL){
        for(i=0u;i<RT_MAX_TASKS;i++)n+=rt_stop_slot(i);
    }else n=rt_stop_slot(target);
    rt_recount_active();
    if(rt_task_count==0u)rt_task_id=0u;
    return n;
}
static uint32_t rt_recount_active(void){
    uint32_t i,count=0u;
    for(i=0;i<RT_MAX_TASKS;i++){
        if((rt_tasks[i].state==TASK_READY||rt_tasks[i].state==TASK_RUNNING||rt_tasks[i].state==TASK_BLOCKED)&&
           rt_tasks[i].rt_period_ms!=0u)count++;
    }
    rt_task_count=count;
    rt_background_active=(count!=0u)?1u:0u;
    return count;
}
static int rt_find_current(void){
    struct process_identity id=process_current_identity();
    return id.type==PROCESS_TYPE_RT?(int)id.slot:-1;
}
static int rt_pick_ready_class_mask(uint32_t missed_class,uint32_t exclude_mask){
    uint32_t i;
    int best=-1;
    for(i=0;i<RT_MAX_TASKS;i++){
        struct sched_task*t=&rt_tasks[i];
        int pc;
        if(exclude_mask&(1u<<i))continue;
        if(t->state!=TASK_READY||!t->rt_job_active)continue;
        /* Deadline-missed jobs remain a best-effort class, but they must not
           disappear from a mixed-priority sweep.  Callers first ask for a
           normal job, then fall back to MISSED using the same exclusion mask. */
        if((t->rt_job_missed?1u:0u)!=missed_class)continue;
        if(best<0)best=(int)i;
        else {
            pc=rt_priority_compare(t->rt_priority,rt_tasks[best].rt_priority);
            if(pc>0||
               (pc==0&&(rt_deadline_before(t->rt_deadline_tick,rt_tasks[best].rt_deadline_tick)||
                (t->rt_deadline_tick==rt_tasks[best].rt_deadline_tick&&i<(uint32_t)best))))
                best=(int)i;
        }
    }
    return best;
}
static int rt_pick_ready_class(uint32_t missed_class){return rt_pick_ready_class_mask(missed_class,0u);}
static int rt_pick_ready(void){return rt_pick_ready_class_mask(0u,0u);}
static int rt_pick_ready_missed(void){return rt_pick_ready_class_mask(1u,0u);}
static int rt_pick_ready_sweep(void){int n=rt_pick_ready_class_mask(0u,rt_burst_done_mask);if(n<0)n=rt_pick_ready_class_mask(1u,rt_burst_done_mask);return n;}
static void rt_release_jobs(uint32_t now){
    uint32_t i;
    for(i=0;i<RT_MAX_TASKS;i++){
        struct sched_task*t=&rt_tasks[i];
        if(t->rt_period_ms==0u||t->state==TASK_STOPPED||t->state==TASK_EXIT)continue;
        if(!t->rt_job_active&&rt_deadline_reached(now,t->rt_next_release_tick)){
            uint32_t release=t->rt_next_release_tick;
            rt_record_release_observation(t,release,now);
            rt_begin_job(t,release);
            if(rt_deadline_before(t->rt_deadline_tick,now)){
                /* The release was serviced strictly after its deadline, too late even to execute the job.
                   Count this release as an actual deadline miss, then skip
                   any additional stale releases without creating a backlog. */
                t->rt_missed_deadlines++;
                t->rt_job_active=0u;
                rt_advance_release(t,now);
                t->rt_deadline_tick=rt_deadline_absolute(t->rt_next_release_tick,rt_deadline_ms_to_ticks(t->rt_deadline_ms));
                if(t->rt_next_release_tick==now){
                    rt_begin_job(t,t->rt_next_release_tick);
                    t->state=TASK_READY;
                }else t->state=TASK_BLOCKED;
            }else t->state=TASK_READY;
        }
        if(t->rt_job_active&&rt_deadline_before(t->rt_deadline_tick,now)){
            /* Stage 5.1: reaching the absolute deadline does not change the
               identity of the current job. Keep release/deadline immutable
               until the application completes the job with SYS_RT_WAIT; only
               record the miss. Deadline enforcement is deliberately deferred
               to a later stage. */
            if(!t->rt_job_missed){
                t->rt_job_missed=1u;
                t->rt_missed_deadlines++;
            }
        }
    }
}
static int rt_start_task(uint32_t p,struct frame*f){
    struct rt_start_request req;
    char name[QUEUE_NAME_SIZE];
    uint32_t n,entry,image_end,id;
    int r,slot=-1;
    uint32_t caller_cr3;
    const char *src;
    if(!user_range(p,sizeof(req)))return EXE_ERR_NAME;
    mem_copy(&req,(const void*)p,sizeof(req));
    src=req.name;
    if((f->cs&3u)==0u)return EXE_ERR_BUSY;
    if(queue_active)return EXE_ERR_BUSY;
    if(!exe_active)return EXE_ERR_BUSY;
    rt_recount_active();
    n=0;while(n<QUEUE_NAME_SIZE&&req.name[n])n++;
    if(n==0u||n>=QUEUE_NAME_SIZE)return EXE_ERR_NAME;
    if(req.period_ms==0u||req.deadline_ms==0u||req.deadline_ms>req.period_ms)return EXE_ERR_NAME;
    if(req.priority>255u)return EXE_ERR_NAME;
    while(n>0u&&req.name[n-1u]==' ')n--;
    if(n==0u)return EXE_ERR_NAME;
    n=0;while(n<QUEUE_NAME_SIZE-1u&&src[n]){name[n]=src[n];n++;}
    if(n==0u)return EXE_ERR_NAME;
    name[n]=0;
    for(id=0;id<RT_MAX_TASKS;id++){
        if(rt_tasks[id].state==TASK_CREATE||rt_tasks[id].state==TASK_STOPPED||rt_tasks[id].state==TASK_EXIT){slot=(int)id;break;}
    }
    if(slot<0)return EXE_ERR_BUSY;
    id=(uint32_t)slot;
    if(boot_memory_size_value()<RT_IMAGE_BASE_PHYS+((id+1u)*EXECMT_IMAGE_SLOT_SIZE))return EXE_ERR_MEMORY;
    caller_cr3=read_cr3();
    mem_set((void*)rt_stack_phys(id),0,0x1000u);
    mem_set(&rt_tasks[id],0,sizeof(rt_tasks[id]));
    rt_tasks[id].state=TASK_STOPPED;rt_tasks[id].id=id;rt_tasks[id].cr3=(uint32_t)rt_page_directory[id];
    paging_prepare_rt_task(id);
    r=task_load_one(name,id,1u,&entry,&image_end);
    if(r!=EXE_OK){load_cr3(caller_cr3);return r;}
    sched_load_initial(&rt_tasks[id],entry,MT_STACK_TOP,(uint32_t)rt_page_directory[id],id);
    rt_tasks[id].process_pid=process_alloc_pid();
    rt_tasks[id].image_end=image_end;
    mem_copy(rt_tasks[id].name,name,QUEUE_NAME_SIZE);
    rt_tasks[id].rt_period_ms=req.period_ms;rt_tasks[id].rt_deadline_ms=req.deadline_ms;rt_tasks[id].rt_priority=req.priority;
    rt_tasks[id].rt_period_ticks=rt_deadline_ms_to_ticks(req.period_ms);
    rt_tasks[id].rt_next_release_tick=rt_time_ticks;
    rt_tasks[id].rt_job_sequence=0u;
    rt_tasks[id].rt_skipped_releases=0u;
    rt_tasks[id].rt_missed_deadlines=0u;
    rt_tasks[id].rt_release_observed_tick=rt_time_ticks;
    rt_tasks[id].rt_release_last_tick=rt_time_ticks;
    rt_tasks[id].rt_release_interval_ticks=0u;
    rt_tasks[id].rt_release_min_interval=0u;
    rt_tasks[id].rt_release_max_interval=0u;
    rt_tasks[id].rt_release_samples=1u;
    rt_tasks[id].rt_release_late_ticks=0u;
    rt_tasks[id].rt_release_max_late=0u;
    rt_tasks[id].rt_job_dispatch_tick=0u;
    rt_tasks[id].rt_job_dispatch_latency=0u;
    rt_tasks[id].rt_job_dispatched=0u;
    rt_tasks[id].rt_job_cpu_ticks=0u;
    rt_stats_reset(&rt_tasks[id]);rt_mon[id].diag_verbose=0u;rt_data_clear(id);
    rt_begin_job(&rt_tasks[id],rt_time_ticks);
    rt_tasks[id].rt_runtime_budget=1u+
        (rt_tasks[id].rt_period_ticks/255u)*req.priority+
        ((rt_tasks[id].rt_period_ticks%255u)*req.priority)/255u;
    if(rt_tasks[id].rt_runtime_budget>rt_tasks[id].rt_period_ticks)rt_tasks[id].rt_runtime_budget=rt_tasks[id].rt_period_ticks;
    if(rt_tasks[id].rt_runtime_budget==0u)rt_tasks[id].rt_runtime_budget=1u;
    rt_tasks[id].rt_runtime_ticks=0u;rt_tasks[id].rt_dispatch_seq=0u;rt_tasks[id].state=TASK_READY;
    rt_stop_requested&=~(1u<<id);rt_recount_active();
    if(rt_task_count==1u){
        rt_task_id=id;rt_started_tick=rt_time_ticks;rt_return_cr3=read_cr3();
        mem_copy(rt_shell_frame,saved_user_frame,MT_CONTEXT_WORDS*4u);rt_shell_frame_valid=1u;rt_shell_waiting=0u;rt_shell_hold=0u;
    }
    return EXE_OK;
}
static void rt_record_dispatch(int next){
    if(next<0)return;
    rt_dispatch_sequence++;
    if(rt_dispatch_sequence==0u)rt_dispatch_sequence=1u;
    rt_tasks[next].rt_dispatch_seq=rt_dispatch_sequence;
}
static void rt_switch_to_common(struct frame*f,int next,uint32_t capture_shell){
    int from_rt;
    if(next<0)return;
    from_rt=rt_find_current();
    if(from_rt>=0)rt_chain_count++;else{rt_chain_count=1u;rt_burst_done_mask=0u;}
    /* The shell is not always blocked in SYS_CONSOLE_READ: rtstat watch and
       other foreground commands continue running while detached RT jobs are
       active.  Before switching away from a Ring-3 shell frame, preserve that
       exact frame so SYS_RT_WAIT can return to the interrupted shell command.
       Without this, a task that called SYS_RT_WAIT while watch was foreground
       could resume its own BLOCKED context instead of yielding back to shell.

       FIX35B: a known RT owner may already have been changed to TASK_EXIT
       before this hand-off.  In that window rt_find_current() deliberately no
       longer classifies its CR3 as a live RT task.  Such a frame is NOT a
       shell frame and must never replace rt_shell_frame.  The caller therefore
       suppresses shell capture for RT-exit/fault hand-offs. */
    if(capture_shell&&(f->cs&3u)!=0u&&rt_find_current()<0&&rt_background_active){
        rt_return_cr3=read_cr3();
        mem_copy(rt_shell_frame,(const void*)f,MT_CONTEXT_WORDS*4u);
        rt_shell_frame_valid=1u;
    }
    rt_task_id=(uint32_t)next;rt_tasks[next].state=TASK_RUNNING;rt_tasks[next].switches++;
    if(rt_tasks[next].rt_job_active&&!rt_tasks[next].rt_job_dispatched){
        rt_tasks[next].rt_job_dispatch_tick=rt_time_ticks;
        rt_tasks[next].rt_job_dispatch_latency=(uint32_t)(rt_time_ticks-rt_tasks[next].rt_job_release_tick);
        rt_tasks[next].rt_job_dispatched=1u;
    }
    rt_record_dispatch(next);
    load_cr3(rt_tasks[next].cr3);mem_copy((void*)f,rt_tasks[next].words,MT_CONTEXT_WORDS*4u);
}
static void rt_switch_to(struct frame*f,int next){rt_switch_to_common(f,next,1u);}
static void rt_switch_from_known_rt(struct frame*f,int next){rt_switch_to_common(f,next,0u);}
static void rt_switch_to_shell(struct frame*f){
    int next;
    if(!rt_shell_frame_valid)return;
    rt_chain_count=0u;
    rt_burst_done_mask=0u;
    /* FIX60ZEF: an MT task that was granted only the sub-period slack must
       never become the owner of the next foreground interval.  If RT
       interrupted that slack task, RT completion returns directly to the
       exact saved foreground continuation. */
    if(rt_mt_slack_active){
        rt_mt_slack_active=0u;
        rt_shell_waiting=0u;
        mt_quantum_used=0u;
        load_cr3(sched_saved_shell_cr3);
        mem_copy((void*)f,sched_saved_shell,MT_CONTEXT_WORDS*4u);
        return;
    }
    /* If RT interrupted an ordinary MT quantum, resume that exact task/context. */
    if(mt_current_running()||
       (sched_active&&sched_current<SCHED_TASKS&&rt_return_cr3==sched_tasks[sched_current].cr3)){
        load_cr3(rt_return_cr3);
        mem_copy((void*)f,rt_shell_frame,MT_CONTEXT_WORDS*4u);
        return;
    }
    /* A foreground EXE (notably RTD while F10 is launching tasks) owns the
       shell address space.  Never replace it with an MT frame. */
    if(exe_active){
        load_cr3(rt_return_cr3);
        mem_copy((void*)f,rt_shell_frame,MT_CONTEXT_WORDS*4u);
        return;
    }
    /* FIX60ZEF: idle console wait is the primary safe slack boundary.
       SYS_CONSOLE_READ no longer performs an RT context switch itself; it
       returns EAX=2 to Ring3 and marks the shell as idle.  IRQ0 captures that
       real Ring3 continuation, RT runs first, then one MT task may use the
       remainder of the interval.  The next PIT is guaranteed to return shell. */
    if(rt_shell_waiting){
        rt_shell_hold=0u;
        /* The saved frame came from a genuine Ring3 PIT interruption, often
           from readline()'s small idle spin.  Preserve every general register;
           unlike the old direct-CONSOLE_READ dispatch this is not an int80
           return frame, so forcing EAX would corrupt an arbitrary instruction. */
        mem_copy(sched_saved_shell,rt_shell_frame,MT_CONTEXT_WORDS*4u);
        sched_saved_shell_cr3=rt_return_cr3;
        if(sched_active&&!exe_active){
            next=sched_pick_next();if(next<0)next=sched_pick_any_ready();
            if(next>=0){
                if(process_wait_active)process_wait_mt_turn=0u;
                rt_mt_slack_active=1u;
                sched_current=(uint32_t)next;
                sched_tasks[sched_current].state=TASK_RUNNING;
                sched_tasks[sched_current].switches++;sched_switches++;
                mt_stats_dispatch(sched_current);mt_quantum_used=0u;
                load_cr3(sched_tasks[sched_current].cr3);
                mem_copy((void*)f,sched_tasks[sched_current].words,MT_CONTEXT_WORDS*4u);
                return;
            }
        }
        rt_shell_waiting=0u;
        load_cr3(rt_return_cr3);
        mem_copy((void*)f,rt_shell_frame,MT_CONTEXT_WORDS*4u);
        return;
    }
    /* Ordinary foreground commands (rtstat, ps, file commands, etc.) are
       ownership boundaries: after RT they MUST resume the same command.
       Only an explicit SYS_RT_YIELD is allowed to donate its remainder to MT. */
    if(rt_explicit_yield_active&&sched_active&&rt_return_cr3==sched_saved_shell_cr3){
        mem_copy(sched_saved_shell,rt_shell_frame,MT_CONTEXT_WORDS*4u);
        sched_saved_shell[7]=0u;
        sched_saved_shell_cr3=rt_return_cr3;
        next=sched_pick_next();if(next<0)next=sched_pick_any_ready();
        if(next>=0){
            rt_explicit_yield_active=0u;
            if(process_wait_active)process_wait_mt_turn=0u;
            rt_mt_slack_active=1u;
            sched_current=(uint32_t)next;
            sched_tasks[sched_current].state=TASK_RUNNING;
            sched_tasks[sched_current].switches++;sched_switches++;
            mt_stats_dispatch(sched_current);mt_quantum_used=0u;
            load_cr3(sched_tasks[sched_current].cr3);
            mem_copy((void*)f,sched_tasks[sched_current].words,MT_CONTEXT_WORDS*4u);
            return;
        }
        rt_explicit_yield_active=0u;
        load_cr3(rt_return_cr3);mem_copy((void*)f,rt_shell_frame,MT_CONTEXT_WORDS*4u);f->eax=0u;return;
    }
    load_cr3(rt_return_cr3);
    mem_copy((void*)f,rt_shell_frame,MT_CONTEXT_WORDS*4u);
    if(rt_explicit_yield_active){rt_explicit_yield_active=0u;f->eax=0u;}
}

static void rt_wait(struct frame*f){
    int id=rt_find_current();uint32_t now,next,dt;struct sched_task*t;
    if(id<0||(f->cs&3u)==0u){f->eax=0xffffffffu;return;}
    t=&rt_tasks[id];now=rt_time_ticks;
    if(!t->rt_job_active){f->eax=0;return;}
    if(t->rt_job_missed||rt_deadline_before(t->rt_deadline_tick,now)){
        if(!t->rt_job_missed)t->rt_missed_deadlines++;
        f->eax=1u;
    }else f->eax=0u;
    /* Determine skipped releases before recording the completed job.  Older
       builds recorded RTSTAT first and updated rt_skipped_releases after it,
       so RTSTAT/DUMP always showed skip information one completed job late. */
    dt=t->rt_period_ticks;next=t->rt_next_release_tick+dt;
    {uint32_t skipped=0u;next=rt_next_release_after(next,now,dt,&skipped);t->rt_next_release_tick=next;t->rt_skipped_releases+=skipped;}
    rt_stats_record(t,now,(t->rt_job_missed||rt_deadline_before(t->rt_deadline_tick,now))?1u:0u);
    t->rt_job_active=0u;t->rt_job_missed=0u;t->rt_runtime_ticks=0u;
    mem_copy(t->words,(const void*)f,MT_CONTEXT_WORDS*4u);
    if(next==now){
        rt_begin_job(t,next);
        t->state=TASK_READY;
    }else t->state=TASK_BLOCKED;
    /* FIX60ZED: completing a job marks this slot served for the current RT
       burst.  Even if period=10 ms makes its next release READY immediately,
       it cannot run a second completed job until every other runnable slot has
       had its opportunity or control has returned to shell/MT.  MISSED jobs
       are explicitly included as best-effort candidates instead of starving. */
    rt_burst_done_mask|=(1u<<(uint32_t)id);
    {int nextid=rt_pick_ready_sweep();
     if(nextid>=0){rt_switch_to(f,nextid);return;}}
    rt_switch_to_shell(f);
}
/* FIX35A: terminate a known RT slot without resolving the caller a second
   time.  Exception entry has already identified the owner from the loaded
   address space; re-running the generic identity resolver during fault
   teardown made containment depend on transient scheduler state. */
static void rt_exit_slot(struct frame*f,int id){
    if(id<0||(uint32_t)id>=RT_MAX_TASKS){f->eax=0xffffffffu;return;}
    mem_copy(rt_tasks[id].words,(const void*)f,MT_CONTEXT_WORDS*4u);
    if(rt_tasks[id].exit_reason==PROCESS_EXIT_NONE){rt_tasks[id].exit_reason=PROCESS_EXIT_NORMAL;rt_tasks[id].exit_status=f->ebx;}
    process_result_store(rt_tasks[id].process_pid,PROCESS_TYPE_RT,rt_tasks[id].exit_reason,rt_tasks[id].exit_status,rt_tasks[id].fault_vector,rt_tasks[id].fault_error,rt_tasks[id].fault_eip,rt_tasks[id].fault_cr2);
    process_handles_close_pid(rt_tasks[id].process_pid);data_readers_close_pid(rt_tasks[id].process_pid);
    console_background_done(PROCESS_TYPE_RT,(uint32_t)id);
    rt_tasks[id].state=TASK_EXIT;rt_data_clear((uint32_t)id);rt_stop_requested&=~(1u<<id);
    rt_recount_active();
    if(rt_task_count==0u){
        rt_background_active=0u;load_cr3(rt_return_cr3);
        if(rt_shell_frame_valid)mem_copy((void*)f,rt_shell_frame,MT_CONTEXT_WORDS*4u);
        rt_shell_frame_valid=0u;rt_shell_waiting=0u;rt_shell_hold=0u;rt_mt_slack_active=0u;rt_explicit_yield_active=0u;rt_chain_count=0u;rt_burst_done_mask=0u;rt_task_id=0u;
        f->eax=3u;return;
    }
    {int next=rt_pick_ready();if(next<0)next=rt_pick_ready_missed();if(next>=0){rt_switch_from_known_rt(f,next);return;}}
    rt_switch_to_shell(f);
}
static void rt_exit(struct frame*f){
    int id=rt_find_current();
    if(id<0){f->eax=0xffffffffu;return;}
    rt_exit_slot(f,id);
}

#define SPAWN_ARGC_MAX 8u
#define SPAWN_ARG_SIZE 32u
struct process_spawn_request { char name[QUEUE_NAME_SIZE]; uint32_t argc; char argv[SPAWN_ARGC_MAX][SPAWN_ARG_SIZE]; };
/* FIX37: detached process creation deliberately reuses the proven MT address-space
   and 20-ms scheduling class.  It does not alter RT selection or MT quantum policy. */
static int process_spawn(uint32_t p,struct frame*f,uint32_t*pid_out){
    struct process_spawn_request req; uint32_t i,j,slot=SCHED_TASKS,entry,image_end,saved_cr3,argc; int r;
    if(!user_range(p,sizeof(req)))return EXE_ERR_NAME;
    mem_copy(&req,(const void*)p,sizeof(req)); req.name[QUEUE_NAME_SIZE-1u]=0;
    if(!req.name[0]||req.argc>SPAWN_ARGC_MAX)return EXE_ERR_NAME;
    for(i=0;i<req.argc;i++){for(j=0;j<SPAWN_ARG_SIZE;j++)if(req.argv[i][j]==0)break;if(j==SPAWN_ARG_SIZE)return EXE_ERR_NAME;}
    for(i=0;i<SCHED_TASKS;i++)if(sched_tasks[i].state==TASK_STOPPED||sched_tasks[i].state==TASK_EXIT||sched_tasks[i].state==TASK_CREATE){slot=i;break;}
    if(slot==SCHED_TASKS)return EXE_ERR_BUSY;
    if(boot_memory_size_value()<EXECMT_IMAGE_BASE_PHYS+((slot+1u)*EXECMT_IMAGE_SLOT_SIZE))return EXE_ERR_MEMORY;
    if(!mt_session_active){
        mt_data_clear_all();mt_session_id++;mt_session_count=0u;
        mem_copy(sched_saved_shell,f,MT_CONTEXT_WORDS*4u);sched_saved_shell[7]=0u;sched_saved_shell_cr3=read_cr3();
        for(i=0;i<SCHED_TASKS;i++){mem_set((void*)sched_stack_phys(i),0,0x1000u);mem_set(&sched_tasks[i],0,sizeof(sched_tasks[i]));sched_tasks[i].state=TASK_STOPPED;sched_tasks[i].id=i;sched_tasks[i].cr3=(uint32_t)mt_page_directory[i];}
        mt_stats_reset_all();sched_current=SCHED_TASKS-1u;sched_switches=0u;mt_quantum_used=0u;mt_shell_turn=0u;mt_session_active=1u;
        slot=0u;
    }else {
        /* A previous detached task may have ended and left the MT session metadata
           alive for MTDATA.  Refresh the shell return frame before restarting an
           otherwise idle MT domain; never restore a stale earlier spawn frame. */
        if(!sched_active){mem_copy(sched_saved_shell,f,MT_CONTEXT_WORDS*4u);sched_saved_shell[7]=0u;sched_saved_shell_cr3=read_cr3();}
        mem_set((void*)sched_stack_phys(slot),0,0x1000u);
    }
    paging_prepare_execmt_task(slot);r=task_load_one(req.name,slot,0u,&entry,&image_end);if(r!=EXE_OK){
        /* FIX37A: spawn preparation is transactional.  FIX37 left a newly
           created MT session behind when image loading failed; subsequent
           spawn/execmt calls could then observe stale scheduler metadata. */
        mem_set(&sched_tasks[slot],0,sizeof(sched_tasks[slot]));
        sched_tasks[slot].state=TASK_STOPPED;sched_tasks[slot].id=slot;sched_tasks[slot].cr3=(uint32_t)mt_page_directory[slot];
        if(mt_session_count==0u){mt_session_active=0u;sched_active=0u;mt_quantum_used=0u;mt_shell_turn=0u;}
        return r;
    }
    sched_load_initial(&sched_tasks[slot],entry,MT_STACK_TOP,(uint32_t)mt_page_directory[slot],slot);sched_tasks[slot].image_end=image_end;
    for(i=0;i<QUEUE_NAME_SIZE-1u&&req.name[i];i++)sched_tasks[slot].name[i]=req.name[i];sched_tasks[slot].name[i]=0;
    sched_tasks[slot].process_pid=process_alloc_pid(); argc=req.argc;
    /* ABI: EBX=argc, ECX=argv. argv points to a NULL-terminated vector in the
       private Ring-3 stack page; argv[0] is the executable name. */
    saved_cr3=read_cr3();load_cr3((uint32_t)mt_page_directory[slot]);
    {uint32_t vec=MT_STACK_PAGE+0x100u, str=MT_STACK_PAGE+0x180u;uint32_t*av=(uint32_t*)vec;char*d;
     d=(char*)str;for(j=0;j<QUEUE_NAME_SIZE-1u&&req.name[j];j++)d[j]=req.name[j];d[j]=0;av[0]=str;str+=j+1u;
     for(i=0;i<argc;i++){d=(char*)str;for(j=0;j<SPAWN_ARG_SIZE-1u&&req.argv[i][j];j++)d[j]=req.argv[i][j];d[j]=0;av[i+1u]=str;str+=j+1u;}av[argc+1u]=0u;
     sched_tasks[slot].words[4]=argc+1u;sched_tasks[slot].words[6]=vec;}
    load_cr3(saved_cr3);sched_tasks[slot].state=TASK_READY;if(slot+1u>mt_session_count)mt_session_count=slot+1u;sched_active=1u;*pid_out=sched_tasks[slot].process_pid;return EXE_OK;
}

static int execmt_prepare(uint32_t p,uint32_t count,struct frame*f){
    uint32_t i,n;
    const char*src=(const char*)p;
    if(mt_session_active||sched_active||exe_active||queue_active)return EXE_ERR_BUSY;
    if(count==0u||count>EXECMT_MAX_TASKS)return EXE_ERR_NAME;
    if(boot_memory_size_value()<EXECMT_IMAGE_BASE_PHYS+(count*EXECMT_IMAGE_SLOT_SIZE))return EXE_ERR_MEMORY;
    if(!user_range(p,count*QUEUE_NAME_SIZE))return EXE_ERR_NAME;
    for(i=0;i<count;i++){
        n=0;
        while(n<QUEUE_NAME_SIZE-1u&&src[i*QUEUE_NAME_SIZE+n])n++;
        if(n==0)return EXE_ERR_NAME;
    }
    mt_data_clear_all();
    mt_session_id++; /* wrap is intentional; compare session ids only by != */
    mt_session_count=count;
    mem_copy(sched_saved_shell,f,MT_CONTEXT_WORDS*4u);
    sched_saved_shell[7]=0;
    sched_saved_shell_cr3=read_cr3();
    for(i=0;i<SCHED_TASKS;i++){
        mem_set((void*)sched_stack_phys(i),0,0x1000u);
        mem_set(&sched_tasks[i],0,sizeof(sched_tasks[i]));
        sched_tasks[i].state=TASK_STOPPED;
        sched_tasks[i].id=i;
        sched_tasks[i].cr3=(uint32_t)mt_page_directory[i];
    }
    for(i=0;i<count;i++){
        char name[QUEUE_NAME_SIZE];
        n=0;
        while(n<QUEUE_NAME_SIZE-1u&&src[i*QUEUE_NAME_SIZE+n]){name[n]=src[i*QUEUE_NAME_SIZE+n];n++;}
        name[n]=0;
        paging_prepare_execmt_task(i);
        {uint32_t entry,image_end;
        {int r=task_load_one(name,i,0u,&entry,&image_end);
        if(r!=EXE_OK){
            load_cr3(sched_saved_shell_cr3);
            mt_session_count=0u;mt_data_clear_all();
            return r;
        }}
        sched_load_initial(&sched_tasks[i],entry,MT_STACK_TOP,(uint32_t)mt_page_directory[i],i);
        sched_tasks[i].process_pid=process_alloc_pid();
        /* sched_load_initial() clears the task record, so restore the
           validated image boundary after context construction.  This boundary
           is required by the Ring-3 SYS_CONSOLE_WRITE user-range check. */
        sched_tasks[i].image_end=image_end;
        for(n=0;n<QUEUE_NAME_SIZE-1u&&name[n];n++)sched_tasks[i].name[n]=name[n];
        sched_tasks[i].name[n]=0;
        }
    }
    for(i=0;i<count;i++)sched_tasks[i].state=TASK_READY;
    mt_stats_reset_all();sched_current=SCHED_TASKS-1u;sched_switches=0;mt_quantum_used=0u;mt_shell_turn=0u;
    /* FIX60O: explicit PREPARED -> RUNNABLE boundary. Loading must return
       through SYS_EXECMT before IRQ0 may execute the new task. Interactive
       shell commits from SYS_CONSOLE_READ after printing completion + toy0>.
       Tests commit through syscall 75 using the same state transition. */
    sched_active=0u;mt_session_active=1u;
    return EXE_OK;
}
static void sched_start(struct frame*f){
    uint32_t *w=(uint32_t*)f,i;
    uint32_t e0=(uint32_t)&mt_task_a,e1=(uint32_t)&mt_task_b,e2=(uint32_t)&mt_task_c,e3=(uint32_t)&mt_task_d;
    if(sched_active||exe_active||queue_active){f->eax=0xffffffffu;return;}
    mem_copy(sched_saved_shell,w,MT_CONTEXT_WORDS*4u);
    sched_saved_shell[7]=0;
    sched_saved_shell_cr3=read_cr3();
    for(i=0;i<SCHED_TASKS;i++)mem_set((void*)sched_stack_phys(i),0,0x1000u);
    sched_load_initial(&sched_tasks[0],e0,MT_STACK_TOP,(uint32_t)mt_page_directory[0],0); sched_tasks[0].process_pid=process_alloc_pid();
    sched_load_initial(&sched_tasks[1],e1,MT_STACK_TOP,(uint32_t)mt_page_directory[1],1); sched_tasks[1].process_pid=process_alloc_pid();
    sched_load_initial(&sched_tasks[2],e2,MT_STACK_TOP,(uint32_t)mt_page_directory[2],2); sched_tasks[2].process_pid=process_alloc_pid();
    sched_load_initial(&sched_tasks[3],e3,MT_STACK_TOP,(uint32_t)mt_page_directory[3],3); sched_tasks[3].process_pid=process_alloc_pid();
    for(i=4;i<SCHED_TASKS;i++){mem_set(&sched_tasks[i],0,sizeof(sched_tasks[i]));sched_tasks[i].state=TASK_STOPPED;sched_tasks[i].id=i;sched_tasks[i].cr3=(uint32_t)mt_page_directory[i];}
    for(i=0;i<4u;i++)sched_tasks[i].state=TASK_READY;
    mt_stats_reset_all();
    sched_current=0;
    sched_tasks[0].state=TASK_RUNNING;
    mt_stats_dispatch(0u);
    sched_switches=0;
    sched_active=1;
    load_cr3(sched_tasks[0].cr3);
    mem_copy(w,sched_tasks[0].words,MT_CONTEXT_WORDS*4u);
}
static void sched_stop(struct frame*f){
    /* FIX28: sched_active means that an EXECMT session has runnable work; it
       does NOT mean that this syscall frame belongs to an MT task.  MTSTOP ALL
       is normally issued by the foreground shell while RT/MT are active.
       Replacing that live shell syscall frame with sched_saved_shell made the
       shell resume an older console-read frame, so the first Enter only woke
       that stale read and a second Enter was required.  Restore the saved
       shell context only when SYS_MT_STOP was actually invoked by the running
       MT context. */
    uint32_t caller_is_mt=mt_current_running()?1u:0u;
    __asm__ volatile("cli" ::: "memory");
    if(!mt_session_active&&!sched_active){f->eax=0xffffffffu;return;}
    sched_active=0;
    for(uint32_t i=0;i<SCHED_TASKS;i++){
        if(sched_tasks[i].state!=TASK_STOPPED&&sched_tasks[i].state!=TASK_EXIT&&sched_tasks[i].state!=TASK_CREATE&&sched_tasks[i].process_pid){process_result_store(sched_tasks[i].process_pid,PROCESS_TYPE_MT,PROCESS_EXIT_STOPPED,0u,0u,0u,0u,0u);sched_tasks[i].exit_reason=PROCESS_EXIT_STOPPED;sched_tasks[i].exit_status=0u;}
        process_handles_close_pid(sched_tasks[i].process_pid);data_readers_close_pid(sched_tasks[i].process_pid);process_heartbeat_clear(sched_tasks[i].process_pid);
        console_background_done(PROCESS_TYPE_MT,i);
        if(sched_tasks[i].state!=TASK_EXIT)sched_tasks[i].state=TASK_STOPPED;
    }
    mt_quantum_used=0u;mt_shell_turn=0u;rt_mt_slack_active=0u;rt_explicit_yield_active=0u;rt_shell_waiting=0u;
    /* MTSTOP ALL is the explicit end of the EXECMT session. */
    mt_session_active=0u;mt_session_count=0u;mt_data_clear_all();
    if(caller_is_mt){
        load_cr3(sched_saved_shell_cr3);
        mem_copy((uint32_t*)f,sched_saved_shell,MT_CONTEXT_WORDS*4u);
        sched_saved_shell[7]=0;
    }else f->eax=0u;
}
static void sched_dispatch_next(struct frame*f){
    int next=sched_pick_any_ready();
    if(next<0){
        sched_active=0;
        load_cr3(sched_saved_shell_cr3);
        mem_copy((void*)f,sched_saved_shell,MT_CONTEXT_WORDS*4u);
        return;
    }
    sched_current=(uint32_t)next;
    sched_tasks[sched_current].state=TASK_RUNNING;
    sched_tasks[sched_current].switches++;
    sched_switches++;
    mt_stats_dispatch(sched_current);
    load_cr3(sched_tasks[sched_current].cr3);
    sched_announce(&sched_tasks[sched_current]);
    mem_copy((void*)f,sched_tasks[sched_current].words,MT_CONTEXT_WORDS*4u);
}
static uint32_t sched_commit_prepared(void){
    int probe;
    if(!mt_session_active)return 0xffffffffu;
    if(sched_active)return 0u;
    if(exe_active||queue_active)return 0xffffffffu;
    probe=sched_pick_any_ready();
    if(probe<0)return 0xffffffffu;
    sched_active=1u;
    return 0u;
}

static void sched_irq_tick(struct frame*f){
    int next,current;uint32_t now=rt_time_ticks,cr3=read_cr3();
    if(!sched_active&&!rt_background_active)return;
    if(rt_background_active)rt_release_jobs(now);
    if((f->cs&3u)==0u)return;
    if(rt_launch_guard)return;
    {uint32_t mt_quantum_expired=0u;
    uint32_t current_is_mt=(sched_active&&sched_current<SCHED_TASKS&&cr3==sched_tasks[sched_current].cr3&&sched_tasks[sched_current].state==TASK_RUNNING)?1u:0u;
    if(current_is_mt){
        mt_stats_cpu_tick(sched_current);
        mt_quantum_used++;
        if(mt_quantum_used>=MT_QUANTUM_TICKS)mt_quantum_expired=1u;
    }
    current=rt_find_current();
    if(current>=0){
        struct sched_task*t=&rt_tasks[current];
        rt_task_id=(uint32_t)current;
        mem_copy(t->words,(const void*)f,MT_CONTEXT_WORDS*4u);
        if(t->state==TASK_RUNNING)t->state=TASK_READY;
        if(t->rt_job_active){t->rt_runtime_ticks++;t->rt_job_cpu_ticks++;}
        next=rt_pick_ready_sweep();
        if(next>=0){if(next==current){t->state=TASK_RUNNING;load_cr3(t->cr3);mem_copy((void*)f,t->words,MT_CONTEXT_WORDS*4u);}else rt_switch_to(f,next);return;}
        if(rt_shell_frame_valid){rt_switch_to_shell(f);return;}
        return;
    }
    /* RT always has first claim on a PIT.  When the interrupted context is a
       bounded MT-slack slice, preserve that MT frame as READY, run the RT
       sweep, then rt_switch_to_shell() returns the saved foreground owner. */
    if(rt_background_active){
        next=rt_pick_ready();if(next<0)next=rt_pick_ready_missed();
        if(next>=0){
            if(current_is_mt){
                mem_copy(sched_tasks[sched_current].words,(const void*)f,MT_CONTEXT_WORDS*4u);
                if(rt_mt_slack_active||mt_quantum_expired){
                    sched_tasks[sched_current].state=TASK_READY;
                    mt_quantum_used=0u;
                    if(mt_quantum_expired&&!rt_mt_slack_active)mt_shell_turn=1u;
                }
            }
            rt_switch_to(f,next);
            if(rt_mt_slack_active||mt_quantum_expired){
                rt_return_cr3=sched_saved_shell_cr3;
                mem_copy(rt_shell_frame,sched_saved_shell,MT_CONTEXT_WORDS*4u);
                rt_shell_frame_valid=1u;
            }
            return;
        }
    }
    /* No RT release on this PIT.  A bounded slack slice still ends now: its
       contract is one PIT interval maximum, not a full MT quantum. */
    if(rt_mt_slack_active&&current_is_mt){
        mem_copy(sched_tasks[sched_current].words,(const void*)f,MT_CONTEXT_WORDS*4u);
        sched_tasks[sched_current].state=TASK_READY;
        rt_mt_slack_active=0u;rt_shell_waiting=0u;mt_quantum_used=0u;
        load_cr3(sched_saved_shell_cr3);
        mem_copy((void*)f,sched_saved_shell,MT_CONTEXT_WORDS*4u);
        return;
    }
    if(mt_quantum_expired&&current_is_mt){
        mem_copy(sched_tasks[sched_current].words,(const void*)f,MT_CONTEXT_WORDS*4u);
        sched_tasks[sched_current].state=TASK_READY;mt_quantum_used=0u;mt_shell_turn=1u;
        load_cr3(sched_saved_shell_cr3);mem_copy((void*)f,sched_saved_shell,MT_CONTEXT_WORDS*4u);return;
    }
    if(exe_active&&cr3==sched_saved_shell_cr3)return;
    if(!sched_active)return;
    if(current_is_mt)return;
    if(cr3==sched_saved_shell_cr3){
        mem_copy(sched_saved_shell,(const void*)f,MT_CONTEXT_WORDS*4u);
        if(mt_shell_turn){mt_shell_turn=0u;return;}
        next=sched_pick_next();if(next<0)next=sched_pick_any_ready();
        if(next<0){sched_active=0u;return;}
        /* When the interactive shell is idle while RT is active, this MT run
           is only sub-period slack.  Bound it to the next PIT so keyboard
           ownership is returned regularly even on a tick with no RT release. */
        if(rt_background_active&&rt_shell_waiting)rt_mt_slack_active=1u;
        sched_current=(uint32_t)next;sched_tasks[sched_current].state=TASK_RUNNING;
        sched_tasks[sched_current].switches++;sched_switches++;mt_stats_dispatch(sched_current);
        mt_quantum_used=0u;load_cr3(sched_tasks[sched_current].cr3);
        mem_copy((void*)f,sched_tasks[sched_current].words,MT_CONTEXT_WORDS*4u);return;
    }
    }
}

static void sched_block(struct frame*f){
    if(!sched_active||(f->cs&3u)==0u){f->eax=0xffffffffu;return;}
    sched_tasks[sched_current].state=TASK_BLOCKED;
    f->eax=0;
    if(rt_mt_slack_active){
        rt_mt_slack_active=0u;rt_shell_waiting=0u;mt_quantum_used=0u;
        load_cr3(sched_saved_shell_cr3);mem_copy((void*)f,sched_saved_shell,MT_CONTEXT_WORDS*4u);
        return;
    }
    sched_dispatch_next(f);
}
/* FIX60J: a detached MT service may finish one unit of work long before its
   20-ms quantum boundary.  Give it an explicit cooperative return path to the
   foreground instead of forcing an always-running service (SONARDRV) to rely
   exclusively on a later PIT preemption.  The current Ring-3 frame is retained
   as READY and the most recent foreground continuation is restored atomically. */
static void sched_yield(struct frame*f){
    if(!sched_active||(f->cs&3u)==0u||sched_current>=SCHED_TASKS||read_cr3()!=sched_tasks[sched_current].cr3||sched_tasks[sched_current].state!=TASK_RUNNING){f->eax=0xffffffffu;return;}
    f->eax=0u;
    mem_copy(sched_tasks[sched_current].words,(const void*)f,MT_CONTEXT_WORDS*4u);
    sched_tasks[sched_current].state=TASK_READY;
    mt_quantum_used=0u;
    if(rt_mt_slack_active){rt_mt_slack_active=0u;rt_shell_waiting=0u;mt_shell_turn=0u;}
    else mt_shell_turn=1u;
    load_cr3(sched_saved_shell_cr3);
    mem_copy((void*)f,sched_saved_shell,MT_CONTEXT_WORDS*4u);
}
static void sched_wake(struct frame*f){
    uint32_t id=f->ebx;
    if(!sched_active||id>=SCHED_TASKS){f->eax=0xffffffffu;return;}
    /* Wake is idempotent: an IRQ0 tick may have moved the target from
       BLOCKED to READY before this syscall reaches the kernel.  In that
       race, READY already means "wake request satisfied" and must not be
       reported as an error.  A currently RUNNING target is likewise
       already runnable; waking the caller itself is rejected below. */
    if(sched_tasks[id].state==TASK_BLOCKED){
        sched_tasks[id].state=TASK_READY;
        f->eax=0;
        return;
    }
    if(sched_tasks[id].state==TASK_READY ||
       (sched_tasks[id].state==TASK_RUNNING && id!=sched_current)){
        f->eax=0;
        return;
    }
    f->eax=0xffffffffu;
}
static void sched_exit(struct frame*f){
    uint32_t i,live=0u;
    if(!sched_active||(f->cs&3u)==0u){f->eax=0xffffffffu;return;}
    if(sched_tasks[sched_current].exit_reason==PROCESS_EXIT_NONE){sched_tasks[sched_current].exit_reason=PROCESS_EXIT_NORMAL;sched_tasks[sched_current].exit_status=f->ebx;}
    process_result_store(sched_tasks[sched_current].process_pid,PROCESS_TYPE_MT,sched_tasks[sched_current].exit_reason,sched_tasks[sched_current].exit_status,sched_tasks[sched_current].fault_vector,sched_tasks[sched_current].fault_error,sched_tasks[sched_current].fault_eip,sched_tasks[sched_current].fault_cr2);
    process_handles_close_pid(sched_tasks[sched_current].process_pid);data_readers_close_pid(sched_tasks[sched_current].process_pid);
    console_background_done(PROCESS_TYPE_MT,sched_current);
    sched_tasks[sched_current].state=TASK_EXIT;mt_quantum_used=0u;mt_shell_turn=0u;
    if(rt_mt_slack_active){rt_mt_slack_active=0u;rt_shell_waiting=0u;}
    load_cr3(sched_saved_shell_cr3);mem_copy((void*)f,sched_saved_shell,MT_CONTEXT_WORDS*4u);
    for(i=0u;i<SCHED_TASKS;i++)if(sched_tasks[i].state!=TASK_STOPPED&&sched_tasks[i].state!=TASK_EXIT&&sched_tasks[i].state!=TASK_CREATE){live=1u;break;}
    if(!live){
        /* FIX60ZEA: normal exit of the final MT process must end the MT
           session exactly like stopping the final slot does.  FIX60ZE's
           transient SONARVWR exposed the stale-session case: sched_active
           became zero but mt_session_active stayed one, so the next EXECMT
           returned BUSY even though no MT task was alive. */
        sched_active=0u;mt_session_active=0u;
        /* Keep mt_session_count as the retained post-mortem slot boundary.
           A future session clears/replaces it transactionally. */
        mt_quantum_used=0u;mt_shell_turn=0u;
        /* FIX60ZEB: natural exit closes the scheduler session but retains the
           completed task MTDATA/result snapshot for WAIT/LAST_MTDATA.  The
           snapshot is cleared transactionally when the next MT session starts,
           or explicitly by MTSTOP ALL. */
    }else if(sched_pick_any_ready()<0)sched_active=0u;
}
static uint32_t process_stop_pid(uint32_t pid){uint32_t i;if(!pid)return 0xffffffffu;for(i=0;i<SCHED_TASKS;i++)if(sched_tasks[i].process_pid==pid&&sched_tasks[i].state!=TASK_STOPPED&&sched_tasks[i].state!=TASK_EXIT&&sched_tasks[i].state!=TASK_CREATE){process_result_store(pid,PROCESS_TYPE_MT,PROCESS_EXIT_WATCHDOG,124u,0u,0u,0u,0u);process_handles_close_pid(pid);data_readers_close_pid(pid);console_background_done(PROCESS_TYPE_MT,i);sched_tasks[i].exit_reason=PROCESS_EXIT_WATCHDOG;sched_tasks[i].exit_status=124u;sched_tasks[i].state=TASK_STOPPED;if(i==sched_current)mt_quantum_used=MT_QUANTUM_TICKS;return 0u;}return 0xffffffffu;}
static uint32_t mt_stop_one(uint32_t id){uint32_t pid,i,live=0u;if(id>=SCHED_TASKS)return 0xffffffffu;if(sched_tasks[id].state==TASK_STOPPED||sched_tasks[id].state==TASK_EXIT||sched_tasks[id].state==TASK_CREATE)return 0xffffffffu;pid=sched_tasks[id].process_pid;if(pid){process_result_store(pid,PROCESS_TYPE_MT,PROCESS_EXIT_STOPPED,0u,0u,0u,0u,0u);sched_tasks[id].exit_reason=PROCESS_EXIT_STOPPED;sched_tasks[id].exit_status=0u;}process_handles_close_pid(pid);data_readers_close_pid(pid);process_heartbeat_clear(pid);console_background_done(PROCESS_TYPE_MT,id);sched_tasks[id].state=TASK_STOPPED;if(id==sched_current)mt_quantum_used=MT_QUANTUM_TICKS;
    /* FIX54A: an explicit stop of the final MT task ends the MT session just
       like MTSTOP ALL.  FIX54 left sched_active/mt_session_active set after
       the last slot became STOPPED; the next EXECMT therefore returned BUSY
       even though MT_ACTIVE and handle accounting were already zero. */
    for(i=0u;i<SCHED_TASKS;i++)if(sched_tasks[i].state!=TASK_STOPPED&&sched_tasks[i].state!=TASK_EXIT&&sched_tasks[i].state!=TASK_CREATE){live=1u;break;}
    if(!live){sched_active=0u;mt_session_active=0u;mt_session_count=0u;mt_quantum_used=0u;mt_shell_turn=0u;mt_data_clear_all();}
    return 0u;}

/* FIX35: contain selected CPU exceptions raised by a real Ring-3 EXE1 context.
   Ring-0 faults and unknown Ring-3 ownership remain fatal by design. */
static uint32_t process_ring3_fault(struct frame*f){
    struct process_identity id;uint32_t cr2=0u;
    if((f->cs&3u)!=3u)return 0u;
    if(f->int_no!=6u&&f->int_no!=13u&&f->int_no!=14u)return 0u;
    id=process_current_identity();
    /* FIX35A: an exception is synchronous: the loaded CR3 is the strongest
       evidence of which private Ring-3 address space faulted.  During RT->RT
       preemption the scheduler state can be in a hand-off window, so if the
       ordinary resolver cannot classify the frame, recover the RT owner by
       exact CR3 match.  This is containment-only and does not change normal
       FIX34A identity or scheduling policy. */
    if(id.type==PROCESS_TYPE_NONE){
        uint32_t i,cr3=read_cr3();
        for(i=0u;i<RT_MAX_TASKS;i++)if(rt_tasks[i].process_pid&&rt_tasks[i].cr3==cr3){
            id.type=PROCESS_TYPE_RT;id.slot=i;id.pid=rt_tasks[i].process_pid;id.cr3=cr3;break;
        }
    }
    if(id.type==PROCESS_TYPE_NONE||id.pid==0u)return 0u;
    if(f->int_no==14u)__asm__ volatile("movl %%cr2,%0":"=r"(cr2));
    if(id.type==PROCESS_TYPE_FG){
        exe_exit_reason=PROCESS_EXIT_FAULT;exe_exit_status=0xffffffffu;exe_fault_vector=f->int_no;exe_fault_error=f->error;exe_fault_eip=f->eip;exe_fault_cr2=cr2;
        /* A fault aborts a foreground queue instead of starting its next member. */
        f->ebx=1u;exe_exit(f);
        console_write("PROCESS FAULT PID=");console_write_u32(id.pid);console_write(" TYPE=FG VECTOR=");console_write_u32(exe_fault_vector);console_write(" EIP=");console_write_u32(exe_fault_eip);console_write("\n");
        return 1u;
    }
    if(id.type==PROCESS_TYPE_MT&&id.slot<SCHED_TASKS){
        struct sched_task*t=&sched_tasks[id.slot];
        t->exit_reason=PROCESS_EXIT_FAULT;t->exit_status=0xffffffffu;t->fault_vector=f->int_no;t->fault_error=f->error;t->fault_eip=f->eip;t->fault_cr2=cr2;
        /* No background console output. Reuse the proven MT termination path. */
        sched_exit(f);return 1u;
    }
    if(id.type==PROCESS_TYPE_RT&&id.slot<RT_MAX_TASKS){
        struct sched_task*t=&rt_tasks[id.slot];
        t->exit_reason=PROCESS_EXIT_FAULT;t->exit_status=0xffffffffu;t->fault_vector=f->int_no;t->fault_error=f->error;t->fault_eip=f->eip;t->fault_cr2=cr2;
        /* No background console output. The owner is already known here; do
           not ask the generic resolver to rediscover it during teardown. */
        rt_exit_slot(f,(int)id.slot);return 1u;
    }
    return 0u;
}


void interrupt_dispatch(struct frame*f){uint32_t n=f->int_no;if(n==32){rt_time_ticks++;rt_time_phase^=1u;if(rt_time_phase==0u)timer_ticks++;sched_irq_tick(f);}else if(n==33)keyboard_irq();else if(n==36)uart1_irq();else if(n==46)ata_irq();else if(n<32){
        if(process_ring3_fault(f))return;
        /* Ring-0 faults, unsupported vectors, or an unowned Ring-3 frame remain fatal.
           This deliberately prevents FIX35 from hiding kernel bugs. */
        /* При аварийном исключении печатаем номер, EIP, CS и error code.
           Для #PF дополнительно печатаем CR2 — адрес, вызвавший fault. */
        console_write("EXC ");console_write_u32(n);
        console_write(" EIP=");console_write_u32(f->eip);
        console_write(" CS=");console_write_u32(f->cs);
        console_write(" ERR=");console_write_u32(f->error);
        if(n==14){uint32_t cr2;__asm__ volatile("movl %%cr2,%0":"=r"(cr2));console_write(" CR2=");console_write_u32(cr2);}
        console_write("\n");
        for(;;){__asm__ volatile("cli");cpu_hlt();}}
    else if(n==0x80){uint32_t a=f->ebx,b=f->ecx,c=f->edx;switch(f->eax){
        case SYS_CONSOLE_WRITE:if((f->cs&3u)&&!user_range(a,b)){f->eax=0xffffffffu;break;}if(b&&a){console_background_write_note();f->eax=console_write_atomic_n((const char*)a,b);}else f->eax=0u;break;
        case SYS_CONSOLE_READ:{
            if((f->cs&3u)&&!user_rw_range(a,b)){f->eax=0xffffffffu;break;}
            if(b==0u){f->eax=0;break;}
            /* FIX60ZEG: code 3 is the already-established shell redraw token.
               Deliver it only to the interactive shell, after a detached
               console writer has terminated.  No frame/CR3 switch occurs. */
            if(console_redraw_pending&&!exe_active&&process_current_identity().type==PROCESS_TYPE_NONE){console_redraw_pending=0u;rt_shell_waiting=0u;rt_shell_hold=0u;f->eax=3u;break;}
            for(;;){
                uint8_t ch=key_pop();
                if(ch==KEY_EVENT_F10){rt_shell_waiting=0u;rt_shell_hold=0u;f->eax=4u;break;}
                if(ch==KEY_EVENT_UP){rt_shell_waiting=0u;rt_shell_hold=0u;f->eax=5u;break;}
                if(ch){rt_shell_waiting=0u;rt_shell_hold=0u;((uint8_t*)a)[0]=ch;f->eax=1u;break;}
                /* FIX60ZEF: CONSOLE_READ is no longer a context-switching
                   syscall.  Direct RT dispatch from this int80 frame was the
                   last path that could turn an idle/prompt continuation into
                   an RT->MT ownership chain.  With background work present,
                   mark the shell idle and return EAX=2 to Ring3.  IRQ0 then
                   performs every RT/MT switch from a genuine Ring3 frame. */
                if(mt_session_active&&!sched_active&&!exe_active)(void)sched_commit_prepared();
                if(rt_background_active||(sched_active&&!exe_active)){
                    rt_shell_waiting=1u;rt_shell_hold=0u;f->eax=2u;break;
                }
                __asm__ volatile("sti;hlt;cli":::"memory");
            }
        }break;
        case SYS_CONSOLE_POLL:{uint8_t ch=0;{int rid=rt_find_current();if(rid>=0){/* Detached RT owns only the explicit ESC stop event. All other keyboard bytes stay in the shell queue so interactive shell input cannot be consumed by SENSOR. */if(rt_stop_requested&(1u<<rid))ch=0x1bu;}else ch=key_pop();}if(ch&&((f->cs&3u)&&!user_rw_range(a,1u))){f->eax=0xffffffffu;break;}if(ch&&a&&((f->cs&3u)==0u||user_rw_range(a,1u))) *((uint8_t*)a)=ch;f->eax=(uint32_t)ch;break;}
        case SYS_TIMER_GET:f->eax=timer_ticks;break;case SYS_RT_TIME_GET:f->eax=rt_time_ticks;break;case SYS_RT_INFO:if((f->cs&3u)&&rt_background_active){int rid=rt_find_current();if(rid>=0&&a&&user_rw_range(a,32u)){((uint32_t*)a)[0]=rt_tasks[rid].rt_period_ms;((uint32_t*)a)[1]=rt_tasks[rid].rt_deadline_ms;((uint32_t*)a)[2]=rt_tasks[rid].rt_priority;((uint32_t*)a)[3]=rt_tasks[rid].rt_period_ticks;((uint32_t*)a)[4]=rt_tasks[rid].rt_next_release_tick;((uint32_t*)a)[5]=rt_tasks[rid].rt_deadline_tick;((uint32_t*)a)[6]=rt_tasks[rid].rt_job_active;((uint32_t*)a)[7]=rt_tasks[rid].rt_missed_deadlines;f->eax=0;}else f->eax=0xffffffffu;}else f->eax=0xffffffffu;break;case SYS_RT_JOB_INFO:if((f->cs&3u)&&rt_background_active){int rid=rt_find_current();if(rid>=0&&a&&user_rw_range(a,36u)){((uint32_t*)a)[0]=rt_tasks[rid].rt_job_sequence;((uint32_t*)a)[1]=rt_tasks[rid].rt_job_release_tick;((uint32_t*)a)[2]=rt_tasks[rid].rt_deadline_tick;((uint32_t*)a)[3]=rt_time_ticks;((uint32_t*)a)[4]=rt_tasks[rid].rt_next_release_tick;((uint32_t*)a)[5]=rt_tasks[rid].rt_job_active;((uint32_t*)a)[6]=rt_tasks[rid].rt_job_missed;((uint32_t*)a)[7]=rt_tasks[rid].rt_missed_deadlines;((uint32_t*)a)[8]=rt_tasks[rid].rt_skipped_releases;f->eax=0;}else f->eax=0xffffffffu;}else f->eax=0xffffffffu;break;case SYS_RT_JITTER_INFO:if((f->cs&3u)&&rt_background_active){int rid=rt_find_current();if(rid>=0&&a&&user_rw_range(a,52u)){((uint32_t*)a)[0]=rt_tasks[rid].rt_job_sequence;((uint32_t*)a)[1]=rt_tasks[rid].rt_period_ticks;((uint32_t*)a)[2]=rt_tasks[rid].rt_job_release_tick;((uint32_t*)a)[3]=rt_tasks[rid].rt_release_observed_tick;((uint32_t*)a)[4]=rt_tasks[rid].rt_release_interval_ticks;((uint32_t*)a)[5]=rt_tasks[rid].rt_release_min_interval;((uint32_t*)a)[6]=rt_tasks[rid].rt_release_max_interval;((uint32_t*)a)[7]=rt_tasks[rid].rt_release_late_ticks;((uint32_t*)a)[8]=rt_tasks[rid].rt_release_max_late;((uint32_t*)a)[9]=rt_tasks[rid].rt_job_dispatch_tick;((uint32_t*)a)[10]=rt_tasks[rid].rt_job_dispatch_latency;((uint32_t*)a)[11]=rt_tasks[rid].rt_job_dispatched;((uint32_t*)a)[12]=rt_tasks[rid].rt_release_samples;f->eax=0;}else f->eax=0xffffffffu;}else f->eax=0xffffffffu;break;case SYS_RT_EXEC_INFO:if((f->cs&3u)&&rt_background_active){int rid=rt_find_current();if(rid>=0&&a&&user_rw_range(a,56u)){((uint32_t*)a)[0]=rt_tasks[rid].rt_job_sequence;((uint32_t*)a)[1]=rt_tasks[rid].rt_period_ticks;((uint32_t*)a)[2]=rt_tasks[rid].rt_job_release_tick;((uint32_t*)a)[3]=rt_tasks[rid].rt_release_observed_tick;((uint32_t*)a)[4]=rt_tasks[rid].rt_release_interval_ticks;((uint32_t*)a)[5]=rt_tasks[rid].rt_release_late_ticks;((uint32_t*)a)[6]=rt_tasks[rid].rt_job_dispatch_tick;((uint32_t*)a)[7]=rt_tasks[rid].rt_job_dispatch_latency;((uint32_t*)a)[8]=rt_tasks[rid].rt_job_cpu_ticks;((uint32_t*)a)[9]=rt_time_ticks;((uint32_t*)a)[10]=rt_tasks[rid].rt_deadline_tick;((uint32_t*)a)[11]=rt_tasks[rid].rt_job_missed;((uint32_t*)a)[12]=rt_tasks[rid].rt_skipped_releases;((uint32_t*)a)[13]=rt_tasks[rid].rt_missed_deadlines;f->eax=0;}else f->eax=0xffffffffu;}else f->eax=0xffffffffu;break;case SYS_RT_TRACE:if((f->cs&3u)&&rt_background_active){int rid=rt_find_current();if(rid>=0&&a&&user_rw_range(a,20u)){((uint32_t*)a)[0]=(uint32_t)rid;((uint32_t*)a)[1]=rt_tasks[rid].rt_priority;((uint32_t*)a)[2]=rt_tasks[rid].switches;((uint32_t*)a)[3]=rt_tasks[rid].rt_dispatch_seq;((uint32_t*)a)[4]=rt_tasks[rid].state;f->eax=0;}else f->eax=0xffffffffu;}else f->eax=0xffffffffu;break;case SYS_RT_DEADLINE_INFO:if((f->cs&3u)&&rt_background_active){int rid=rt_find_current();if(rid>=0&&a&&user_rw_range(a,20u)){((uint32_t*)a)[0]=rt_tasks[rid].rt_job_release_tick;((uint32_t*)a)[1]=rt_tasks[rid].rt_deadline_tick;((uint32_t*)a)[2]=rt_time_ticks;((uint32_t*)a)[3]=rt_tasks[rid].rt_period_ticks;((uint32_t*)a)[4]=rt_tasks[rid].rt_missed_deadlines;f->eax=0;}else f->eax=0xffffffffu;}else f->eax=0xffffffffu;break;case SYS_RT_STATS:{uint32_t op=b,slot,i,j;if(!(f->cs&3u)){f->eax=0xffffffffu;break;}if(op==1u){for(slot=0u;slot<RT_MAX_TASKS;slot++)rt_stats_reset(&rt_tasks[slot]);f->eax=0u;break;}if(op==6u){rt_watch_active=a?1u:0u;f->eax=0u;break;}if(op==7u){
            if(a){
                uint32_t mask=0u;
                for(slot=0u;slot<RT_MAX_TASKS;slot++)
                    if(rt_tasks[slot].rt_period_ms!=0u&&rt_tasks[slot].state!=TASK_CREATE&&rt_tasks[slot].state!=TASK_STOPPED&&rt_tasks[slot].state!=TASK_EXIT)mask|=(1u<<slot);
                rt_launch_guard_start_mask=mask;
                rt_launch_guard=1u;
            }else{
                uint32_t now=rt_time_ticks,mask=rt_launch_guard_start_mask;
                /* FIX27: a task created by this guarded F10 transaction has
                   never been eligible to execute.  Do not interpret the
                   foreground RTD loading time as missed RT releases.  Commit
                   its first release at the instant the batch is unlocked.
                   Pre-existing RT tasks are deliberately untouched. */
                for(slot=0u;slot<RT_MAX_TASKS;slot++)if(!(mask&(1u<<slot))&&rt_tasks[slot].rt_period_ms!=0u&&rt_tasks[slot].state!=TASK_CREATE&&rt_tasks[slot].state!=TASK_STOPPED&&rt_tasks[slot].state!=TASK_EXIT){
                    struct sched_task*t=&rt_tasks[slot];
                    t->rt_next_release_tick=now;
                    t->rt_job_sequence=0u;t->rt_skipped_releases=0u;t->rt_missed_deadlines=0u;
                    t->rt_release_observed_tick=now;t->rt_release_last_tick=now;t->rt_release_interval_ticks=0u;t->rt_release_min_interval=0u;t->rt_release_max_interval=0u;t->rt_release_samples=1u;t->rt_release_late_ticks=0u;t->rt_release_max_late=0u;
                    t->rt_job_active=0u;t->rt_job_missed=0u;t->rt_job_dispatched=0u;t->rt_job_dispatch_tick=0u;t->rt_job_dispatch_latency=0u;t->rt_job_cpu_ticks=0u;t->rt_runtime_ticks=0u;
                    rt_stats_reset(t);rt_begin_job(t,now);
                    /* FIX60ZC: committing a guarded F10 launch is a state
                       transition, not only a timing reset.  A newly admitted
                       RT task must be dispatchable immediately after the guard
                       is released, regardless of any transient state observed
                       while its RTD helper was executing. */
                    t->state=TASK_READY;
                }
                rt_launch_guard=0u;rt_launch_guard_start_mask=0u;
                rt_recount_active();
            }
            f->eax=0u;break;
        }if(op==3u){char nbuf[QUEUE_NAME_SIZE];uint32_t k;if(!a||!user_range(a,QUEUE_NAME_SIZE)||c>1u){f->eax=0xffffffffu;break;}mem_copy(nbuf,(const void*)a,QUEUE_NAME_SIZE);for(slot=0u;slot<RT_MAX_TASKS;slot++){if(rt_tasks[slot].rt_period_ms!=0u&&str_len(rt_tasks[slot].name)==str_len(nbuf)){uint32_t ok=1u;for(k=0u;k<QUEUE_NAME_SIZE;k++)if(rt_tasks[slot].name[k]!=nbuf[k]){ok=0u;break;}if(ok)rt_mon[slot].diag_verbose=c;}}f->eax=0u;break;}if(op==4u){int rid=rt_find_current();f->eax=(rid>=0)?rt_mon[rid].diag_verbose:0u;break;}if(op==2u){uint32_t pos,k;slot=c;if(slot>=RT_MAX_TASKS||!a||!user_rw_range(a,65u*4u)){f->eax=0xffffffffu;break;}((uint32_t*)a)[0]=rt_mon[slot].ring_count;for(k=0u;k<rt_mon[slot].ring_count;k++){pos=(rt_mon[slot].ring_pos+8u-rt_mon[slot].ring_count+k)%8u;for(i=0u;i<8u;i++)((uint32_t*)a)[1u+k*8u+i]=rt_mon[slot].ring[pos][i];}f->eax=0u;break;}if(op==5u){uint32_t pos,k;slot=c;if(slot>=RT_MAX_TASKS||!a||!user_rw_range(a,83u*4u)){f->eax=0xffffffffu;break;}((uint32_t*)a)[0]=rt_mon[slot].event_count;((uint32_t*)a)[1]=rt_mon[slot].lifetime_misses;((uint32_t*)a)[2]=rt_mon[slot].lifetime_skips;for(k=0u;k<rt_mon[slot].event_count;k++){pos=(rt_mon[slot].event_pos+RT_ANOMALY_EVENTS-rt_mon[slot].event_count+k)%RT_ANOMALY_EVENTS;for(i=0u;i<5u;i++)((uint32_t*)a)[3u+k*5u+i]=rt_mon[slot].events[pos][i];}f->eax=0u;break;}if(!a||!user_rw_range(a,RT_MAX_TASKS*30u*4u)){f->eax=0xffffffffu;break;}for(slot=0u;slot<RT_MAX_TASKS;slot++){uint32_t*q=((uint32_t*)a)+(slot*30u);struct sched_task*t=&rt_tasks[slot];q[0]=t->id;q[1]=t->state;q[2]=t->rt_period_ticks;q[3]=t->rt_deadline_ms;for(i=0u;i<4u;i++){uint32_t w=0u;for(j=0u;j<4u;j++)w|=((uint32_t)(uint8_t)t->name[i*4u+j])<<(j*8u);q[4u+i]=w;}{uint32_t wv[18];rt_stats_window(&rt_mon[slot],rt_time_ticks,wv);q[8]=wv[0];q[9]=wv[1];q[10]=wv[2];q[11]=wv[3];q[12]=wv[4];q[13]=wv[5];q[14]=wv[6];q[15]=wv[7];q[16]=wv[8];q[17]=wv[9];q[18]=wv[10];q[19]=wv[11];q[20]=wv[12];q[21]=wv[13];q[22]=wv[14];q[23]=wv[15];q[24]=wv[16];q[25]=wv[17];}q[26]=rt_mon[slot].last_seq;q[27]=rt_mon[slot].last_interval;q[28]=(uint32_t)rt_mon[slot].last_jitter;q[29]=rt_mon[slot].last_dispatch;}f->eax=0u;break;}case SYS_RT_DATA:{
            /* EBX=buffer, ECX=operation/length, EDX=slot/capacity.
               Publish: ECX=1..255, EDX=0; only the currently executing RT
               task may publish and the destination slot is derived by the
               kernel. Read: ECX=0, EDX=slot; copies at most 255 bytes plus
               NUL to a 256-byte shell buffer. */
            if((f->cs&3u)==0u){f->eax=0xffffffffu;break;}
            if(b!=0u){
                int rid=rt_find_current();uint32_t n=b;
                if(rid<0||n>RT_DATA_MAX||!a||!user_range(a,n)){f->eax=0xffffffffu;break;}
                mem_copy(rt_data[rid].data,(const void*)a,n);
                rt_data[rid].data[n]=0;
                rt_data[rid].len=n;
                rt_data[rid].valid=1u;
                /* Commit marker is last. Wrap from UINT32_MAX to zero is
                   intentional and safe because readers compare only !=. */
                rt_data[rid].seq++;
                f->eax=n;break;
            }else{
                uint32_t slot=c,n;
                if(slot>=RT_MAX_TASKS||!a||!user_rw_range(a,RT_DATA_BUF_SIZE)){f->eax=0xffffffffu;break;}
                if(rt_tasks[slot].rt_period_ms==0u||rt_tasks[slot].state==TASK_CREATE||rt_tasks[slot].state==TASK_STOPPED||rt_tasks[slot].state==TASK_EXIT){f->eax=0xfffffffeu;break;}
                if(!rt_data[slot].valid){f->eax=0xfffffffdu;break;}
                n=rt_data[slot].len;if(n>RT_DATA_MAX)n=RT_DATA_MAX;
                mem_copy((void*)a,rt_data[slot].data,n);((char*)a)[n]=0;
                f->eax=n;break;
            }
        }break;case SYS_RT_YIELD:{int rid;if((f->cs&3u)==0u){f->eax=0xffffffffu;break;}if(!rt_background_active){f->eax=0u;break;}/* FIX60Y: foreground diagnostic yield is also a scheduler service point.\n           Reconcile releases before selecting a task so a test/watch caller does not\n           depend on having crossed a separate IRQ0 boundary immediately beforehand. */rt_release_jobs(rt_time_ticks);rid=rt_pick_ready();if(rid<0)rid=rt_pick_ready_missed();if(rid>=0){rt_explicit_yield_active=1u;rt_switch_to(f,rid);break;}__asm__ volatile("sti;hlt;cli":::"memory");f->eax=0u;break;}case SYS_RT_WAIT:rt_wait(f);break;
        case SYS_DISK_READ:if((f->cs&3u)&&(c>8u||!user_rw_range(b,c*SECTOR_SIZE))){f->eax=0xffffffffu;break;}f->eax=disk_read(a,(void*)b,c)?0:0xffffffffu;break;
        case SYS_DISK_WRITE:if((f->cs&3u)&&(c>8u||!user_range(b,c*SECTOR_SIZE))){f->eax=0xffffffffu;break;}f->eax=disk_write(a,(const void*)b,c)?0:0xffffffffu;break;
        case SYS_FILE_OPEN:{int fd;uint32_t owner;if((f->cs&3u)&&!user_cstr(a)){f->eax=0xffffffffu;break;}owner=(f->cs&3u)?process_handle_owner():0u;fd=fat_open((const char*)a,b);if(fd>=0)handles[fd].owner_pid=owner;f->eax=(uint32_t)fd;break;}
        case SYS_FILE_READ:{int r;uint32_t owner=(f->cs&3u)?process_handle_owner():0u;if((f->cs&3u)&&!user_rw_range(b,c)){f->eax=0xffffffffu;break;}if(!process_handle_access((int)a,owner)){f->eax=0xffffffffu;break;}r=fat_read_file_fd((int)a,(void*)b,c);f->eax=(r<0)?0xffffffffu:(uint32_t)r;break;}
        case SYS_FILE_WRITE:{int r;uint32_t owner=(f->cs&3u)?process_handle_owner():0u;if((f->cs&3u)&&!user_range(b,c)){f->eax=0xffffffffu;break;}if(!process_handle_access((int)a,owner)){f->eax=0xffffffffu;break;}r=fat_write_file_fd((int)a,(const void*)b,c);f->eax=(r<0)?0xffffffffu:(uint32_t)r;break;}
        case SYS_FILE_CLOSE:{uint32_t owner=(f->cs&3u)?process_handle_owner():0u;if(!process_handle_access((int)a,owner))f->eax=0xffffffffu;else f->eax=fat_close((int)a)?0:0xffffffffu;break;}

        case SYS_FILE_PREAD:{uint32_t q[3],owner=(f->cs&3u)?process_handle_owner():0u;int r;if((f->cs&3u)&&(!a||!user_range(a,12u))){f->eax=0xffffffffu;break;}mem_copy(q,(const void*)a,12u);if((f->cs&3u)&&!user_rw_range(q[1],q[2])){f->eax=0xffffffffu;break;}if(!process_handle_access((int)q[0],owner)){f->eax=0xffffffffu;break;}r=fat_pread_file_fd((int)q[0],(void*)q[1],q[2],b);f->eax=(r<0)?0xffffffffu:(uint32_t)r;break;}
        case SYS_FILE_PWRITE:{uint32_t q[3],owner=(f->cs&3u)?process_handle_owner():0u;int r;if((f->cs&3u)&&(!a||!user_range(a,12u))){f->eax=0xffffffffu;break;}mem_copy(q,(const void*)a,12u);if((f->cs&3u)&&!user_range(q[1],q[2])){f->eax=0xffffffffu;break;}if(!process_handle_access((int)q[0],owner)){f->eax=0xffffffffu;break;}r=fat_pwrite_file_fd((int)q[0],(const void*)q[1],q[2],b);f->eax=(r<0)?0xffffffffu:(uint32_t)r;break;}
        case SYS_FILE_DELETE:if((f->cs&3u)&&!user_cstr(a)){f->eax=0xffffffffu;break;}f->eax=fat_delete((const char*)a)?0:0xffffffffu;break;
        case SYS_FILE_COPY:if((f->cs&3u)&&(!user_cstr(a)||!user_cstr(b))){f->eax=0xffffffffu;break;}f->eax=fat_copy_file((const char*)a,(const char*)b)?0:0xffffffffu;break;
        case SYS_FILE_SIZE:if((f->cs&3u)&&!user_cstr(a)){f->eax=0xffffffffu;break;}{int r=fat_file_size((const char*)a);f->eax=(r<0)?0xffffffffu:(uint32_t)r;}break;
        case SYS_LS:if((f->cs&3u)&&a&&!user_cstr(a)){f->eax=0xffffffffu;break;}f->eax=fat_ls(a?(const char*)a:"/")?0:0xffffffffu;break;
        case SYS_MKDIR:if((f->cs&3u)&&!user_cstr(a)){f->eax=0xffffffffu;break;}f->eax=fat_mkdir((const char*)a)?0:0xffffffffu;break;
        case SYS_RMDIR:if((f->cs&3u)&&!user_cstr(a)){f->eax=0xffffffffu;break;}f->eax=fat_rmdir((const char*)a)?0:0xffffffffu;break;
        case SYS_CHDIR:if((f->cs&3u)&&!user_cstr(a)){f->eax=0xffffffffu;break;}f->eax=fat_chdir((const char*)a)?0:0xffffffffu;break;
        case SYS_GETCWD:if((f->cs&3u)&&(!user_rw_range(a,b)||b==0u)){f->eax=0xffffffffu;break;}{uint32_t n=str_len(fat_cwd)+1u;if(b<n){f->eax=0xffffffffu;break;}mem_copy((void*)a,fat_cwd,n);f->eax=n-1u;}break;
        case SYS_MT_DATA:{
            struct mt_data_request req;uint32_t slot,n;
            if((f->cs&3u)==0u||!a||!user_rw_range(a,sizeof(req))){f->eax=0xffffffffu;break;}
            mem_copy(&req,(const void*)a,sizeof(req));
            if(req.op==MT_DATA_PUBLISH){
                /* A publisher never chooses its MT number: the kernel derives
                   it from the currently executing task/CR3. */
                if(!mt_session_active||!sched_active||sched_current>=mt_session_count||
                   sched_tasks[sched_current].state!=TASK_RUNNING||read_cr3()!=sched_tasks[sched_current].cr3||
                   req.length==0u||req.length>MT_DATA_MAX||!req.buffer||!user_range(req.buffer,req.length)){
                    f->eax=0xffffffffu;break;
                }
                slot=sched_current;n=req.length;
                mem_copy(mt_data[slot].data,(const void*)req.buffer,n);mt_data[slot].data[n]=0;
                mt_data[slot].len=n;mt_data[slot].valid=1u;mt_data[slot].seq++;
                req.mt_id=slot;req.seq=mt_data[slot].seq;req.session=mt_session_id;
                mem_copy((void*)a,&req,sizeof(req));f->eax=n;break;
            }
            if(req.op==MT_DATA_READ){
                slot=req.mt_id;
                /* FIX60ZEC: retained MTDATA is readable after the final task
                   exits naturally. Explicit MTSTOP ALL sets count=0 and clears
                   snapshots, so stale data is still inaccessible there. */
                if(slot>=mt_session_count||slot>=SCHED_TASKS||!req.buffer||req.length<MT_DATA_BUF_SIZE||
                   !user_rw_range(req.buffer,MT_DATA_BUF_SIZE)){f->eax=0xfffffffeu;break;}
                req.session=mt_session_id;req.seq=mt_data[slot].seq;
                if(!mt_data[slot].valid){req.length=0u;mem_copy((void*)a,&req,sizeof(req));f->eax=0xfffffffdu;break;}
                n=mt_data[slot].len;if(n>MT_DATA_MAX)n=MT_DATA_MAX;
                mem_copy((void*)req.buffer,mt_data[slot].data,n);((char*)req.buffer)[n]=0;
                req.length=n;mem_copy((void*)a,&req,sizeof(req));f->eax=n;break;
            }
            f->eax=0xffffffffu;
        }break;
        case SYS_PROCESS_INFO:{
            /* FIX34 extends FIX33 syscall 55 without changing its number.
               EBX=0: snapshot (34 x 10 u32, unchanged FIX33 ABI).
               EBX=1: current Ring-3 identity (one 10-u32 record).
               Record: PID,TYPE,STATE,SLOT,CR3,CURRENT,NAME[16]. */
            uint32_t i,j,row=0u,*out=(uint32_t*)a,cr3=read_cr3();
            if(!(f->cs&3u)||!a){f->eax=0xffffffffu;break;}
            if(b==1u){
                struct process_identity id=process_current_identity();uint32_t*q=out;
                if(!user_rw_range(a,10u*4u)){f->eax=0xffffffffu;break;}mem_set(q,0,10u*4u);
                q[0]=id.pid;q[1]=id.type;q[3]=id.slot;q[4]=id.cr3;q[5]=id.type?1u:0u;
                if(id.type==PROCESS_TYPE_FG){q[2]=TASK_RUNNING;for(j=0;j<4u;j++){uint32_t w=0u,k;for(k=0;k<4u;k++)w|=((uint32_t)(uint8_t)exe_proc_name[j*4u+k])<<(k*8u);q[6u+j]=w;}}
                else if(id.type==PROCESS_TYPE_MT){q[2]=sched_tasks[id.slot].state;for(j=0;j<4u;j++){uint32_t w=0u,k;for(k=0;k<4u;k++)w|=((uint32_t)(uint8_t)sched_tasks[id.slot].name[j*4u+k])<<(k*8u);q[6u+j]=w;}}
                else if(id.type==PROCESS_TYPE_RT){q[2]=rt_tasks[id.slot].state;for(j=0;j<4u;j++){uint32_t w=0u,k;for(k=0;k<4u;k++)w|=((uint32_t)(uint8_t)rt_tasks[id.slot].name[j*4u+k])<<(k*8u);q[6u+j]=w;}}
                f->eax=id.type?1u:0u;break;
            }
            if(b==2u){
                /* FIX35 fault detail query. ECX=2, EDX=PID, EBX points to 9 u32:
                   PID,TYPE,STATE,SLOT,EXIT_REASON,VECTOR,ERROR,EIP,CR2. */
                uint32_t pid=c,*q=out,found=0u;
                if(!user_rw_range(a,9u*4u)||pid==0u){f->eax=0xffffffffu;break;}mem_set(q,0,9u*4u);
                if(exe_pid==pid){q[0]=pid;q[1]=PROCESS_TYPE_FG;q[2]=exe_active?TASK_RUNNING:exe_proc_state;q[3]=0xffffffffu;q[4]=exe_exit_reason;q[5]=exe_fault_vector;q[6]=exe_fault_error;q[7]=exe_fault_eip;q[8]=exe_fault_cr2;found=1u;}
                for(i=0u;!found&&i<SCHED_TASKS;i++)if(sched_tasks[i].process_pid==pid){struct sched_task*t=&sched_tasks[i];q[0]=pid;q[1]=PROCESS_TYPE_MT;q[2]=t->state;q[3]=i;q[4]=t->exit_reason;q[5]=t->fault_vector;q[6]=t->fault_error;q[7]=t->fault_eip;q[8]=t->fault_cr2;found=1u;}
                for(i=0u;!found&&i<RT_MAX_TASKS;i++)if(rt_tasks[i].process_pid==pid){struct sched_task*t=&rt_tasks[i];q[0]=pid;q[1]=PROCESS_TYPE_RT;q[2]=t->state;q[3]=i;q[4]=t->exit_reason;q[5]=t->fault_vector;q[6]=t->fault_error;q[7]=t->fault_eip;q[8]=t->fault_cr2;found=1u;}
                f->eax=found?1u:0u;break;
            }
            if(b!=0u||!user_rw_range(a,34u*10u*4u)){f->eax=0xffffffffu;break;}
            mem_set(out,0,34u*10u*4u);
            if(exe_pid){uint32_t*q=out+row++*10u;q[0]=exe_pid;q[1]=PROCESS_TYPE_FG;q[2]=exe_active?TASK_RUNNING:exe_proc_state;q[3]=0xffffffffu;q[4]=(uint32_t)page_directory;q[5]=(exe_active&&cr3==(uint32_t)page_directory)?1u:0u;for(j=0;j<4u;j++){uint32_t w=0u,k;for(k=0;k<4u;k++)w|=((uint32_t)(uint8_t)exe_proc_name[j*4u+k])<<(k*8u);q[6u+j]=w;}}
            for(i=0u;i<SCHED_TASKS;i++)if(sched_tasks[i].process_pid){uint32_t*q=out+row++*10u;q[0]=sched_tasks[i].process_pid;q[1]=PROCESS_TYPE_MT;q[2]=sched_tasks[i].state;q[3]=i;q[4]=sched_tasks[i].cr3;q[5]=(cr3==sched_tasks[i].cr3&&sched_tasks[i].state==TASK_RUNNING)?1u:0u;for(j=0;j<4u;j++){uint32_t w=0u,k;for(k=0;k<4u;k++)w|=((uint32_t)(uint8_t)sched_tasks[i].name[j*4u+k])<<(k*8u);q[6u+j]=w;}}
            for(i=0u;i<RT_MAX_TASKS;i++)if(rt_tasks[i].process_pid){uint32_t*q=out+row++*10u;q[0]=rt_tasks[i].process_pid;q[1]=PROCESS_TYPE_RT;q[2]=rt_tasks[i].state;q[3]=i;q[4]=rt_tasks[i].cr3;q[5]=(cr3==rt_tasks[i].cr3&&rt_tasks[i].state==TASK_RUNNING)?1u:0u;for(j=0;j<4u;j++){uint32_t w=0u,k;for(k=0;k<4u;k++)w|=((uint32_t)(uint8_t)rt_tasks[i].name[j*4u+k])<<(k*8u);q[6u+j]=w;}}
            f->eax=row;
        }break;
        case SYS_PROCESS_SPAWN:{uint32_t pid=0u;int r;if((f->cs&3u)==0u){f->eax=0xffffffffu;break;}if(safe_mode==2u){safe_deny_spawn++;f->eax=SAFE_POLICY_DENIED;break;}r=process_spawn(a,f,&pid);if(r==EXE_OK){f->eax=pid;}else f->eax=(uint32_t)r;break;}
        case SYS_RESOURCE_INFO:{uint32_t pid;if((f->cs&3u)==0u){f->eax=0xffffffffu;break;}if(a==0u){f->eax=process_handles_count(0xffffffffu,1u);break;}if(a==1u){pid=b;if(!pid){f->eax=0xffffffffu;break;}f->eax=process_handles_count(pid,0u);break;}f->eax=0xffffffffu;break;}
        case SYS_PROCESS_RESULT:{struct process_result*r;uint32_t*q=(uint32_t*)a;if((f->cs&3u)==0u||!a||!user_rw_range(a,8u*4u)||!b){f->eax=0xffffffffu;break;}r=process_result_find(b);if(r){q[0]=r->pid;q[1]=r->type;q[2]=r->reason;q[3]=r->status;q[4]=r->vector;q[5]=r->error;q[6]=r->eip;q[7]=r->cr2;f->eax=1u;}else f->eax=process_pid_known(b)?0u:0xfffffffdu;}break;
        case SYS_PROCESS_HEARTBEAT:{struct process_identity id;struct process_heartbeat*h;if((f->cs&3u)==0u){f->eax=0xffffffffu;break;}if(b==0u){id=process_current_identity();if(!id.pid){f->eax=0xffffffffu;break;}h=process_heartbeat_get(id.pid);h->last_tick=timer_ticks;h->seq++;f->eax=h->seq;}else{uint32_t*q=(uint32_t*)a;if(!q||!user_rw_range(a,3u*4u)){f->eax=0xffffffffu;break;}h=process_heartbeat_find(b);if(!h){f->eax=0u;break;}q[0]=h->pid;q[1]=h->last_tick;q[2]=h->seq;f->eax=1u;}}break;
        case SYS_PROCESS_STOP_PID:{struct process_identity id;if((f->cs&3u)==0u){f->eax=0xffffffffu;break;}id=process_current_identity();if(id.type!=PROCESS_TYPE_MT||id.pid==a){f->eax=0xffffffffu;break;}f->eax=process_stop_pid(a);}break;
        case SYS_EVENT_LOG:{
            uint32_t op=b,i,n,start,*q=(uint32_t*)a;
            if((f->cs&3u)==0u){f->eax=0xffffffffu;break;}
            if(op==1u){process_event_pos=0u;process_event_seq=0u;f->eax=0u;break;}
            if(op!=0u||!q||!user_rw_range(a,(1u+PROCESS_EVENT_MAX*8u)*4u)){f->eax=0xffffffffu;break;}
            n=process_event_pos<PROCESS_EVENT_MAX?process_event_pos:PROCESS_EVENT_MAX;start=process_event_pos-n;q[0]=n;
            for(i=0u;i<n;i++){struct process_event*e=&process_events[(start+i)%PROCESS_EVENT_MAX];uint32_t*x=q+1u+i*8u;x[0]=e->seq;x[1]=e->tick;x[2]=e->pid;x[3]=e->type;x[4]=e->reason;x[5]=e->status;x[6]=e->vector;x[7]=e->error;}
            f->eax=n;
        }break;
        case SYS_SAFE_MODE:{
            uint32_t op=b,*q=(uint32_t*)a;
            if((f->cs&3u)==0u){f->eax=0xffffffffu;break;}
            if(op==0u){if(!q||!user_rw_range(a,4u*4u)){f->eax=0xffffffffu;break;}q[0]=safe_mode;q[1]=safe_reason;q[2]=safe_seq;q[3]=safe_tick;f->eax=0u;break;}
            if(op==1u){uint32_t old=safe_mode;if(a>2u){f->eax=0xffffffffu;break;}safe_mode=a;safe_reason=c;safe_seq++;safe_tick=timer_ticks;if(old!=a||c!=0u)process_event_store(0u,a==0u?EVENT_REC_NORMAL:(a==1u?EVENT_REC_DEGRADED:EVENT_REC_SAFE),c,old,0u,0u);f->eax=0u;break;}
            f->eax=0xffffffffu;
        }break;
        case SYS_RECOVERY_EVENT:{
            /* FIX49: Ring3 recovery components may append metadata only. This
               never changes scheduler/SAFE state and never performs filesystem I/O.
               q={type,pid,reason,status}; type is restricted to supervisor markers. */
            uint32_t*q=(uint32_t*)a;if((f->cs&3u)==0u||!q||!user_range(a,4u*4u)||q[0]<EVENT_REC_RESTART||q[0]>EVENT_REC_BUDGET){f->eax=0xffffffffu;break;}
            process_event_store(q[1],q[0],q[2],q[3],0u,0u);f->eax=0u;
        }break;
        case SYS_RECOVERY_MANAGER:{
            /* FIX51: FIX50 op0..2 ABI is preserved. New lifecycle reports:
               op3=old PID stopped, op4={old,new} spawned, op5=new PID verified,
               op6={pid,result,detail} failed, op7=extended query (12 DWORD).
               This remains metadata/policy only: no FAT I/O or scheduler manipulation. */
            uint32_t*q=(uint32_t*)a;
            if((f->cs&3u)==0u){f->eax=0xffffffffu;break;}
            if(b==0u){if(!q||!user_rw_range(a,8u*4u)){f->eax=0xffffffffu;break;}q[0]=recovery_attempts;q[1]=RECOVERY_RESTART_LIMIT;q[2]=recovery_last_pid;q[3]=recovery_last_kind;q[4]=recovery_last_action;q[5]=recovery_seq;q[6]=safe_mode;q[7]=safe_reason;f->eax=0u;break;}
            if(b==1u){recovery_attempts=0u;recovery_last_pid=0u;recovery_last_kind=0u;recovery_last_action=0u;recovery_state=RECOVERY_STATE_IDLE;recovery_old_pid=0u;recovery_new_pid=0u;recovery_result=RECOVERY_RESULT_NONE;recovery_detail=0u;recovery_tick=timer_ticks;recovery_seq++;f->eax=0u;break;}
            if(b==2u){uint32_t pid,kind,detail,reason;if(!q||!user_range(a,3u*4u)){f->eax=0xffffffffu;break;}pid=q[0];kind=q[1];detail=q[2];if(!pid||(kind!=RECOVERY_KIND_FAULT&&kind!=RECOVERY_KIND_WATCHDOG)){f->eax=0xffffffffu;break;}recovery_last_pid=pid;recovery_last_kind=kind;recovery_old_pid=pid;recovery_new_pid=0u;recovery_result=RECOVERY_RESULT_NONE;recovery_detail=detail;recovery_state=RECOVERY_STATE_DETECTED;recovery_tick=timer_ticks;recovery_seq++;
                if(recovery_attempts<RECOVERY_RESTART_LIMIT){recovery_attempts++;reason=kind==RECOVERY_KIND_WATCHDOG?4502u:4501u;{uint32_t old=safe_mode;safe_mode=1u;safe_reason=reason;safe_seq++;safe_tick=timer_ticks;if(old!=1u||reason)process_event_store(0u,EVENT_REC_DEGRADED,reason,old,0u,0u);}process_event_store(pid,EVENT_REC_RESTART,reason,detail,0u,0u);recovery_last_action=RECOVERY_ACTION_RESTART;f->eax=RECOVERY_ACTION_RESTART;break;}
                reason=kind==RECOVERY_KIND_WATCHDOG?4592u:4591u;process_event_store(pid,EVENT_REC_BUDGET,reason,detail,0u,0u);{uint32_t old=safe_mode;safe_mode=2u;safe_reason=reason;safe_seq++;safe_tick=timer_ticks;process_event_store(0u,EVENT_REC_SAFE,reason,old,0u,0u);}recovery_last_action=RECOVERY_ACTION_SAFE;recovery_state=RECOVERY_STATE_SAFE;recovery_tick=timer_ticks;f->eax=RECOVERY_ACTION_SAFE;break;}
            if(b==3u){struct process_result*pr;if(!a||a!=recovery_old_pid||recovery_state!=RECOVERY_STATE_DETECTED||process_pid_known(a)||process_heartbeat_find(a)||(pr=process_result_find(a))==0||pr->reason==PROCESS_EXIT_NONE){f->eax=0xffffffffu;break;}recovery_state=RECOVERY_STATE_STOPPED;recovery_tick=timer_ticks;recovery_seq++;f->eax=0u;break;}
            if(b==4u){if(!q||!user_range(a,2u*4u)||!q[0]||!q[1]||q[0]!=recovery_old_pid||q[0]==q[1]||recovery_state!=RECOVERY_STATE_STOPPED||!process_pid_known(q[1])){f->eax=0xffffffffu;break;}recovery_new_pid=q[1];recovery_state=RECOVERY_STATE_VERIFYING;recovery_tick=timer_ticks;recovery_seq++;f->eax=0u;break;}
            if(b==5u){struct process_heartbeat*h;if(!a||a!=recovery_new_pid||recovery_state!=RECOVERY_STATE_VERIFYING||!process_pid_known(a)||(h=process_heartbeat_find(a))==0||h->seq==0u){f->eax=0xffffffffu;break;}recovery_state=RECOVERY_STATE_RECOVERED;recovery_result=RECOVERY_RESULT_SUCCESS;recovery_detail=0u;recovery_tick=timer_ticks;recovery_seq++;f->eax=0u;break;}
            if(b==6u){if(!q||!user_range(a,3u*4u)||!q[0]||q[1]<RECOVERY_RESULT_STOP_FAILED||q[1]>RECOVERY_RESULT_PROCESS_FAILED){f->eax=0xffffffffu;break;}recovery_state=RECOVERY_STATE_FAILED;recovery_result=q[1];recovery_detail=q[2];recovery_tick=timer_ticks;recovery_seq++;f->eax=0u;break;}
            if(b==7u){if(!q||!user_rw_range(a,12u*4u)){f->eax=0xffffffffu;break;}q[0]=1u;q[1]=recovery_state;q[2]=recovery_attempts;q[3]=RECOVERY_RESTART_LIMIT;q[4]=recovery_old_pid;q[5]=recovery_new_pid;q[6]=recovery_last_kind;q[7]=recovery_last_action;q[8]=recovery_result;q[9]=recovery_detail;q[10]=recovery_seq;q[11]=recovery_tick;f->eax=0u;break;}
            f->eax=0xffffffffu;
        }break;
        case SYS_LAYOUT_INFO:{
            uint32_t*q=(uint32_t*)a,i;
            if((f->cs&3u)==0u||b!=0u||!layout_valid||!q||!user_rw_range(a,LAYOUT_WORDS*4u)){f->eax=0xffffffffu;break;}
            for(i=0u;i<LAYOUT_WORDS;i++)q[i]=layout_word(i);
            f->eax=0u;
        }break;
        case SYS_SYSTEM_HEALTH:{
            /* FIX47: read-only health aggregation. No policy action is taken here.
               q: HEALTH,FLAGS,SAFE_MODE,SAFE_REASON,RT_ACTIVE,MT_ACTIVE,HANDLES,HWWD_ARMED,HWWD_BACKEND,NOW.
               FLAGS bit0=DEGRADED, bit1=SAFE, bit2=HWWD_EXPIRED(emulated observation). */
            uint32_t *q=(uint32_t*)a,rt=0u,mt=0u,flags=0u,health=0u,i;
            if((f->cs&3u)==0u||b!=0u||!q||!user_rw_range(a,10u*4u)){f->eax=0xffffffffu;break;}
            for(i=0u;i<RT_MAX_TASKS;i++)if(rt_tasks[i].process_pid&&(rt_tasks[i].state==TASK_READY||rt_tasks[i].state==TASK_RUNNING||rt_tasks[i].state==TASK_BLOCKED))rt++;
            for(i=0u;i<SCHED_TASKS;i++)if(sched_tasks[i].process_pid&&(sched_tasks[i].state==TASK_READY||sched_tasks[i].state==TASK_RUNNING||sched_tasks[i].state==TASK_BLOCKED))mt++;
            if(safe_mode==1u){health=1u;flags|=1u;}else if(safe_mode==2u){health=2u;flags|=2u;}
            if(hwwd_armed&&hwwd_timeout&&(uint32_t)(timer_ticks-hwwd_last_feed)>=hwwd_timeout){flags|=4u;if(health<1u)health=1u;}
            q[0]=health;q[1]=flags;q[2]=safe_mode;q[3]=safe_reason;q[4]=rt;q[5]=mt;q[6]=process_handles_count(0xffffffffu,1u);q[7]=hwwd_armed;q[8]=hwwd_backend;q[9]=timer_ticks;f->eax=0u;
        }break;
        case SYS_SAFE_POLICY:{
            /* FIX48: read-only policy snapshot plus explicit diagnostic-counter reset.
               q: MODE,FLAGS,DENY_SPAWN,DENY_MT,DENY_RT,DENY_TOTAL.
               FLAGS bit0 blocks detached SPAWN, bit1 EXECMT, bit2 RT start. */
            uint32_t op=b,*q=(uint32_t*)a;
            if((f->cs&3u)==0u){f->eax=0xffffffffu;break;}
            if(op==1u){safe_deny_spawn=safe_deny_mt=safe_deny_rt=0u;f->eax=0u;break;}
            if(op!=0u||!q||!user_rw_range(a,6u*4u)){f->eax=0xffffffffu;break;}
            q[0]=safe_mode;q[1]=(safe_mode==2u)?7u:0u;q[2]=safe_deny_spawn;q[3]=safe_deny_mt;q[4]=safe_deny_rt;q[5]=safe_deny_spawn+safe_deny_mt+safe_deny_rt;f->eax=0u;
        }break;
        case SYS_HW_WATCHDOG:{
            uint32_t op=b,*q=(uint32_t*)a;
            if((f->cs&3u)==0u){f->eax=0xffffffffu;break;}
            if(op==0u){if(!q||!user_rw_range(a,6u*4u)){f->eax=0xffffffffu;break;}q[0]=hwwd_backend;q[1]=hwwd_armed;q[2]=hwwd_timeout;q[3]=hwwd_last_feed;q[4]=hwwd_feed_seq;q[5]=timer_ticks;f->eax=0u;break;}
            if(op==1u){if(a==0u){f->eax=0xffffffffu;break;}hwwd_armed=1u;hwwd_timeout=a;hwwd_last_feed=timer_ticks;hwwd_feed_seq=0u;f->eax=0u;break;}
            if(op==2u){if(!hwwd_armed){f->eax=0xffffffffu;break;}hwwd_last_feed=timer_ticks;hwwd_feed_seq++;f->eax=hwwd_feed_seq;break;}
            if(op==3u){hwwd_armed=0u;hwwd_timeout=0u;f->eax=0u;break;}
            f->eax=0xffffffffu;
        }break;
        case SYS_PROCESS_WAIT:{
            /* FIX36B: EBX=8-u32 output, ECX=PID. Return 1=completed,
               0=known but not completed, 0xfffffffd=unknown/expired.
               ECX=0 is an internal shell cancel/cleanup operation.

               A pending wait must not spin through SYS_RT_YIELD: that syscall
               can sleep inside Ring0 when no RT job is ready, and PIT Ring0
               frames deliberately cannot schedule MT. Instead WAIT itself
               hands the original Ring3 shell frame to RT first, then MT, using
               the same proven ownership rules as SYS_CONSOLE_READ. */
            struct process_result*r;uint32_t*q=(uint32_t*)a;
            if(!(f->cs&3u)){f->eax=0xffffffffu;break;}
            if(b==0u){process_wait_active=0u;process_wait_pid=0u;process_wait_mt_turn=0u;f->eax=0u;break;}
            if(!a||!user_rw_range(a,8u*4u)){process_wait_active=0u;process_wait_pid=0u;process_wait_mt_turn=0u;f->eax=0xffffffffu;break;}
            r=process_result_find(b);
            if(r){process_wait_active=0u;process_wait_pid=0u;process_wait_mt_turn=0u;q[0]=r->pid;q[1]=r->type;q[2]=r->reason;q[3]=r->status;q[4]=r->vector;q[5]=r->error;q[6]=r->eip;q[7]=r->cr2;f->eax=1u;break;}
            if(!process_pid_known(b)){process_wait_active=0u;process_wait_pid=0u;process_wait_mt_turn=0u;f->eax=0xfffffffdu;break;}
            if(process_wait_pid!=b){process_wait_pid=b;process_wait_mt_turn=0u;}
            process_wait_active=1u;f->eax=0u;
            /* FIX60ZEE: a tight WAIT loop previously asked RT first on every
               iteration. With 4x10-ms RT tasks that can leave a spawned MT
               target permanently READY while the shell appears hung. Reserve
               one MT opportunity after every RT burst. */
            if(process_wait_mt_turn&&sched_active&&!exe_active){int mid=sched_pick_next();if(mid<0)mid=sched_pick_any_ready();process_wait_mt_turn=0u;if(mid>=0){mem_copy(sched_saved_shell,(const void*)f,MT_CONTEXT_WORDS*4u);sched_saved_shell[7]=0u;sched_saved_shell_cr3=read_cr3();sched_current=(uint32_t)mid;sched_tasks[sched_current].state=TASK_RUNNING;sched_tasks[sched_current].switches++;sched_switches++;mt_stats_dispatch(sched_current);mt_quantum_used=0u;load_cr3(sched_tasks[sched_current].cr3);mem_copy((void*)f,sched_tasks[sched_current].words,MT_CONTEXT_WORDS*4u);break;}}
            if(rt_background_active){int rid=rt_pick_ready();if(rid<0)rid=rt_pick_ready_missed();if(rid>=0){process_wait_mt_turn=1u;rt_switch_to(f,rid);break;}}
            if(sched_active&&!exe_active){int mid=sched_pick_next();if(mid<0)mid=sched_pick_any_ready();if(mid>=0){process_wait_mt_turn=0u;mem_copy(sched_saved_shell,(const void*)f,MT_CONTEXT_WORDS*4u);sched_saved_shell[7]=0u;sched_saved_shell_cr3=read_cr3();sched_current=(uint32_t)mid;sched_tasks[sched_current].state=TASK_RUNNING;sched_tasks[sched_current].switches++;sched_switches++;mt_stats_dispatch(sched_current);mt_quantum_used=0u;load_cr3(sched_tasks[sched_current].cr3);mem_copy((void*)f,sched_tasks[sched_current].words,MT_CONTEXT_WORDS*4u);break;}}
            __asm__ volatile("sti;hlt;cli":::"memory");
        }break;
        case SYS_EXECMT_COMMIT:if((f->cs&3u)==0u)f->eax=0xffffffffu;else f->eax=sched_commit_prepared();break;
        case SYS_MT_STATS:{uint32_t i,j;if(!(f->cs&3u)||!a||!user_rw_range(a,SCHED_TASKS*10u*4u)){f->eax=0xffffffffu;break;}for(i=0u;i<SCHED_TASKS;i++){uint32_t*q=((uint32_t*)a)+i*10u;q[0]=i;q[1]=sched_tasks[i].state;q[2]=mt_stat_dispatch[i];q[3]=mt_stat_quanta[i];q[4]=mt_stat_cpu_ticks[i];q[5]=(i==sched_current&&sched_tasks[i].state==TASK_RUNNING&&read_cr3()==sched_tasks[i].cr3)?1u:0u;for(j=0u;j<4u;j++){uint32_t w=0u,k;for(k=0u;k<4u;k++)w|=((uint32_t)(uint8_t)sched_tasks[i].name[j*4u+k])<<(k*8u);q[6u+j]=w;}}f->eax=0u;}break;
        case SYS_MT_STATUS:{uint32_t i,j;if(!(f->cs&3u)||!a||!user_rw_range(a,SCHED_TASKS*8u*4u)){f->eax=0xffffffffu;break;}for(i=0;i<SCHED_TASKS;i++){uint32_t*q=((uint32_t*)a)+i*8u;q[0]=i;q[1]=sched_tasks[i].state;q[2]=sched_tasks[i].switches;q[3]=(i==sched_current&&sched_tasks[i].state==TASK_RUNNING)?1u:0u;for(j=0;j<4u;j++){uint32_t w=0u,k;for(k=0;k<4u;k++)w|=((uint32_t)(uint8_t)sched_tasks[i].name[j*4u+k])<<(k*8u);q[4u+j]=w;}}f->eax=0u;}break;case SYS_MT_STOP_ONE:if(f->cs&3u)f->eax=mt_stop_one(a);else f->eax=0xffffffffu;break;case SYS_EXECMT:{int r;if((f->cs&3u)&&((b==0u)||(b>EXECMT_MAX_TASKS)||!user_range(a,b*QUEUE_NAME_SIZE))){f->eax=0xffffffffu;break;}if((f->cs&3u)&&safe_mode==2u){safe_deny_mt++;f->eax=SAFE_POLICY_DENIED;break;}r=execmt_prepare(a,b,f);f->eax=(uint32_t)r;}break;case SYS_MT_START:if(f->cs&3u){if(safe_mode==2u){safe_deny_mt++;f->eax=SAFE_POLICY_DENIED;}else sched_start(f);}else f->eax=0xffffffffu;break;case SYS_SCHED_BLOCK:if(f->cs&3u)sched_block(f);else f->eax=0xffffffffu;break;case SYS_MT_YIELD:if(f->cs&3u)sched_yield(f);else f->eax=0xffffffffu;break;case SYS_SCHED_WAKE:if(f->cs&3u)sched_wake(f);else f->eax=0xffffffffu;break;case SYS_SCHED_EXIT:if(f->cs&3u){struct process_identity id=process_current_identity();if(id.type==PROCESS_TYPE_RT)rt_exit(f);else if(id.type==PROCESS_TYPE_MT)sched_exit(f);else if(id.type==PROCESS_TYPE_FG)exe_exit(f);else f->eax=0xffffffffu;}else f->eax=0xffffffffu;break;
        case SYS_MT_STOP:if(f->cs&3u)sched_stop(f);else f->eax=0xffffffffu;break;
        case SYS_EXEC:if(rt_find_current()>=0){f->eax=(uint32_t)EXE_ERR_BUSY;break;}if((f->cs&3u)&&!user_cstr(a)){f->eax=(uint32_t)EXE_ERR_NAME;break;}f->eax=(uint32_t)exe_load((const char*)a,f);break;
        case SYS_RT_START:{int r;rt_recount_active();if((f->cs&3u)&&!user_range(a,sizeof(struct rt_start_request))){f->eax=(uint32_t)EXE_ERR_NAME;break;}if((f->cs&3u)&&safe_mode==2u){safe_deny_rt++;f->eax=SAFE_POLICY_DENIED;break;}r=rt_start_task(a,f);f->eax=(uint32_t)r;break;}
        case SYS_RT_STATUS:{
            if((f->cs&3u)==0u){f->eax=0xffffffffu;break;}
            if(b==RT_STATUS_STOP){
                /* Existing syscall 43 is extended without changing its number.
                   ECX selects one RT slot; 0xffffffff means all SENSOR tasks. */
                f->eax=rt_stop_tasks(c);
                break;
            }
            if(a&&user_rw_range(a,16u)){
                uint32_t i,active=0u;
                rt_recount_active();
                for(i=0u;i<RT_MAX_TASKS;i++)if((rt_tasks[i].state==TASK_READY||rt_tasks[i].state==TASK_RUNNING||rt_tasks[i].state==TASK_BLOCKED)&&rt_tasks[i].rt_period_ms!=0u)active++;
                ((uint32_t*)a)[0]=active;((uint32_t*)a)[1]=RT_MAX_TASKS-active;((uint32_t*)a)[2]=rt_background_active;((uint32_t*)a)[3]=rt_task_id;
                f->eax=0;
            }else f->eax=0xffffffffu;
            break;
        }
        case SYS_EXEC_ARGS:{int r;if(rt_find_current()>=0){f->eax=(uint32_t)EXE_ERR_BUSY;break;}if((f->cs&3u)&&(!user_cstr(a)||!copy_exec_args(b))){f->eax=(uint32_t)EXE_ERR_NAME;break;}mem_copy(exe_args, (const void*)b, EXEC_ARG_BLOCK_SIZE);r=exe_load_args((const char*)a,f,exe_args);f->eax=(uint32_t)r;break;}

        /* EXECMT tasks are ordinary EXE1 images, so existing SYS_EXIT must
           terminate the current scheduler task instead of falling through
           to the legacy single-EXE path.  Without this, QPASS/QPORT/etc.
           return -1 from SYS_EXIT and their final for(;;) hangs the system. */
        case SYS_EXIT:if(f->cs&3u){struct process_identity id=process_current_identity();if(id.type==PROCESS_TYPE_RT)rt_exit(f);else if(id.type==PROCESS_TYPE_MT)sched_exit(f);else if(id.type==PROCESS_TYPE_FG)exe_exit(f);else f->eax=0xffffffffu;}else f->eax=0xffffffffu;break;
        case SYS_UART_TRANSPORT:{uint32_t op=b,n=c,k;uint8_t*p=(uint8_t*)a;if(!(f->cs&3u)){f->eax=0xffffffffu;break;}if(op==0u){uint32_t*q=(uint32_t*)a;if(!q||!user_rw_range(a,10u*4u)){f->eax=0xffffffffu;break;}q[0]=(uint32_t)((uart_rx_head+UART_RX_CAP-uart_rx_tail)%UART_RX_CAP);q[1]=(uint32_t)((uart_tx_head+UART_TX_CAP-uart_tx_tail)%UART_TX_CAP);q[2]=uart_rx_bytes;q[3]=uart_tx_bytes;q[4]=uart_rx_overrun;q[5]=uart_line_errors;q[6]=uart_irq_count;q[7]=uart_last_rx_us;q[8]=uart_last_irq_us;q[9]=serial_time_us();f->eax=0u;break;}if(op==1u){if(!p||!n||n>UART_RX_CAP||!user_rw_range(a,n)){f->eax=0xffffffffu;break;}if(uart_transport_enabled){(void)uart1_poll_tx_hw();(void)uart1_poll_rx_hw();}f->eax=uart1_rx_pop(p,n);break;}if(op==2u){if(!p||!n||n>UART_TX_CAP||!user_range(a,n)){f->eax=0xffffffffu;break;}f->eax=uart1_tx_push(p,n);if(uart_transport_enabled)(void)uart1_poll_tx_hw();break;}if(op==3u){uart1_reset();f->eax=0u;break;}if(op==4u){f->eax=serial_time_us();break;}if(op==7u){if(a>1u){f->eax=0xffffffffu;break;}uart_transport_enabled=a;if(a){uart1_reset();outb(UART1_BASE+1u,0u);}else outb(UART1_BASE+1u,0u);f->eax=0u;break;}if(op==9u){f->eax=(uint32_t)inb(UART1_BASE+1u);break;}if(op==10u){f->eax=rt_time_ticks*10000u;break;}if(op==11u){/* FIX60P: non-destructive physical UART diagnostic snapshot; unlike legacy op0 it never latches PIT0. */uint32_t*q=(uint32_t*)a;if(!q||!user_rw_range(a,16u*4u)){f->eax=0xffffffffu;break;}q[0]=uart_transport_enabled;q[1]=(uint32_t)((uart_rx_head+UART_RX_CAP-uart_rx_tail)%UART_RX_CAP);q[2]=(uint32_t)((uart_tx_head+UART_TX_CAP-uart_tx_tail)%UART_TX_CAP);q[3]=uart_hw_rx_bytes;q[4]=uart_hw_tx_bytes;q[5]=uart_hw_poll_calls;q[6]=uart_hw_poll_count;q[7]=uart_rx_overrun;q[8]=uart_lsr_oe;q[9]=uart_lsr_pe;q[10]=uart_lsr_fe;q[11]=uart_lsr_bi;q[12]=uart_rx_max_queued;q[13]=uart_flush_hw_bytes;q[14]=uart_irq_count;q[15]=uart_last_lsr;f->eax=0u;break;}if(op==8u){/* FIX60I: transaction RX flush must clear both the software ring and any bytes already waiting in the physical 16550 FIFO.  Otherwise a late byte from the previous response can become byte 0 of the next Modbus frame.  Keep the drain bounded; IER remains zero. */uint32_t drained=0u;uart_rx_tail=uart_rx_head;if(uart_transport_enabled){while(drained<UART_RX_CAP){uint8_t lsr=inb(UART1_BASE+5u);if(!(lsr&1u))break;uart_note_lsr(lsr);(void)inb(UART1_BASE);drained++;uart_flush_hw_bytes++;}}f->eax=0u;break;}/* op5/op6 are deterministic FIX55 test hooks: inject RX and consume TX without external COM. */if(op==5u){if(!p||!n||n>UART_RX_CAP||!user_range(a,n)){f->eax=0xffffffffu;break;}for(k=0u;k<n;k++)uart1_rx_push(p[k]);f->eax=n;break;}if(op==6u){if(!p||!n||n>UART_TX_CAP||!user_rw_range(a,n)){f->eax=0xffffffffu;break;}k=0u;while(k<n&&uart_tx_tail!=uart_tx_head){p[k++]=uart_tx_buf[uart_tx_tail];uart_tx_tail=(uint16_t)((uart_tx_tail+1u)%UART_TX_CAP);uart_tx_bytes++;}if(uart_tx_tail==uart_tx_head&&uart_transport_enabled)uart1_set_tx_irq(0u);f->eax=k;break;}f->eax=0xffffffffu;}break;
        case SYS_DATA_CHANNEL:{
            uint32_t op=b,ch=c,i,h;struct data_channel*dc;struct data_reader*dr;
            if(!(f->cs&3u)){f->eax=0xffffffffu;break;}
            if(op==0u){uint32_t*q=(uint32_t*)a;if(ch>=DATA_CH_MAX||!q||!user_rw_range(a,7u*4u)){f->eax=0xffffffffu;break;}dc=&data_channels[ch];q[0]=dc->generation;q[1]=dc->next_sequence;q[2]=dc->count;q[3]=dc->published;q[4]=dc->overwrites;q[5]=DATA_CH_DEPTH;q[6]=DATA_CH_PAYLOAD;f->eax=0u;break;}
            if(op==1u){f->eax=data_begin(a);break;}
            if(op==2u){/* a -> [channel,status,length,payload(8 dwords)] */uint32_t*q=(uint32_t*)a,seq,idx,len;if(!q||!user_range(a,11u*4u)){f->eax=0xffffffffu;break;}ch=q[0];if(ch>=DATA_CH_MAX){f->eax=0xffffffffu;break;}dc=&data_channels[ch];if(dc->generation==0u){f->eax=0xffffffffu;break;}len=q[2];if(len>DATA_CH_PAYLOAD){f->eax=0xffffffffu;break;}seq=dc->next_sequence++;idx=(seq-1u)%DATA_CH_DEPTH;dc->ring[idx].generation=dc->generation;dc->ring[idx].sequence=seq;dc->ring[idx].timestamp_us=rt_time_ticks*10000u;/* FIX60N: bounded monotonic timestamp; Data Channel publish must not latch PIT0. */dc->ring[idx].status=q[1];dc->ring[idx].length=len;for(i=0u;i<DATA_CH_PAYLOAD;i++)dc->ring[idx].payload[i]=((uint8_t*)(q+3))[i];if(dc->count<DATA_CH_DEPTH)dc->count++;else dc->overwrites++;dc->published++;f->eax=seq;break;}
            if(op==3u){f->eax=data_reader_open(a,process_handle_owner());break;}
            if(op==4u){/* a -> 14 dwords output, c=reader handle */uint32_t*q=(uint32_t*)a,oldest,idx;if(ch==0u||ch>DATA_READER_MAX||!q||!user_rw_range(a,14u*4u)){f->eax=0xffffffffu;break;}dr=&data_readers[ch-1u];if(!dr->used){f->eax=0xffffffffu;break;}dc=&data_channels[dr->channel];if(dr->generation!=dc->generation){dr->generation=dc->generation;dr->next_sequence=data_oldest_sequence(dc);f->eax=DATA_READ_GENERATION;break;}oldest=data_oldest_sequence(dc);if(dr->next_sequence<oldest){dr->next_sequence=oldest;dr->overruns++;f->eax=DATA_READ_OVERRUN;break;}if(dr->next_sequence>=dc->next_sequence){f->eax=0u;break;}idx=(dr->next_sequence-1u)%DATA_CH_DEPTH;q[0]=dr->channel;q[1]=dc->ring[idx].generation;q[2]=dc->ring[idx].sequence;q[3]=dc->ring[idx].timestamp_us;q[4]=dc->ring[idx].status;q[5]=dc->ring[idx].length;for(i=0u;i<8u;i++)q[6u+i]=((uint32_t*)dc->ring[idx].payload)[i];dr->next_sequence++;f->eax=DATA_READ_SAMPLE;break;}
            if(op==5u){f->eax=data_reader_close(a);break;}
            if(op==6u){data_reset_all();f->eax=0u;break;}
            if(op==7u){uint32_t*q=(uint32_t*)a;if(ch==0u||ch>DATA_READER_MAX||!q||!user_rw_range(a,5u*4u)){f->eax=0xffffffffu;break;}dr=&data_readers[ch-1u];if(!dr->used){f->eax=0xffffffffu;break;}q[0]=dr->channel;q[1]=dr->generation;q[2]=dr->next_sequence;q[3]=dr->overruns;q[4]=dr->used;f->eax=0u;break;}
            f->eax=0xffffffffu;
        }break;
        case SYS_PORT_OUT8:if(a>0xffffu||b>0xffu){f->eax=0xffffffffu;break;}outb((uint16_t)a,(uint8_t)b);f->eax=0;break;
        case SYS_PORT_IN8:if(a>0xffffu){f->eax=0xffffffffu;break;}f->eax=(uint32_t)inb((uint16_t)a);break;
        case SYS_VIDEO_MAP:
            /* FIX32: sched_active only means that an MT session exists; it does
               not mean that the caller is an MT task.  A foreground EXE1 uses
               the normal page_directory and must be allowed to own VGA while
               detached MT tasks exist.  Check the actual caller address space
               instead of rejecting every active MT session. */
            if(!(f->cs&3u)||process_current_identity().type!=PROCESS_TYPE_FG||video_user_mapped){f->eax=0xffffffffu;break;}
            paging_set_video_user(1);
            f->eax=0;
            break;
        case SYS_VIDEO_UNMAP:
            /* Only the foreground EXE1 address space that can own the mapping
               may revoke it; RT/MT contexts have private CR3s. */
            if(!(f->cs&3u)||process_current_identity().type!=PROCESS_TYPE_FG||!video_user_mapped){f->eax=0xffffffffu;break;}
            paging_set_video_user(0);
            f->eax=0;
            break;
        case SYS_CONSOLE_AT:
            /* EBX = row<<16 | col, ECX = user buffer, EDX = length.
               Проверяем диапазон пользователя до любого чтения памяти. */
            if((f->cs&3u)&&!user_range(b,c)){f->eax=0xffffffffu;break;}
            if((a>>16)>=H||(a&0xffffu)>=W||c==0u||c>W-(a&0xffffu)){f->eax=0xffffffffu;break;}
            f->eax=console_write_at(a>>16,a&0xffffu,(const char*)b,c);
            break;
        case SYS_VIDEO_TEXT:
            /*
             * Графический Ring-3 драйвер не меняет режим обратно сам.
             * Это делает kernel одним неделимым действием: VGA -> text,
             * затем очищение B8000 и обновление аппаратного курсора.
             */
            /* FIX32: as with SYS_VIDEO_MAP, an existing MT session is not a
               reason to reject the foreground VGA owner. */
            if(!(f->cs&3u)||process_current_identity().type!=PROCESS_TYPE_FG||!video_user_mapped){f->eax=0xffffffffu;break;}
            if(!vga_restore_text_mode_hw()){f->eax=0xffffffffu;break;}
            vga_restore_saved_font();
            /* v67.4: после любого возврата из VGA начинаем новую текстовую
               сессию строго с верхнего левого угла. Предыдущее размещение
               курсора на H-2 оставляло boot/VGA остатки на экране и при первом
               `ls` могло визуально смешивать старый экран с новым выводом. */
            console_clear();
            vga_x=0;
            vga_y=0;
            vga_cursor_update();
            f->eax=0;
            break;
        case SYS_EXEC_ARG:
            /*
             * Получить один из трёх аргументов текущего EXE1 безопасно:
             * ядро само владеет сохранённой копией аргументного блока, поэтому
             * пользовательская программа больше не зависит от регистров EBX/ECX/EDX
             * в момент старта. EBX = индекс 0..2, ECX = user-буфер, EDX = ёмкость.
             * Ядро копирует максимум 16 байт вместе с завершающим NUL.
             */
            if(!(f->cs&3u)||!exe_active||a>=EXEC_ARG_COUNT||c<EXEC_ARG_SIZE||!user_rw_range(b,EXEC_ARG_SIZE)){f->eax=0xffffffffu;break;}
            mem_copy((void*)b,exe_args[a],EXEC_ARG_SIZE);
            f->eax=str_len((const char*)exe_args[a]);
            break;
        case SYS_EXEC_QUEUE:{int r;uint32_t from_exe=(f->cs&3u)&&exe_active;if((f->cs&3u)&&((b==0u)||(b>QUEUE_MAX_FILES)||!user_range(a,b*QUEUE_NAME_SIZE))){f->eax=0xffffffffu;break;}if(c>QUEUE_MAX_REPS){f->eax=0xffffffffu;break;}r=queue_prepare(a,b,c,f,from_exe);if(r!=EXE_OK){f->eax=(uint32_t)r;break;}if(from_exe){if(video_user_mapped)paging_set_video_user(0);exe_active=0;paging_set_exec_user(EXEC_LOAD_ADDR,exe_end,0);}queue_start_next(f);break;}
        case SYS_EXEC_QUEUE_ARGS:{int r;uint32_t from_exe=(f->cs&3u)&&exe_active;if((f->cs&3u)&&((b==0u)||(b>QUEUE_MAX_FILES)||!user_range(a,b*EXEC_QUEUE_RECORD_SIZE)||c>QUEUE_MAX_REPS)){f->eax=0xffffffffu;break;}r=queue_prepare_args(a,b,c,f,from_exe);if(r!=EXE_OK){f->eax=(uint32_t)r;break;}queue_start_next(f);break;}
        case SYS_QUEUE_STOP:if(queue_active){queue_stop_requested=1;if((f->cs&3u)&&exe_active)queue_finish(f,0xffffffffu);else f->eax=0;}else f->eax=0xffffffffu;break;
        default:f->eax=0xffffffffu;break;}
    }
    if(n>=32&&n<48)irq_eoi(n);}

extern void zero_bss(void);
extern void user_entry(void);

void kernel_main(void){
    char b[11];
    char sample[96];
    static uint8_t test_out[700];
    static uint8_t test_in[700];
    uint32_t mem,got=0,i,wr;
    int fd;

    zero_bss();
    /* Сохраняем BIOS/VGA 8x16 font до первого перехода в graphics mode. */
    vga_save_font();
    mem=*(volatile uint32_t*)0xB8FF0;
    console_clear();
    console_write("Toy OS kernel\nRAM: ");
    u32_dec(mem,b);console_write(b);console_write(" bytes\n");
    console_write("IDT/PIC/PIT(50Hz sys, 100Hz RT)/KBD/UART/ATA/FAT16 init...\n");

    idt_init();
    gdt_init_ring3();
    /* PIC must be remapped before any hardware IRQ can be enabled. */
    pic_remap();
    paging_init();
    pit_init();
    keyboard_init();
    uart1_init();
    irq_enable(0);
    irq_enable(1);
    /* FIX60N: syscall-70 physical transport is fully polled. Keep PIC IRQ4
       masked as well as UART IER=0, so COM1 cannot asynchronously enter the
       kernel through vector 36. The handler remains for ABI/history only. */
    /* ATA is a polling driver; keep IRQ14 masked. This prevents an ATA
       completion interrupt from re-entering the common ISR while a PIO
       command is still being handled synchronously. */

    layout_valid=layout_validate();
    if(!layout_valid){console_write("LAYOUT: invalid descriptor\n");for(;;)__asm__ volatile("cli;hlt");}
    if(fat_mount(layout_word(9))&&fat.part==layout_word(9)&&fat.total_sectors==layout_word(10)){char lb[16];console_write("FAT16 mounted at dynamic LBA ");u32_dec(layout_word(9),lb);console_write(lb);console_write("\n");}
    else{console_write("FAT16/layout mismatch\n");for(;;)__asm__ volatile("cli;hlt");}
    /* Keep interrupts disabled while the kernel performs its synchronous FAT/ATA self-test.
       The shell enables interrupts only after all kernel initialization is complete. */
    sys_console_write("syscall console: OK\n",20);

    if(fat_read_file("README.TXT",sample,sizeof(sample)-1,&got)){
        sample[got]=0;
        console_write("README.TXT: ");
        console_write(sample);
        if(got&&sample[got-1]!='\n')console_write("\n");
    }

    for(i=0;i<sizeof(test_out);i++)test_out[i]=(uint8_t)('A'+(i%26u));
    fd=(int)sys_file_open("TEST.TXT",FAT16_MODE_READ|FAT16_MODE_WRITE|FAT16_MODE_CREATE|FAT16_MODE_TRUNC);
    if(fd>=0){
        wr=sys_file_write((uint32_t)fd,test_out,sizeof(test_out));
        sys_file_close((uint32_t)fd);
        console_write("File write 700B: ");
        if(wr==sizeof(test_out))console_write("OK\n");else console_write("FAIL\n");

        fd=(int)sys_file_open("TEST.TXT",FAT16_MODE_READ);
        if(fd>=0){
            uint32_t rd=sys_file_read((uint32_t)fd,test_in,sizeof(test_in));
            sys_file_close((uint32_t)fd);
            for(i=0;i<sizeof(test_in)&&test_in[i]==test_out[i];i++){}
            console_write("File read/verify: ");
            if(rd==sizeof(test_in)&&i==sizeof(test_in))console_write("OK\n");else console_write("FAIL\n");
        }
    }else console_write("File open: FAIL\n");

    /* Only now are timer/keyboard interrupts allowed to preempt the kernel. */
    __asm__ volatile("sti" ::: "memory");
    console_write("Timer ticks: ");
    console_write_u32(sys_timer_get());
    console_write("\nReady. Entering ring-3 shell.\n");
    enter_user_shell();
    for(;;)cpu_hlt();
}
__asm__(
".section .start,\"ax\"\n"
".global _start\n"
"_start:\n"
"jmpl $0x08, $_kernel_main\n"
);
