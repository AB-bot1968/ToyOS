/*
 * VGADRV.EXE1 — пользовательский VGA-драйвер Toy OS v65.
 *
 * Назначение:
 *   exec VGADRV.EXE FILE
 *
 * FILE — обычный файл FAT16 размером ровно 64000 байт. Байты файла являются
 * индексами 256-цветной палитры для VGA Mode 13h (320x200, 8 bit/pixel).
 *
 * Архитектура намеренно не содержит копии картинки в kernel:
 *   1. kernel загружает VGADRV.EXE как обычный Ring-3 EXE1;
 *   2. драйвер открывает FILE через существующий FAT16 syscall API;
 *   3. драйвер запрашивает временный user-доступ к VGA VRAM 0xA0000;
 *   4. VGA-регистры программируются через существующий SYS_PORT_OUT8;
 *   5. файл читается блоками по 512 байт прямо в user-буфер и копируется
 *      в framebuffer;
 *   6. по нажатию клавиши драйвер запрашивает у kernel возврат в текстовый
 *      80x25 через SYS_VIDEO_TEXT; kernel одновременно очищает B8000;
 *   7. доступ к VRAM снимается и драйвер завершает EXE1 через SYS_EXIT.
 *
 * Прямые IN/OUT инструкции из Ring 3 здесь намеренно не используются.
 * Это сохраняет существующую модель Toy OS с IOPL=0: доступ к I/O-портам
 * проходит через уже существующие системные вызовы 13/14.
 */
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

#define SYS_CONSOLE_WRITE 1u
#define SYS_CONSOLE_READ  2u
#define SYS_FILE_OPEN     6u
#define SYS_FILE_READ     7u
#define SYS_FILE_CLOSE    9u
#define SYS_EXIT         12u
#define SYS_PORT_OUT8    13u
#define SYS_PORT_IN8     14u
#define SYS_VIDEO_MAP    32u
#define SYS_VIDEO_UNMAP  33u
#define SYS_EXEC_ARG     34u
#define SYS_VIDEO_TEXT   35u
#define FAT16_MODE_READ  0x01u
#define VGA_WIDTH        320u
#define VGA_HEIGHT       200u
#define IMAGE_WIDTH      320u
#define IMAGE_HEIGHT     200u
#define IMAGE_SIZE       (IMAGE_WIDTH*IMAGE_HEIGHT)
#define VGA_CRTC_INDEX   0x3d4u
#define VGA_CRTC_DATA    0x3d5u
#define VGA_SEQ_INDEX    0x3c4u
#define VGA_SEQ_DATA     0x3c5u
#define VGA_GC_INDEX     0x3ceu
#define VGA_GC_DATA      0x3cfu
#define VGA_ATTR_INDEX   0x3c0u
#define VGA_MISC_WRITE   0x3c2u
#define VGA_DAC_WRITE    0x3c8u
#define VGA_DAC_DATA     0x3c9u
#define VGA_INPUT_STATUS 0x3dau
#define SYS_FAIL         0xffffffffu
#define EXEC_ARG_SIZE    16u

static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){
    uint32_t r;
    __asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");
    return r;
}
static void put(const char*s){uint32_t n=0;while(s[n])n++;sc(SYS_CONSOLE_WRITE,(uint32_t)s,n,0);}
static uint32_t out8(uint32_t p,uint32_t v){return sc(SYS_PORT_OUT8,p,v,0);}
static uint32_t file_open(const char*n){return sc(SYS_FILE_OPEN,(uint32_t)n,FAT16_MODE_READ,0);}
static uint32_t file_read(uint32_t fd,void*b,uint32_t n){return sc(SYS_FILE_READ,fd,(uint32_t)b,n);}
static uint32_t file_close(uint32_t fd){return sc(SYS_FILE_CLOSE,fd,0,0);}
static void fail_and_exit(const char*s){
    /* v67.4: любой аварийный выход после SYS_VIDEO_MAP обязан сначала
       вернуть аппаратный VGA в text mode. Ранее ошибка открытия IMAGE
       печаталась ещё в Mode 13h, затем SYS_EXIT просто возвращал shell,
       оставляя видеоконтроллер в графическом режиме. Это давало исчезновение
       заставки и последующие артефакты/мусор в текстовой консоли. */
    sc(SYS_VIDEO_TEXT,0,0,0);
    sc(SYS_VIDEO_UNMAP,0,0,0);
    put(s);
    sc(SYS_EXIT,1,0,0);
    for(;;){}
}

/* Запись CRTC-регистра. Отдельная функция уменьшает вероятность перепутать
 * адрес index/data при ручном изменении таблиц режима. */
static int crtc(uint8_t index,uint8_t data){
    if(out8(VGA_CRTC_INDEX,index)!=0)return 0;
    return out8(VGA_CRTC_DATA,data)==0;
}
static int sequencer(uint8_t index,uint8_t data){
    if(out8(VGA_SEQ_INDEX,index)!=0)return 0;
    return out8(VGA_SEQ_DATA,data)==0;
}
static int graphics(uint8_t index,uint8_t data){
    if(out8(VGA_GC_INDEX,index)!=0)return 0;
    return out8(VGA_GC_DATA,data)==0;
}
static int attr(uint8_t index,uint8_t data){
    if(sc(SYS_PORT_IN8,VGA_INPUT_STATUS,0,0)==SYS_FAIL)return 0;
    if(out8(VGA_ATTR_INDEX,index)!=0)return 0;
    return out8(VGA_ATTR_INDEX,data)==0;
}

/*
 * Mode 13h: стандартная VGA 320x200x256. Таблицы соответствуют классической
 * IBM/VGA раскладке регистра режима. Все обращения выполняются через kernel
 * syscall, потому что текущий Toy OS намеренно работает с IOPL=0.
 */
static int set_mode13(void){
    static const uint8_t s[5]={0x03,0x01,0x0f,0x00,0x0e};
    static const uint8_t c[25]={0x5f,0x4f,0x50,0x82,0x54,0x80,0xbf,0x1f,0x00,0x41,0x00,0x00,0x00,0x00,0x00,0x00,0x9c,0x8e,0x8f,0x28,0x40,0x96,0xb9,0xa3,0xff};
    static const uint8_t g[9]={0x00,0x00,0x00,0x00,0x00,0x40,0x05,0x0f,0xff};
    static const uint8_t a[21]={0x00,0x01,0x02,0x03,0x04,0x05,0x14,0x07,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x41,0x00,0x0f,0x00,0x00};
    uint32_t i;
    if(out8(VGA_MISC_WRITE,0x63)!=0)return 0;
    if(!sequencer(0,0x01))return 0;
    for(i=1;i<5;i++)if(!sequencer((uint8_t)i,s[i]))return 0;
    if(!sequencer(0,0x03))return 0;
    if(!crtc(0x11,0x00))return 0; /* unlock CRTC registers 0..7 */
    for(i=0;i<25;i++)if(!crtc((uint8_t)i,c[i]))return 0;
    for(i=0;i<9;i++)if(!graphics((uint8_t)i,g[i]))return 0;
    for(i=0;i<21;i++)if(!attr((uint8_t)i,a[i]))return 0;
    if(out8(VGA_ATTR_INDEX,0x20)!=0)return 0; /* display enable */
    return 1;
}

/*
 * Возврат в стандартный цветной текстовый режим 80x25. Нам не нужен BIOS:
 * режим восстанавливается тем же VGA-регистровым механизмом из Ring 3.
 */
/* Палитра 6-bit VGA: строим RGB 6x6x6 colour cube. Индекс 0..215 получает
 * 36 оттенков на каждый канал; 216..255 оставляем градациями серого. */
static int set_palette(void){
    uint32_t i,r,g,b;
    if(out8(VGA_DAC_WRITE,0)!=0)return 0;
    for(i=0;i<216u;i++){
        r=(i/36u)*63u/5u;g=((i/6u)%6u)*63u/5u;b=(i%6u)*63u/5u;
        if(out8(VGA_DAC_DATA,r)!=0||out8(VGA_DAC_DATA,g)!=0||out8(VGA_DAC_DATA,b)!=0)return 0;
    }
    for(i=216;i<256u;i++){
        uint32_t v=(i-216u)*63u/39u;
        if(out8(VGA_DAC_DATA,v)!=0||out8(VGA_DAC_DATA,v)!=0||out8(VGA_DAC_DATA,v)!=0)return 0;
    }
    return 1;
}

static void copy_image(const uint8_t*b,uint32_t offset,uint32_t n){
    /*
     * v65.8 использует нативный размер VGA Mode 13h: 320x200x256.
     * Поэтому один байт SPLASH.RAW соответствует одному пикселю VRAM.
     * Никакого 2x2 увеличения больше нет: мелкий текст заставки не
     * растягивается и сохраняется в исходной детализации 320x200.
     */
    volatile uint8_t *v=(volatile uint8_t*)0x000a0000u;
    uint32_t i;
    for(i=0u;i<n;i++)v[offset+i]=b[i];
}

__attribute__((section(".usertext"))) void program_main(void){
    uint8_t buffer[512];
    char file[EXEC_ARG_SIZE],file_norm[EXEC_ARG_SIZE];
    uint32_t fd,n,total=0,arg_len;
    /*
     * Имя файла берём через SYS_EXEC_ARG(2), а не из EBX/ECX/EDX.
     * Ядро уже сохранило аргументы EXE1 в защищённом блоке запуска, поэтому
     * это надёжный ABI, не зависящий от того, какие регистры портит startup-код.
     */
    arg_len=sc(SYS_EXEC_ARG,0u,(uint32_t)file,EXEC_ARG_SIZE);
    if(arg_len==SYS_FAIL||arg_len==0u||arg_len>=EXEC_ARG_SIZE){
        fail_and_exit("VGADRV: usage exec VGADRV.EXE FILE\n");
    }
    /* v67.3: имя ресурсного файла нормализуется к ASCII upper-case перед
       SYS_FILE_OPEN. FAT16 уже регистронезависима, но явная нормализация
       делает поведение VGADRV одинаковым при любом регистре аргумента и
       не затрагивает содержимое файла. */
    {uint32_t i;for(i=0;i<sizeof(file_norm)-1u&&file[i];i++){char c=file[i];if(c>='a'&&c<='z')c=(char)(c-'a'+'A');file_norm[i]=c;}file_norm[i]=0;}
    /* v67.4: ресурс заставки открывается относительно ROOT. AUTOSTART.SH
       запускается из ROOT, но shell cwd не должен становиться скрытой
       зависимостью графического драйвера. */
    {char image_path[EXEC_ARG_SIZE];uint32_t i=0;
     image_path[0]='/';
     while(i<EXEC_ARG_SIZE-2u&&file_norm[i]){image_path[i+1u]=file_norm[i];i++;}
     image_path[i+1u]=0;
     fd=file_open(image_path);
     /* Совместимость с существующими интерактивными вызовами: если абсолютный
        вариант не найден, повторяем обычный относительный поиск. */
     if(fd==SYS_FAIL)fd=file_open(file_norm);
    }
    if(fd==SYS_FAIL)fail_and_exit("VGADRV: image open failed\n");
    if(sc(SYS_VIDEO_MAP,0,0,0)!=0){file_close(fd);fail_and_exit("VGADRV: VRAM map failed\n");}
    if(!set_mode13()||!set_palette()){
        file_close(fd);sc(SYS_VIDEO_TEXT,0,0,0);sc(SYS_VIDEO_UNMAP,0,0,0);
        fail_and_exit("VGADRV: VGA mode setup failed\n");
    }
    while(total<IMAGE_SIZE){
        uint32_t want=IMAGE_SIZE-total;
        if(want>sizeof(buffer))want=sizeof(buffer);
        n=file_read(fd,buffer,want);
        if(n==SYS_FAIL||n==0){file_close(fd);sc(SYS_VIDEO_TEXT,0,0,0);sc(SYS_VIDEO_UNMAP,0,0,0);fail_and_exit("VGADRV: image read failed or truncated\n");}
        copy_image(buffer,total,n);
        total+=n;
    }
    /* Если файл длиннее 64000 байт, это невалидный ресурс. */
    n=file_read(fd,buffer,1);
    file_close(fd);
    if(n!=0){sc(SYS_VIDEO_TEXT,0,0,0);sc(SYS_VIDEO_UNMAP,0,0,0);fail_and_exit("VGADRV: image has trailing data\n");}
    /* Никакого вывода в B8000 здесь нет: экран ещё находится в Mode 13h.
       Инструкция «Press any key...» уже является частью SPLASH.RAW. */
    {
        char key;
        uint32_t rr;
        /* FIX32: SYS_CONSOLE_READ may return EAX=2 after an internal RT/MT
           scheduler hand-off.  That is not keyboard input.  Previously
           VGADRV treated this scheduler return as "any key", so with an RT
           task the splash could disappear almost immediately. */
        do { rr=sc(SYS_CONSOLE_READ,(uint32_t)&key,1,0); } while(rr==2u);
        if(rr==SYS_FAIL)fail_and_exit("VGADRV: console read failed\n");
    }
    if(sc(SYS_VIDEO_TEXT,0,0,0)!=0){
        sc(SYS_VIDEO_UNMAP,0,0,0);
        fail_and_exit("VGADRV: text mode restore failed\n");
    }
    sc(SYS_VIDEO_UNMAP,0,0,0);
    /* v67.6: после SYS_VIDEO_TEXT kernel начинает текстовый сеанс с курсором
       (0,0). Сообщение об успешном возврате из графического режима должно
       находиться на предпоследней строке, а shell prompt — в левом нижнем
       углу на последней строке. В режиме 80x25 это строки 23 и 24.
       Двадцать три LF переводят курсор с строки 0 на строку 23. После
       `VGADRV: OK ` выполняется один LF, поэтому следующий `toy0> ` shell
       начинается на строке 24. Это не требует изменения syscall ABI. */
    {uint32_t i;for(i=0u;i<23u;i++)put("\n");}
    put("VGADRV: OK \n");
    sc(SYS_EXIT,0,0,0);
    for(;;){}
}

__asm__(
".section .start,\"ax\"\n"
".global _start\n"
"_start:\n"
"/* Аргументы запуска получаются через SYS_EXEC_ARG; регистровый ABI больше не нужен. */\n"
"jmp _program_main\n"
);
