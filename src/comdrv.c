/*
 * COMDRV.EXE1 — первый пользовательский драйвер последовательного порта Toy OS.
 *
 * Интерфейс программы (ровно три параметра, передаваемые ядром в EBX/ECX/EDX):
 *   EBX = номер COM-порта: 1..4
 *   ECX = направление: SEND или RECV
 *   EDX = имя файла 8.3:
 *         SEND -> существующий файл читается полностью и передаётся;
 *         RECV -> файл создаётся автоматически (или очищается) и принимает
 *                 все принятые байты до тайм-аута линии.
 *
 * UART настраивается в 115200, 8N1. Прямые IN/OUT запрещены Ring-3:
 * каждый доступ к регистру UART проходит через SYS_PORT_IN8/SYS_PORT_OUT8
 * (INT 80h, syscall 14/13). Это делает COMDRV первым драйвером ОС, но сохраняет
 * привилегированную границу IOPL=0.
 *
 * Приём не может зависнуть навсегда: после появления хотя бы одного байта
 * программа завершает сеанс после 1 секунды тишины. Если байтов не пришло,
 * действует общий предел 10 секунд. Файл при RECV создаётся до ожидания данных.
 */
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
#define SYS_CONSOLE_WRITE 1u
#define SYS_TIMER_GET 3u
#define SYS_FILE_OPEN 6u
#define SYS_FILE_READ 7u
#define SYS_FILE_WRITE 8u
#define SYS_FILE_CLOSE 9u
#define SYS_EXIT 12u
#define SYS_PORT_OUT8 13u
#define SYS_PORT_IN8 14u
#define FAT16_MODE_READ 0x01u
#define FAT16_MODE_WRITE 0x02u
#define FAT16_MODE_CREATE 0x04u
#define FAT16_MODE_TRUNC 0x08u

static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}
static void put(const char*s){uint32_t n=0;while(s[n])n++;sc(SYS_CONSOLE_WRITE,(uint32_t)s,n,0);}
static uint32_t ticks(void){return sc(SYS_TIMER_GET,0,0,0);}
static uint32_t out8(uint32_t p,uint32_t v){return sc(SYS_PORT_OUT8,p,v,0);}
static uint32_t in8(uint32_t p){return sc(SYS_PORT_IN8,p,0,0);}
static uint32_t base_for(uint32_t n){static const uint16_t b[4]={0x3f8,0x2f8,0x3e8,0x2e8};return n>=1&&n<=4?b[n-1]:0;}
/* v67.3: параметры COMDRV с текстовым смыслом нормализуются к ASCII upper-case.
   Это не меняет имя файла/данные пользователя, а только канонизирует COM1..COM4
   и направление SEND/RECV перед разбором. Так прямой ABI EXE1 и запуск через
   shell имеют одинаковое регистронезависимое поведение. */
static void upper_copy(char*d,const char*s,uint32_t cap){uint32_t i=0;
    if(!d||cap==0u)return;
    if(!s){d[0]=0;return;}
    while(s[i]&&i+1u<cap){char c=s[i];if(c>='a'&&c<='z')c=(char)(c-'a'+'A');d[i++]=c;}
    d[i]=0;
}
static int eq4(const char*a,const char*b){return a[0]==b[0]&&a[1]==b[1]&&a[2]==b[2]&&a[3]==b[3]&&a[4]==0;}
static void dec(uint32_t v,char*b){char t[11];uint32_t n=0;do{t[n++]=(char)('0'+v%10u);v/=10u;}while(v);while(n)*b++=t[--n];*b=0;}
static int uart_init(uint32_t base){
    /* IER=0: драйвер использует polling, IRQ COM не нужен. */
    if(out8(base+1,0)!=0)return 0;
    /* LCR.DLAB=1, divisor=1 -> 115200 при стандартных 1.8432 MHz. */
    if(out8(base+3,0x80)!=0)return 0;
    if(out8(base+0,1)!=0)return 0;
    if(out8(base+1,0)!=0)return 0;
    /* 8 data bits, no parity, one stop bit = 8N1. */
    if(out8(base+3,0x03)!=0)return 0;
    /* FIFO enable, clear RX/TX, 14-byte threshold. */
    if(out8(base+2,0xC7)!=0)return 0;
    /* DTR + RTS + OUT2. */
    if(out8(base+4,0x0B)!=0)return 0;
    return 1;
}
static int uart_write_byte(uint32_t base,uint8_t c,uint32_t deadline){
    while(ticks()<deadline){uint32_t s=in8(base+5);if(s!=0xffffffffu&&(s&0x20u)){return out8(base,c)==0;}}
    return 0;
}
static int uart_read_byte(uint32_t base,uint8_t*out){uint32_t s=in8(base+5),v;if(s==0xffffffffu)return 0;if(!(s&1u))return 0;v=in8(base);if(v==0xffffffffu)return 0;*out=(uint8_t)v;return 1;}
static int send_file(uint32_t base,const char*name){
    uint8_t buf[512];uint32_t fd,n,i,deadline;
    fd=sc(SYS_FILE_OPEN,(uint32_t)name,FAT16_MODE_READ,0);if(fd==0xffffffffu){put("COMDRV: input file open failed\n");return 0;}
    for(;;){n=sc(SYS_FILE_READ,fd,(uint32_t)buf,sizeof(buf));if(n==0xffffffffu){sc(SYS_FILE_CLOSE,fd,0,0);put("COMDRV: file read failed\n");return 0;}if(n==0)break;for(i=0;i<n;i++){deadline=ticks()+250u;if(!uart_write_byte(base,buf[i],deadline)){sc(SYS_FILE_CLOSE,fd,0,0);put("COMDRV: TX timeout\n");return 0;}}}
    sc(SYS_FILE_CLOSE,fd,0,0);put("COMDRV: send complete\n");return 1;
}
static int receive_file(uint32_t base,const char*name){
    uint8_t buf[512];uint32_t fd,n=0,got=0,last,limit;
    fd=sc(SYS_FILE_OPEN,(uint32_t)name,FAT16_MODE_WRITE|FAT16_MODE_CREATE|FAT16_MODE_TRUNC,0);if(fd==0xffffffffu){put("COMDRV: output file create failed\n");return 0;}
    last=ticks();limit=last+500u; /* 10 seconds at 50 Hz: ожидание первого байта */
    while(ticks()<limit){
        uint8_t c;
        if(uart_read_byte(base,&c)){buf[n++]=c;got++;last=ticks();limit=last+50u; /* 1 s idle timeout after first byte */ if(n==sizeof(buf)){if(sc(SYS_FILE_WRITE,fd,(uint32_t)buf,n)!=n){sc(SYS_FILE_CLOSE,fd,0,0);put("COMDRV: file write failed\n");return 0;}n=0;}}
    }
    if(n&&sc(SYS_FILE_WRITE,fd,(uint32_t)buf,n)!=n){sc(SYS_FILE_CLOSE,fd,0,0);put("COMDRV: file write failed\n");return 0;}
    sc(SYS_FILE_CLOSE,fd,0,0);
    put("COMDRV: receive complete, bytes=");
    {char b[12];dec(got,b);put(b);}put("\n");
    return 1;
}
static uint32_t parse_port(const char*port){
    /* Разрешены только четыре стандартных обозначения: 1..4 и COM1..COM4.
       Неинтерпретируемые строки не должны превращаться в произвольный номер. */
    if(!port||!port[0])return 0;
    if(port[0]>='1'&&port[0]<='4'&&port[1]==0)return (uint32_t)(port[0]-'0');
    if(port[0]=='C'&&port[1]=='O'&&port[2]=='M'&&port[3]>='1'&&port[3]<='4'&&port[4]==0)
        return (uint32_t)(port[3]-'0');
    return 0;
}

__attribute__((section(".usertext"))) void program_main(void){
    /* Startup ABI supplied by kernel: EBX/ECX/EDX point into EXEC_STACK_PAGE. */
    const char*port;
    const char*dir;
    const char*file;
    char port_norm[16],dir_norm[16];
    uint32_t ok=0;
    /* Один asm-блок с явными constraints сохраняет ABI параметров EXE1. */
    __asm__ volatile("" : "=b"(port), "=c"(dir), "=d"(file));
    /* Проверяем наличие всех трёх обязательных аргументов. */
    if(!port||!port[0]||!dir||!dir[0]||!file||!file[0]){
        put("COMDRV: usage PORT SEND|RECV FILE\n");
        sc(SYS_EXIT,1,0,0);
        for(;;){}
    }
    /* v67.3: нормализуем текстовые параметры перед их разбором. */
    upper_copy(port_norm,port,sizeof(port_norm));
    upper_copy(dir_norm,dir,sizeof(dir_norm));
    /* Преобразуем номер COM1..COM4 в базовый адрес UART. */
    {
        uint32_t portno=parse_port(port_norm);
        uint32_t base=base_for(portno);
        /* Отбрасываем недопустимый номер COM-порта до обращения к I/O. */
        if(!portno||!base){
            put("COMDRV: invalid COM port\n");
            sc(SYS_EXIT,1,0,0);
            for(;;){}
        }
        /* Настраиваем UART перед началом обмена. */
        if(!uart_init(base)){
            put("COMDRV: UART init failed\n");
            sc(SYS_EXIT,1,0,0);
            for(;;){}
        }
        /* Выполняем выбранное направление и сохраняем его итог. */
        if(eq4(dir_norm,"SEND")){
            ok=send_file(base,file)?1u:0u;
        }else if(eq4(dir_norm,"RECV")){
            ok=receive_file(base,file)?1u:0u;
        }else{
            put("COMDRV: direction must be SEND or RECV\n");
            ok=0;
        }
    }
    /* v57: ненулевой код завершения останавливает очередь LOADER при ошибке. */
    sc(SYS_EXIT,ok?0u:1u,0,0);
    /* SYS_EXIT при нормальной работе не возвращается в этот EXE1. */
    for(;;){}
}
__asm__(".section .start,\"ax\"\n.global _start\n_start:\njmp _program_main\n");
