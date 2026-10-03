/*
 * NETDRV.EXE1 — сетевой драйвер/клиент Toy OS v66.11.
 *
 * Аппаратная модель:
 *   QEMU NE2000 ISA, базовый I/O-адрес по умолчанию 0x300.
 *   Доступ к портам выполняется только через SYS_PORT_IN8/SYS_PORT_OUT8,
 *   поэтому EXE1 не требует изменения IOPL и не выполняет IN/OUT напрямую.
 *
 * Сетевой стек намеренно минимален:
 *   Ethernet -> ARP -> IPv4 -> TCP -> HTTP/1.0, а v66 дополнительно UDP/IPv4.
 *
 * Поддерживаются TCP-операции и отдельный минимальный режим UDP-телеметрии.
 *
 * UDP-телеметрия:
 *   exec NETDRV.EXE TELEMETRY SLAVE1 192.168.100.10
 *   exec NETDRV.EXE TELEMETRY SLAVE2 192.168.100.10
 *   exec NETDRV.EXE TELEMETRY MASTER 5000
 *
 * Порт телеметрии фиксирован: UDP/5000. Slave отправляет одну короткую
 * строку состояния каждую секунду. Master принимает обе строки и отображает
 * их в двух фиксированных строках VGA, поэтому история значений не накапливается.
 *
 * Поддерживаются две TCP-операции:
 *
 *   exec NETDRV.EXE IP SEND FILE.TXT
 *       FILE.TXT отправляется на http://IP:PORT/PATH методом POST.
 *
 *   exec NETDRV.EXE IP RECV FILE.TXT
 *       выполняется GET http://IP:PORT/PATH, тело HTTP-ответа
 *       сохраняется в FILE.TXT.
 *
 * Настройка берётся только из фиксированного файла NET.CFG.
 * Параметры команды задаются shell и передаются через существующий
 * ABI EBX/ECX/EDX:
 *   EBX = IP назначения;
 *   ECX = SEND или RECV;
 *   EDX = имя файла.
 *
 * Формат NET.CFG:
 *
 *   BASE=300
 *   MAC=52:54:00:12:34:56
 *   IP=10.0.2.15
 *   MASK=255.255.255.0
 *   GATEWAY=10.0.2.2
 *   PORT=8080
 *   PATH=/
 *   TIMEOUT=10
 *
 * BASE допускается в hex без префикса: 300, 320 и т.п.
 * MAC нужен для установки физического адреса NE2000.
 * IP/MASK/GATEWAY задают локальную IPv4-конфигурацию.
 * PORT — TCP-порт HTTP-сервера.
 * PATH — URI без имени хоста.
 * TIMEOUT — общий тайм-аут ожидания одного этапа, секунды.
 *
 * Ограничения намеренно простого варианта:
 *   - только Ethernet/IPv4 без VLAN;
 *   - только ARP request/reply;
 *   - только TCP-клиент, одно соединение;
 *   - HTTP/1.0, без chunked transfer encoding;
 *   - RECV сохраняет тело после первого "\r\n\r\n";
 *   - SEND использует HTTP POST и Content-Length;
 *   - DNS, ICMP, IPv6, TCP window > 1 сегмента и повторная передача
 *     сложных TCP-состояний не реализованы.
 *
 * Это учебный минимальный клиент, а не универсальный интернет-стек.
 */
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

/* ----------------------------- Syscalls ----------------------------- */

#define SYS_CONSOLE_WRITE 1u
#define SYS_TIMER_GET     3u
#define SYS_FILE_OPEN     6u
#define SYS_FILE_READ     7u
#define SYS_FILE_WRITE    8u
#define SYS_FILE_CLOSE    9u
#define SYS_FILE_SIZE    17u
#define SYS_EXIT         12u
#define SYS_PORT_OUT8    13u
#define SYS_PORT_IN8     14u
#define SYS_CONSOLE_AT   36u
#define SYS_CONSOLE_POLL 37u

#define FAT16_MODE_READ   0x01u
#define FAT16_MODE_WRITE  0x02u
#define FAT16_MODE_CREATE 0x04u
#define FAT16_MODE_TRUNC  0x08u

#define EXE_OK             0u
#define EXE_FAIL            1u
#define SYS_FAIL   0xffffffffu

static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){
    uint32_t r;
    __asm__ volatile("int $0x80"
        :"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");
    return r;
}

static void put(const char*s){
    uint32_t n=0;
    while(s[n])n++;
    sc(SYS_CONSOLE_WRITE,(uint32_t)s,n,0);
}

static uint32_t ticks(void){
    return sc(SYS_TIMER_GET,0,0,0);
}

static uint32_t out8(uint32_t p,uint32_t v){
    return sc(SYS_PORT_OUT8,p,v,0);
}

static uint32_t in8(uint32_t p){
    return sc(SYS_PORT_IN8,p,0,0);
}

/* ----------------------------- Utility ------------------------------ */



/* v67.3: нормализуем текстовые параметры NETDRV перед сравнением.
   IP-адрес и числовые значения не изменяются. Нормализация применяется к
   TELEMETRY, MASTER/SLAVE1/SLAVE2 и SEND/RECV, чтобы прямой EXE1 ABI и shell
   одинаково принимали любой ASCII-регистр. */
static void upper_copy(char*d,const char*s,uint32_t cap){uint32_t i=0;
    if(!d||cap==0u)return;
    if(!s){d[0]=0;return;}
    while(s[i]&&i+1u<cap){char c=s[i];if(c>='a'&&c<='z')c=(char)(c-'a'+'A');d[i++]=c;}
    d[i]=0;
}

static int eq(const char*a,const char*b){
    uint32_t i=0;
    while(a[i]&&b[i]){
        if(a[i]!=b[i])return 0;
        i++;
    }
    return a[i]==0&&b[i]==0;
}



static void dec(uint32_t v,char*b){
    char t[11];
    uint32_t n=0;
    do{
        t[n++]=(char)('0'+v%10u);
        v/=10u;
    }while(v);
    while(n)*b++=t[--n];
    *b=0;
}

/* Преобразование десятичного числа. Возвращает 0 при синтаксической ошибке. */
static uint32_t number(const char*s,int*ok){
    uint32_t v=0,i=0;
    *ok=0;
    if(!s[0])return 0;
    while(s[i]>='0'&&s[i]<='9'){
        if(v>429496729u)return 0;
        v=v*10u+(uint32_t)(s[i]-'0');
        i++;
    }
    if(s[i]!=0)return 0;
    *ok=1;
    return v;
}

/* HEX-число для BASE: "300" -> 0x300. */
static uint32_t hex_number(const char*s,int*ok){
    uint32_t v=0,i=0,d;
    *ok=0;
    if(!s[0])return 0;
    while(s[i]){
        char c=s[i++];
        if(c>='0'&&c<='9')d=(uint32_t)(c-'0');
        else if(c>='a'&&c<='f')d=(uint32_t)(c-'a'+10);
        else if(c>='A'&&c<='F')d=(uint32_t)(c-'A'+10);
        else return 0;
        if(v>0x0fffffffU)return 0;
        v=(v<<4)|d;
    }
    *ok=1;
    return v;
}

static int parse_ip(const char*s,uint8_t*out){
    uint32_t i=0,part=0,n=0;
    while(1){
        if(s[i]>='0'&&s[i]<='9'){
            if(part>255u)return 0;
            part=part*10u+(uint32_t)(s[i]-'0');
            if(part>255u)return 0;
            i++;
            continue;
        }
        if(s[i]=='.'||s[i]==0){
            if(n>=4)return 0;
            out[n++]=(uint8_t)part;
            part=0;
            if(s[i]==0)break;
            i++;
            continue;
        }
        return 0;
    }
    return n==4;
}

static int parse_mac(const char*s,uint8_t*out){
    uint32_t i=0,n=0,v;
    while(n<6){
        v=0;
        if(!s[i])return 0;
        while(s[i]&&s[i]!=':'){
            char c=s[i++];
            if(c>='0'&&c<='9')v=(v<<4)|(uint32_t)(c-'0');
            else if(c>='a'&&c<='f')v=(v<<4)|(uint32_t)(c-'a'+10);
            else if(c>='A'&&c<='F')v=(v<<4)|(uint32_t)(c-'A'+10);
            else return 0;
            if(v>255u)return 0;
        }
        out[n++]=(uint8_t)v;
        if(s[i]==':')i++;
        else if(s[i]!=0)return 0;
    }
    return n==6&&s[i]==0;
}

static uint32_t ip_u32(const uint8_t*p){
    return ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|
           ((uint32_t)p[2]<<8)|p[3];
}

static void put_ip(const uint8_t*p){
    char b[12];
    dec(p[0],b);put(b);put(".");
    dec(p[1],b);put(b);put(".");
    dec(p[2],b);put(b);put(".");
    dec(p[3],b);put(b);
}

/* ------------------------- NET.CFG parser --------------------------- */

#define CFG_MAX 512u

struct net_cfg{
    uint16_t base;
    uint8_t mac[6];
    uint8_t ip[4];
    uint8_t mask[4];
    uint8_t gw[4];
    uint16_t port;
    char path[96];
    uint32_t timeout_s;
    uint32_t have;
};

#define C_BASE  0x01u
#define C_MAC   0x02u
#define C_IP    0x04u
#define C_MASK  0x08u
#define C_GW    0x10u
#define C_PORT  0x20u
#define C_PATH  0x40u
#define C_TIME  0x80u

static int cfg_line(struct net_cfg*c,char*line){
    char*e=line;
    char key[16];
    char val[128];
    uint32_t n=0,i;
    int ok;

    while(*e==' '||*e=='\t')e++;
    if(*e==0||*e=='#')return 1;
    while(*e&&*e!='='&&n+1<sizeof(key))key[n++]=*e++;
    key[n]=0;
    if(*e!='=')return 0;
    e++;
    while(*e==' '||*e=='\t')e++;
    n=0;
    while(*e&&*e!='\r'&&*e!='\n'&&n+1<sizeof(val))val[n++]=*e++;
    while(n&& (val[n-1]==' '||val[n-1]=='\t'))n--;
    val[n]=0;

    if(eq(key,"BASE")){
        uint32_t x=hex_number(val,&ok);
        if(!ok||x>0xffffu)return 0;
        c->base=(uint16_t)x;c->have|=C_BASE;return 1;
    }
    if(eq(key,"MAC")){
        if(!parse_mac(val,c->mac))return 0;
        c->have|=C_MAC;return 1;
    }
    if(eq(key,"IP")){
        if(!parse_ip(val,c->ip))return 0;
        c->have|=C_IP;return 1;
    }
    if(eq(key,"MASK")){
        if(!parse_ip(val,c->mask))return 0;
        c->have|=C_MASK;return 1;
    }
    if(eq(key,"GATEWAY")){
        if(!parse_ip(val,c->gw))return 0;
        c->have|=C_GW;return 1;
    }
    if(eq(key,"PORT")){
        uint32_t x=number(val,&ok);
        if(!ok||x==0||x>65535u)return 0;
        c->port=(uint16_t)x;c->have|=C_PORT;return 1;
    }
    if(eq(key,"PATH")){
        if(!val[0]||val[0]!='/')return 0;
        for(i=0;val[i]&&i+1<sizeof(c->path);i++)c->path[i]=val[i];
        if(val[i])return 0;
        c->path[i]=0;c->have|=C_PATH;return 1;
    }
    if(eq(key,"TIMEOUT")){
        uint32_t x=number(val,&ok);
        if(!ok||x==0||x>120u)return 0;
        c->timeout_s=x;c->have|=C_TIME;return 1;
    }
    return 0;
}

static int load_cfg_file(struct net_cfg*c,const char *filename){
    char text[CFG_MAX];
    uint32_t fd,n,total=0,i,start=0;
    uint32_t need=C_BASE|C_MAC|C_IP|C_MASK|C_GW|C_PORT|C_PATH|C_TIME;

    for(i=0;i<sizeof(text);i++)text[i]=0;
    fd=sc(SYS_FILE_OPEN,(uint32_t)filename,FAT16_MODE_READ,0);
    if(fd==SYS_FAIL){put("NETDRV: NET.CFG open failed\n");return 0;}
    while(total<sizeof(text)-1u){
        n=sc(SYS_FILE_READ,fd,(uint32_t)(text+total),
             sizeof(text)-1u-total);
        if(n==SYS_FAIL){sc(SYS_FILE_CLOSE,fd,0,0);return 0;}
        if(n==0)break;
        total+=n;
    }
    sc(SYS_FILE_CLOSE,fd,0,0);
    text[total]=0;

    c->have=0;
    for(i=0;i<=total;i++){
        if(text[i]=='\n'||text[i]==0){
            text[i]=0;
            if(!cfg_line(c,text+start)){put("NETDRV: bad NET.CFG line\n");return 0;}
            start=i+1u;
        }
    }
    if((c->have&need)!=need){
        put("NETDRV: NET.CFG has missing parameters\n");
        return 0;
    }
    return 1;
}

/* Обычные TCP/HTTP операции и старые тесты используют исходный NET.CFG.
 * Для UDP-телеметрии выбирается отдельный конфигурационный файл роли,
 * чтобы три виртуальные машины могли иметь разные IP/MAC в одном архиве. */

/* ----------------------------- NE2000 ------------------------------ */

#define NE_CR       0x00u
#define NE_PSTART   0x01u
#define NE_PSTOP    0x02u
#define NE_BNRY     0x03u
#define NE_TPSR     0x04u
#define NE_TBCR0    0x05u
#define NE_TBCR1    0x06u
#define NE_ISR      0x07u
#define NE_RSAR0    0x08u
#define NE_RSAR1    0x09u
#define NE_RBCR0    0x0au
#define NE_RBCR1    0x0bu
#define NE_RCR      0x0cu
#define NE_TCR      0x0du
#define NE_DCR      0x0eu
#define NE_IMR      0x0fu
#define NE_DATA     0x10u
#define NE_RESET    0x1fu

#define NE_PAGE1     0x40u
#define NE_STOP      0x01u
#define NE_START     0x02u
#define NE_TXP       0x04u
#define NE_RDMA_READ 0x08u
#define NE_RDMA_WRITE 0x10u
#define NE_NODMA     0x20u
#define NE_RXOK      0x01u
#define NE_TXOK      0x02u
#define NE_RXERR     0x04u
#define NE_TXERR     0x08u
#define NE_OVW       0x10u
#define NE_RDC       0x40u
#define NE_RST       0x80u

/* 0x40..0x45 — передатчик; 0x46..0x7f — кольцевой буфер приёма. */
#define NE_TX_PAGE    0x40u
#define NE_RX_START   0x46u
#define NE_RX_STOP    0x80u

static uint16_t ne_base;

static uint32_t ne_r(uint32_t r){return in8((uint32_t)ne_base+r);}
static int ne_w(uint32_t r,uint32_t v){return out8((uint32_t)ne_base+r,v)==0;}
/* Выбор страницы без остановки работающего приёмника.
 * Вызов NE_STOP во время polling создавал окно, в котором SYN-ACK/HTTP
 * ответ от QEMU мог прийти именно тогда, когда RX был остановлен.
 * Для работающего контроллера всегда сохраняем бит START. */
static void ne_select_run(uint8_t page){
    ne_w(NE_CR,(uint32_t)(NE_NODMA|NE_START|page));
}

/* Выбор страницы при остановленном контроллере — только для init. */
static void ne_select_stop(uint8_t page){
    ne_w(NE_CR,(uint32_t)(NE_NODMA|NE_STOP|page));
}

static int ne_wait_reset(void){
    uint32_t end=ticks()+100u;
    while(ticks()<end){
        uint32_t v=ne_r(NE_ISR);
        if(v!=SYS_FAIL && (v&NE_RST))return 1;
    }
    return 0;
}

static int ne_init(const struct net_cfg*c){
    uint32_t i;
    uint32_t x;

    ne_base=c->base;

    /* Читаем reset-порт. QEMU NE2000 после этого устанавливает ISR.RST. */
    x=ne_r(NE_RESET);
    (void)x;
    if(!ne_wait_reset()){put("NETDRV: NE2000 reset timeout\n");return 0;}

    /* Останавливаем устройство и задаём byte-mode remote DMA. */
    if(!ne_w(NE_CR,NE_NODMA|NE_STOP))return 0;
    if(!ne_w(NE_DCR,0x48u))return 0; /* LS=1, FIFO threshold=4, 8-bit DMA. */
    if(!ne_w(NE_RBCR0,0)||!ne_w(NE_RBCR1,0))return 0;
    /*
     * В минимальном драйвере включаем AB + PRO:
     *   AB  = 1 -> принимаем ARP/Ethernet broadcast;
     *   PRO = 1 -> принимаем также unicast, даже если конкретная версия
     *               эмуляции NE2000 не совпала с запрограммированным
     *               физическим адресом.
     *
     * Для QEMU это особенно полезно: ARP уже доказывает, что RX-кольцо
     * работает, а SYN-ACK приходит как unicast. Промискуитет здесь не
     * нарушает TCP/IP протокол и устраняет зависимость от внутренней
     * PROM-копии MAC в эмуляторе.
     */
    if(!ne_w(NE_RCR,0x14u))return 0; /* PRO=1 + AB=1 */
    if(!ne_w(NE_TCR,0x00u))return 0; /* normal transmit */
    if(!ne_w(NE_TPSR,NE_TX_PAGE))return 0;
    if(!ne_w(NE_PSTART,NE_RX_START))return 0;
    if(!ne_w(NE_PSTOP,NE_RX_STOP))return 0;
    /*
     * BNRY должен указывать на страницу ПЕРЕД первой страницей кольца.
     * При PSTART=0x46 это 0x45. Значение 0x7F было ошибкой: после
     * инициализации next вычислялся как 0x46, совпадал с CURR и драйвер
     * считал кольцо пустым даже после получения ARP/TCP кадров.
     */
    if(!ne_w(NE_BNRY,NE_RX_START-1u))return 0;

    /* Страница 1: записываем MAC и указатель текущей свободной страницы. */
    ne_select_stop(NE_PAGE1);
    for(i=0;i<6;i++)if(!ne_w(1u+i,c->mac[i]))return 0;
    if(!ne_w(0x07u,NE_RX_START))return 0; /* CURR */
    for(i=0;i<8;i++)if(!ne_w(0x08u+i,0))return 0; /* multicast hash */

    /* Возвращаемся на page 0, очищаем старые флаги и включаем приём. */
    ne_select_stop(0);
    if(!ne_w(NE_ISR,0xffu))return 0;
    if(!ne_w(NE_IMR,0x00u))return 0; /* IRQ не используем, только polling */
    if(!ne_w(NE_CR,NE_NODMA|NE_START))return 0;
    return 1;
}

/*
 * Ожидание завершения remote DMA.
 * QEMU устанавливает ISR.RDC после того, как RBCR становится нулём.
 * Явное ожидание исключает гонку между последней записью/чтением DATA
 * и следующей операцией с контроллером.
 */
static int ne_wait_rdc(uint32_t timeout_ticks){
    uint32_t end=ticks()+timeout_ticks;
    while(ticks()<end){
        uint32_t v=ne_r(NE_ISR);
        if(v==SYS_FAIL)return 0;
        if(v&NE_RDC){
            ne_w(NE_ISR,NE_RDC);
            return 1;
        }
    }
    return 0;
}

/* Подготовить remote DMA в режиме записи. */
static int ne_dma_write_begin(uint16_t addr,uint16_t n){
    /* Перед новой DMA-операцией контроллер должен работать в START. */
    if(!ne_w(NE_CR,NE_NODMA|NE_START))return 0;
    if(!ne_w(NE_ISR,NE_RDC))return 0;
    if(!ne_w(NE_RSAR0,addr&255u)||!ne_w(NE_RSAR1,addr>>8))return 0;
    if(!ne_w(NE_RBCR0,n&255u)||!ne_w(NE_RBCR1,n>>8))return 0;
    return ne_w(NE_CR,NE_NODMA|NE_RDMA_WRITE|NE_START);
}

/* Подготовить remote DMA в режиме чтения. */
static int ne_dma_read_begin(uint16_t addr,uint16_t n){
    if(!ne_w(NE_CR,NE_NODMA|NE_START))return 0;
    if(!ne_w(NE_ISR,NE_RDC))return 0;
    if(!ne_w(NE_RSAR0,addr&255u)||!ne_w(NE_RSAR1,addr>>8))return 0;
    if(!ne_w(NE_RBCR0,n&255u)||!ne_w(NE_RBCR1,n>>8))return 0;
    return ne_w(NE_CR,NE_NODMA|NE_RDMA_READ|NE_START);
}

/* Записываем N байт в память NE2000 через remote DMA. */
static int ne_mem_write(uint16_t addr,const uint8_t*b,uint16_t n,
                        uint32_t timeout_ticks){
    uint16_t i;
    if(!n)return 1;
    if(!ne_dma_write_begin(addr,n))return 0;
    for(i=0;i<n;i++)if(!ne_w(NE_DATA,b[i]))return 0;
    return ne_wait_rdc(timeout_ticks);
}

/* Читаем N байт из памяти NE2000 через remote DMA. */
static int ne_mem_read(uint16_t addr,uint8_t*b,uint16_t n,
                       uint32_t timeout_ticks){
    uint16_t i;
    if(!n)return 1;
    if(!ne_dma_read_begin(addr,n))return 0;
    for(i=0;i<n;i++){
        uint32_t v=ne_r(NE_DATA);
        if(v==SYS_FAIL)return 0;
        b[i]=(uint8_t)v;
    }
    return ne_wait_rdc(timeout_ticks);
}

/*
 * Чтение области кольцевого буфера с учётом перехода PSTOP -> PSTART.
 * Обычный Ethernet-кадр обычно помещается целиком, но TCP/HTTP должен
 * оставаться корректным и после нескольких кадров, когда пакет попадёт
 * в конец кольца.
 */
static int ne_ring_read(uint16_t addr,uint8_t*b,uint16_t n,
                        uint32_t timeout_ticks){
    uint16_t left=n,pos=0,chunk;
    while(left){
        if(addr<((uint16_t)NE_RX_START<<8)||
           addr>=((uint16_t)NE_RX_STOP<<8))return 0;
        chunk=(uint16_t)(((uint16_t)NE_RX_STOP<<8)-addr);
        if(chunk>left)chunk=left;
        if(!ne_mem_read(addr,b+pos,chunk,timeout_ticks))return 0;
        pos=(uint16_t)(pos+chunk);
        left=(uint16_t)(left-chunk);
        addr=(uint16_t)(addr+chunk);
        if(!left)break;
        addr=(uint16_t)NE_RX_START<<8;
    }
    return 1;
}

#define FRAME_MAX 1600u
static uint8_t rx_frame[FRAME_MAX];
static uint8_t tx_frame[FRAME_MAX];

/* Получить следующий Ethernet кадр из кольца. */
static int ne_recv(uint8_t*b,uint16_t*len,uint32_t timeout_ticks){
    uint32_t end=ticks()+timeout_ticks;

    while(ticks()<end){
        uint8_t cur,hdr[4];
        uint8_t bnry,next;
        uint16_t total,data_len;
        uint16_t addr;

        /* Не останавливаем RX при чтении CURR: ответ TCP может прийти
         * в любой момент после отправки SYN. */
        ne_select_run(NE_PAGE1);
        cur=(uint8_t)ne_r(0x07u); /* page 1 CURR */
        ne_select_run(0);
        bnry=(uint8_t)ne_r(NE_BNRY);
        next=(uint8_t)(bnry+1u);
        if(next>=NE_RX_STOP)next=NE_RX_START;

        if(cur==next)continue;

        addr=(uint16_t)next<<8;
        if(!ne_ring_read(addr,hdr,4,timeout_ticks?timeout_ticks:1u)){
            put("NETDRV: NE2000 RX header DMA error\n");return -1;
        }
        if(!(hdr[0]&NE_RXOK)||hdr[1]<NE_RX_START||hdr[1]>=NE_RX_STOP){
            /* Повреждённый ring header: возвращаем кольцо в безопасную точку. */
            ne_w(NE_BNRY,NE_RX_STOP-1u);
            continue;
        }
        total=(uint16_t)hdr[2]|((uint16_t)hdr[3]<<8);
        if(total<4u||total>FRAME_MAX+4u){
            ne_w(NE_BNRY,(uint32_t)(hdr[1]-1u));
            continue;
        }
        data_len=(uint16_t)(total-4u);
        if(!ne_ring_read((uint16_t)(addr+4u),b,data_len,
                         timeout_ticks?timeout_ticks:1u)){
            put("NETDRV: NE2000 RX data DMA error\n");return -1;
        }
        *len=data_len;
        next=hdr[1];
        if(next==NE_RX_START)ne_w(NE_BNRY,NE_RX_STOP-1u);
        else ne_w(NE_BNRY,(uint32_t)(next-1u));
        ne_w(NE_ISR,NE_RXOK|NE_RXERR|NE_OVW);
        return 1;
    }
    return 0;
}

static int ne_send(uint16_t len,uint32_t timeout_ticks){
    uint32_t end;
    uint32_t v;
    /* Ethernet без CRC должен содержать минимум 60 байт.
     * Сохраняем исходную длину до изменения len, иначе padding никогда
     * не выполнялся (старый код сначала делал len=60). */
    {
        uint16_t original=len;
        if(len<60u)len=60u;
        if(len>FRAME_MAX)return 0;
        if(original<60u){
            uint16_t i;
            for(i=original;i<60u;i++)tx_frame[i]=0;
        }
    }
    if(!ne_mem_write((uint16_t)NE_TX_PAGE<<8,tx_frame,len,
                     timeout_ticks?timeout_ticks:1u))return 0;
    ne_select_run(0);
    if(!ne_w(NE_ISR,NE_TXOK|NE_TXERR))return 0;
    if(!ne_w(NE_TBCR0,len&255u)||!ne_w(NE_TBCR1,len>>8))return 0;
    if(!ne_w(NE_TPSR,NE_TX_PAGE))return 0;
    if(!ne_w(NE_CR,NE_NODMA|NE_TXP|NE_START))return 0;

    end=ticks()+timeout_ticks;
    while(ticks()<end){
        v=ne_r(NE_ISR);
        if(v==SYS_FAIL)return 0;
        if(v&NE_TXOK){ne_w(NE_ISR,NE_TXOK);return 1;}
        if(v&NE_TXERR){ne_w(NE_ISR,NE_TXERR);return 0;}
    }
    return 0;
}

/* ------------------------- Ethernet / ARP -------------------------- */

static const uint8_t BROADCAST[6]={255,255,255,255,255,255};
static uint8_t local_mac[6];
static uint8_t target_mac[6];

static void eth_header(uint8_t*f,const uint8_t*dst,uint16_t type){
    uint32_t i;
    for(i=0;i<6;i++){f[i]=dst[i];f[6+i]=local_mac[i];}
    f[12]=(uint8_t)(type>>8);f[13]=(uint8_t)type;
}

static uint16_t be16(const uint8_t*p){
    return (uint16_t)(((uint16_t)p[0]<<8)|p[1]);
}
static uint32_t be32(const uint8_t*p){
    return ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|
           ((uint32_t)p[2]<<8)|p[3];
}
static void put_be16(uint8_t*p,uint16_t v){p[0]=v>>8;p[1]=(uint8_t)v;}
static void put_be32(uint8_t*p,uint32_t v){
    p[0]=v>>24;p[1]=v>>16;p[2]=v>>8;p[3]=(uint8_t)v;
}

static uint16_t csum(const uint8_t*p,uint32_t n){
    uint32_t sum=0;
    while(n>1){sum+=((uint16_t)p[0]<<8)|p[1];p+=2;n-=2;}
    if(n)sum+=(uint16_t)p[0]<<8;
    while(sum>>16)sum=(sum&0xffffu)+(sum>>16);
    return (uint16_t)~sum;
}

static int arp_request(const struct net_cfg*c,const uint8_t query[4]){
    uint8_t*f=tx_frame;
    uint32_t i;
    eth_header(f,BROADCAST,0x0806u);
    put_be16(f+14,1);        /* Ethernet */
    put_be16(f+16,0x0800);   /* IPv4 */
    f[18]=6;f[19]=4;put_be16(f+20,1); /* request */
    for(i=0;i<6;i++)f[22+i]=local_mac[i];
    for(i=0;i<4;i++)f[28+i]=c->ip[i];
    for(i=0;i<6;i++)f[32+i]=0;
    for(i=0;i<4;i++)f[38+i]=query[i];
    return ne_send(42,(uint32_t)c->timeout_s*50u);
}

static int arp_resolve(const struct net_cfg*c,const uint8_t query[4]){
    uint32_t end=ticks()+(uint32_t)c->timeout_s*50u;
    uint16_t n;
    while(ticks()<end){
        uint32_t i;
        if(!arp_request(c,query))return 0;
        {
            uint32_t wait=ticks()+25u;
            while(ticks()<wait){
                int r=ne_recv(rx_frame,&n,1u);
                if(r<0)return 0;
                if(r==0)continue;
                if(n<42u||be16(rx_frame+12)!=0x0806u)continue;
                if(be16(rx_frame+20)!=2u)continue;
                if(be32(rx_frame+28)!=ip_u32(query))continue;
                for(i=0;i<6;i++)target_mac[i]=rx_frame[22+i];
                return 1;
            }
        }
    }
    return 0;
}

/* ------------------------------ IPv4 ------------------------------- */

static uint16_t ip_ident=1u;

static uint16_t ip_checksum(uint8_t*h){
    h[10]=0;h[11]=0;
    return csum(h,20);
}

static int ip_send_tcp(const struct net_cfg*c,const uint8_t dst_ip[4],
                       uint16_t sport,uint16_t dport,uint32_t seq,uint32_t ack,
                       uint8_t flags,const uint8_t*data,uint16_t dlen){
    uint8_t*ip=tx_frame+14;
    uint8_t*tcp=ip+20;
    uint32_t sum,i;
    uint16_t total=(uint16_t)(20u+20u+dlen);

    if(total>1460u)return 0;

    eth_header(tx_frame,target_mac,0x0800u);
    ip[0]=0x45;ip[1]=0;
    put_be16(ip+2,total);
    put_be16(ip+4,ip_ident++);
    put_be16(ip+6,0x4000u);
    ip[8]=64;ip[9]=6;ip[10]=ip[11]=0;
    for(i=0;i<4;i++){ip[12+i]=c->ip[i];ip[16+i]=dst_ip[i];}
    put_be16(ip+10,ip_checksum(ip));

    put_be16(tcp+0,sport);put_be16(tcp+2,dport);
    put_be32(tcp+4,seq);put_be32(tcp+8,ack);
    tcp[12]=0x50;tcp[13]=flags;
    put_be16(tcp+14,4096u);
    put_be16(tcp+16,0);put_be16(tcp+18,0);
    for(i=0;i<dlen;i++)tcp[20+i]=data[i];

    /* TCP pseudo-header checksum. Складываем 16-битные слова. */
    sum=0;
    sum+=((uint16_t)c->ip[0]<<8)|c->ip[1];
    sum+=((uint16_t)c->ip[2]<<8)|c->ip[3];
    sum+=((uint16_t)dst_ip[0]<<8)|dst_ip[1];
    sum+=((uint16_t)dst_ip[2]<<8)|dst_ip[3];
    sum+=6u;
    sum+=(uint16_t)(20u+dlen);
    for(i=0;i<20u+dlen;i+=2u){
        uint16_t w=(uint16_t)tcp[i]<<8;
        if(i+1u<20u+dlen)w|=tcp[i+1u];
        sum+=w;
    }
    while(sum>>16)sum=(sum&0xffffu)+(sum>>16);
    put_be16(tcp+16,(uint16_t)~sum);

    return ne_send((uint16_t)(14u+20u+20u+dlen),
                   (uint32_t)c->timeout_s*50u);
}

static int ip_match(const uint8_t*a,const uint8_t*b){
    return a[0]==b[0]&&a[1]==b[1]&&a[2]==b[2]&&a[3]==b[3];
}

static int parse_ipv4_tcp(const uint8_t*f,uint16_t n,const uint8_t**ip,
                          const uint8_t**tcp,uint16_t*tl){
    uint8_t ihl;
    if(n<54u||be16(f+12)!=0x0800u)return 0;
    *ip=f+14;
    if((*ip)[0]>>4!=4)return 0;
    ihl=(uint8_t)((*ip)[0]&15u);
    if(ihl<5u)return 0;
    if(n<14u+(uint16_t)ihl*4u+20u)return 0;
    if((*ip)[9]!=6u)return 0;
    {
        uint16_t ip_total=be16(*ip+2);
        uint16_t ihl_bytes=(uint16_t)ihl*4u;
        if(ip_total<ihl_bytes+20u)return 0;
        if(ip_total>(uint16_t)(n-14u))return 0;
        if(csum(*ip,ihl_bytes)!=0u)return 0;
        *tcp=*ip+ihl_bytes;
        *tl=(uint16_t)(ip_total-ihl_bytes);
    }
    return 1;
}

/* ------------------------------ UDP -------------------------------- */

#define TELEMETRY_PORT 5000u
#define TELEMETRY_PERIOD_TICKS 50u /* PIT в Toy OS работает на 50 Гц. */
#define TELEMETRY_MAX 120u

static int ip_send_udp(const struct net_cfg*c,const uint8_t dst_ip[4],
                       uint16_t sport,uint16_t dport,const uint8_t*data,
                       uint16_t dlen){
    uint8_t*ip=tx_frame+14;
    uint8_t*udp=ip+20;
    uint16_t total=(uint16_t)(20u+8u+dlen);
    uint16_t ulen=(uint16_t)(8u+dlen);
    uint32_t i;
    if(total>1500u||dlen>1200u)return 0;
    eth_header(tx_frame,target_mac,0x0800u);
    ip[0]=0x45;ip[1]=0;
    put_be16(ip+2,total);put_be16(ip+4,ip_ident++);put_be16(ip+6,0x4000u);
    ip[8]=64;ip[9]=17;ip[10]=ip[11]=0;
    for(i=0;i<4;i++){ip[12+i]=c->ip[i];ip[16+i]=dst_ip[i];}
    put_be16(ip+10,ip_checksum(ip));
    put_be16(udp+0,sport);put_be16(udp+2,dport);put_be16(udp+4,ulen);
    put_be16(udp+6,0); /* IPv4 UDP checksum 0 = deliberately omitted. */
    for(i=0;i<dlen;i++)udp[8+i]=data[i];
    /* UDP checksum=0 для IPv4 означает отсутствие checksum; этот вариант
       намеренно оставлен минимальным для первого этапа телеметрии. */
    /* В tx_frame первые 14 байт занимает Ethernet-заголовок.
     * Поэтому физическая длина кадра должна включать Ethernet + IPv4.
     * Ранее передавалось только значение total (20+8+payload), из-за чего
     * последние 14 байт UDP/IP кадра не отправлялись. Slave получал TX OK,
     * но Master видел усечённый кадр и закономерно отбрасывал его. */
    return ne_send((uint16_t)(14u+total),(uint32_t)c->timeout_s*50u);
}

static int parse_ipv4_udp(const uint8_t*f,uint16_t n,const uint8_t**ip,
                          const uint8_t**udp,uint16_t*ul){
    uint8_t ihl;
    uint16_t ip_total,ihl_bytes,udp_len;
    if(n<42u||be16(f+12)!=0x0800u)return 0;
    *ip=f+14;
    if((*ip)[0]>>4!=4)return 0;
    ihl=(uint8_t)((*ip)[0]&15u);
    if(ihl<5u)return 0;
    ihl_bytes=(uint16_t)ihl*4u;
    if(n<14u+ihl_bytes+8u)return 0;
    if((*ip)[9]!=17u)return 0;
    ip_total=be16(*ip+2);
    if(ip_total<ihl_bytes+8u||ip_total>(uint16_t)(n-14u))return 0;
    if(csum(*ip,ihl_bytes)!=0u)return 0;
    *udp=*ip+ihl_bytes;
    udp_len=be16(*udp+4);
    if(udp_len<8u||udp_len>ip_total-ihl_bytes)return 0;
    *ul=(uint16_t)(udp_len-8u);
    return 1;
}

static void text_fill(uint8_t*dst,uint32_t n){uint32_t i;for(i=0;i<n;i++)dst[i]=' ';}

static void console_at(uint32_t row,uint32_t col,const char*s,uint32_t n){
    (void)sc(SYS_CONSOLE_AT,(row<<16)|col,(uint32_t)s,n);
}

/*
 * Записывает одну полную строку терминала без прокрутки.
 * Все поля телеметрии используют этот helper, поэтому новая секунда
 * заменяет старое значение на том же месте экрана.
 */
static void telemetry_row(uint32_t row,const char*s){
    uint8_t line[78];
    uint32_t i=0;
    text_fill(line,sizeof(line));
    while(s[i]&&i<sizeof(line)){line[i]=(uint8_t)s[i];i++;}
    console_at(row,0,(const char*)line,sizeof(line));
}

static void telemetry_render_line(uint32_t row,const char*label,const uint8_t*data,
                                  uint16_t n){
    char line[78];
    uint32_t i,pos=0,ll=0;
    text_fill((uint8_t*)line,sizeof(line));
    while(label[ll]&&pos<sizeof(line)){line[pos++]=label[ll++];}
    if(pos<sizeof(line))line[pos++]=' ';
    for(i=0;i<n&&pos<sizeof(line);i++){
        uint8_t ch=data[i];
        if(ch=='\r'||ch=='\n')ch=' ';
        if(ch<32u||ch>126u)ch='.';
        line[pos++]=(char)ch;
    }
    console_at(row,0,line,sizeof(line));
}

static void telemetry_master_dashboard_init(const struct net_cfg*c,uint16_t port){
    char line[78];uint32_t p,i;
    const char title[]="================ TOY OS | UDP TELEMETRY MASTER ================";
    telemetry_row(1,title);
    text_fill((uint8_t*)line,sizeof(line));p=0;
    {const char a[]="MASTER IP: ";for(i=0;a[i]&&p<sizeof(line);i++)line[p++]=a[i];}
    {char ipbuf[20];uint32_t j=0;uint8_t x;
     for(i=0;i<4;i++){x=c->ip[i];if(x>=100u){ipbuf[j++]=(char)('0'+x/100u);x%=100u;}if(x>=10u)ipbuf[j++]=(char)('0'+x/10u);ipbuf[j++]=(char)('0'+x%10u);if(i<3)ipbuf[j++]='.';}ipbuf[j]=0;
     for(i=0;ipbuf[i]&&p<sizeof(line);i++)line[p++]=ipbuf[i];}
    {const char a[]="   UDP PORT: ";for(i=0;a[i]&&p<sizeof(line);i++)line[p++]=a[i];}
    {char b[12];dec(port,b);for(i=0;b[i]&&p<sizeof(line);i++)line[p++]=b[i];}
    telemetry_row(2,line);
    telemetry_row(4,"-------------------- SLAVE TELEMETRY -------------------------");
    telemetry_row(6,"[ SLAVE1 ]  IP=10.66.2.2  STATUS=WAITING   RX=0");
    telemetry_row(7,"DATA: waiting for first UDP packet...");
    telemetry_row(9,"[ SLAVE2 ]  IP=10.66.2.3  STATUS=WAITING   RX=0");
    telemetry_row(10,"DATA: waiting for first UDP packet...");
    telemetry_row(12,"NETWORK: UDP/IPv4 | packets are overwritten in place");
    telemetry_row(14,"CONTROL: ESC = stop telemetry master");
}

static void telemetry_slave_dashboard_init(const struct net_cfg*c,const char*role,
                                            const uint8_t dst[4]){
    char line[78];uint32_t p=0,i;
    telemetry_row(1,"================ TOY OS | UDP TELEMETRY SLAVE ================");
    text_fill((uint8_t*)line,sizeof(line));
    {const char a[]="ROLE: ";for(i=0;a[i]&&p<sizeof(line);i++)line[p++]=a[i];}
    for(i=0;role[i]&&p<sizeof(line);i++)line[p++]=role[i];
    {const char a[]="   LOCAL IP: ";for(i=0;a[i]&&p<sizeof(line);i++)line[p++]=a[i];}
    {char ipbuf[20];uint32_t j=0;uint8_t x;for(i=0;i<4;i++){x=c->ip[i];if(x>=100u){ipbuf[j++]=(char)('0'+x/100u);x%=100u;}if(x>=10u)ipbuf[j++]=(char)('0'+x/10u);ipbuf[j++]=(char)('0'+x%10u);if(i<3)ipbuf[j++]='.';}ipbuf[j]=0;for(i=0;ipbuf[i]&&p<sizeof(line);i++)line[p++]=ipbuf[i];}
    telemetry_row(2,line);
    text_fill((uint8_t*)line,sizeof(line));p=0;
    {const char a[]="MASTER IP: ";for(i=0;a[i]&&p<sizeof(line);i++)line[p++]=a[i];}
    {char ipbuf[20];uint32_t j=0;uint8_t x;for(i=0;i<4;i++){x=dst[i];if(x>=100u){ipbuf[j++]=(char)('0'+x/100u);x%=100u;}if(x>=10u)ipbuf[j++]=(char)('0'+x/10u);ipbuf[j++]=(char)('0'+x%10u);if(i<3)ipbuf[j++]='.';}ipbuf[j]=0;for(i=0;ipbuf[i]&&p<sizeof(line);i++)line[p++]=ipbuf[i];}
    telemetry_row(3,line);
    telemetry_row(5,"NETWORK STATUS");
    telemetry_row(6,"Gateway: 10.66.2.1");
    telemetry_row(7,"ARP: WAITING");
    telemetry_row(8,"TRANSMIT: 0 packets");
    telemetry_row(10,"LAST TELEMETRY: waiting for first transmission...");
    telemetry_row(12,"LINK: routed Wi-Fi / TAP / NE2000");
    telemetry_row(14,"CONTROL: ESC = stop telemetry slave");
}

static void telemetry_slave_refresh(uint32_t tx,const char*state,const char*payload){
    char line[78],cnt[12];uint32_t p=0,i;
    text_fill((uint8_t*)line,sizeof(line));dec(tx,cnt);
    {const char a[]="ARP: ";for(i=0;a[i]&&p<sizeof(line);i++)line[p++]=a[i];}
    for(i=0;state[i]&&p<sizeof(line);i++)line[p++]=state[i];
    telemetry_row(7,line);
    text_fill((uint8_t*)line,sizeof(line));p=0;
    {const char a[]="TRANSMIT: ";for(i=0;a[i]&&p<sizeof(line);i++)line[p++]=a[i];}
    for(i=0;cnt[i]&&p<sizeof(line);i++)line[p++]=cnt[i];
    {const char a[]=" packets";for(i=0;a[i]&&p<sizeof(line);i++)line[p++]=a[i];}
    telemetry_row(8,line);
    text_fill((uint8_t*)line,sizeof(line));p=0;
    {const char a[]="LAST TELEMETRY: ";for(i=0;a[i]&&p<sizeof(line);i++)line[p++]=a[i];}
    for(i=0;payload[i]&&p<sizeof(line);i++)line[p++]=payload[i];
    telemetry_row(10,line);
}

static int telemetry_slave(const struct net_cfg*c,const char*role_text,
                           const uint8_t dst_ip[4]){
    uint32_t next=0,count=0,now,target_valid=0;
    uint16_t sport=(uint16_t)(4000u+(role_text[5]=='2'?2u:1u));
    uint8_t payload[TELEMETRY_MAX],route_ip[4];
    uint32_t status_painted=0;
    if(((ip_u32(dst_ip)^ip_u32(c->ip))&ip_u32(c->mask))==0u){
        uint32_t i; for(i=0;i<4;i++)route_ip[i]=dst_ip[i];
    }else{
        uint32_t i; for(i=0;i<4;i++)route_ip[i]=c->gw[i];
    }
    telemetry_slave_dashboard_init(c,role_text,dst_ip);
    for(;;){
        uint8_t key=0;
        now=ticks();
        (void)sc(SYS_CONSOLE_POLL,(uint32_t)&key,0,0);
        if(key==0x1bu){
            telemetry_row(16,"Telemetry slave stopped. Returning to shell...");
            sc(SYS_EXIT,EXE_OK,0,0);
            for(;;){}
        }
        if(!target_valid){
            struct net_cfg probe=*c;
            probe.timeout_s=1u;
            if(!status_painted){telemetry_slave_refresh(count,"WAITING","");status_painted=1u;}
            if(arp_resolve(&probe,route_ip)){
                target_valid=1u;
                status_painted=0u;
                next=now;
            }else{
                next=now+TELEMETRY_PERIOD_TICKS;
            }
        }
        if(target_valid && now>=next){
            uint32_t p=0,i;char nb[12],tb[12],payload_text[78];
            dec(count,nb);dec(now,tb);
            for(i=0;role_text[i]&&p+1u<sizeof(payload);i++)payload[p++]=(uint8_t)role_text[i];
            if(p+1u<sizeof(payload))payload[p++]=' ';
            {const char a[]="COUNT=";for(i=0;a[i]&&p+1u<sizeof(payload);i++)payload[p++]=(uint8_t)a[i];}
            for(i=0;nb[i]&&p+1u<sizeof(payload);i++)payload[p++]=(uint8_t)nb[i];
            if(p+1u<sizeof(payload))payload[p++]=' ';
            {const char a[]="TICK=";for(i=0;a[i]&&p+1u<sizeof(payload);i++)payload[p++]=(uint8_t)a[i];}
            for(i=0;tb[i]&&p+1u<sizeof(payload);i++)payload[p++]=(uint8_t)tb[i];
            if(ip_send_udp(c,dst_ip,sport,TELEMETRY_PORT,payload,(uint16_t)p)){
                count++;
                for(i=0;i<p&&i+1u<sizeof(payload_text);i++) payload_text[i]=(char)payload[i];
                payload_text[i]=0;
                telemetry_slave_refresh(count,"READY",payload_text);
                next=now+TELEMETRY_PERIOD_TICKS;
            }else{
                target_valid=0u;status_painted=0u;next=now+TELEMETRY_PERIOD_TICKS;
            }
        }
    }
    return 0;
}

/* Восстановление RX-кольца NE2000 после ошибочного/оборванного кадра. */
static void telemetry_rx_recover(void){
    ne_select_run(0);
    (void)ne_w(NE_CR,NE_NODMA|NE_STOP);
    (void)ne_w(NE_ISR,NE_RXOK|NE_RXERR|NE_OVW);
    (void)ne_w(NE_BNRY,NE_RX_START-1u);
    ne_select_stop(NE_PAGE1);
    (void)ne_w(0x07u,NE_RX_START);
    ne_select_stop(0);
    (void)ne_w(NE_CR,NE_NODMA|NE_START);
}

static int telemetry_arp_reply(const struct net_cfg*c,const uint8_t*f,uint16_t n){
    uint32_t i;
    if(n<42u||be16(f+12)!=0x0806u)return 0;
    if(be16(f+14)!=1u||be16(f+16)!=0x0800u||f[18]!=6u||f[19]!=4u)return 0;
    if(be16(f+20)!=1u)return 0;
    if(be32(f+38)!=ip_u32(c->ip))return 0;
    eth_header(tx_frame,f+22,0x0806u);
    put_be16(tx_frame+14,1u);put_be16(tx_frame+16,0x0800u);tx_frame[18]=6u;tx_frame[19]=4u;put_be16(tx_frame+20,2u);
    for(i=0;i<6;i++)tx_frame[22+i]=local_mac[i];
    for(i=0;i<4;i++)tx_frame[28+i]=c->ip[i];
    for(i=0;i<6;i++)tx_frame[32+i]=f[22+i];
    for(i=0;i<4;i++)tx_frame[38+i]=f[28+i];
    return ne_send(42u,(uint32_t)c->timeout_s*50u);
}

static int telemetry_source_role(const uint8_t ip[4],const uint8_t*data,uint16_t n){
    uint32_t i;
    if(ip[0]==10u&&ip[1]==66u&&ip[2]==2u&&ip[3]==2u)return 1;
    if(ip[0]==10u&&ip[1]==66u&&ip[2]==2u&&ip[3]==3u)return 2;
    if(n>=6u){
        const char s1[]="SLAVE1";const char s2[]="SLAVE2";
        for(i=0;i<6u;i++){if(data[i]!=(uint8_t)s1[i])break;}
        if(i==6u)return 1;
        for(i=0;i<6u;i++){if(data[i]!=(uint8_t)s2[i])break;}
        if(i==6u)return 2;
    }
    return 0;
}

static void telemetry_master_dashboard_refresh(uint32_t count1,uint32_t count2,uint32_t last1,uint32_t last2,
                                               const uint8_t*payload1,uint16_t len1,
                                               const uint8_t*payload2,uint16_t len2,uint32_t now){
    char c1[12],c2[12],line[78];const char *s1,*s2;uint32_t p,i;
    dec(count1,c1);dec(count2,c2);
    s1=(last1&&now-last1<=150u)?"ONLINE":"WAITING";
    s2=(last2&&now-last2<=150u)?"ONLINE":"WAITING";
    text_fill((uint8_t*)line,sizeof(line));p=0;{const char a[]="[ SLAVE1 ]  IP=10.66.2.2  STATUS=";for(i=0;a[i]&&p<sizeof(line);i++)line[p++]=a[i];}
    for(i=0;s1[i]&&p<sizeof(line);i++) line[p++]=s1[i];
    {const char a[]="  RX=";for(i=0;a[i]&&p<sizeof(line);i++) line[p++]=a[i];}
    for(i=0;c1[i]&&p<sizeof(line);i++) line[p++]=c1[i];
    telemetry_row(6,line);
    telemetry_render_line(7,"DATA:",payload1,len1);
    text_fill((uint8_t*)line,sizeof(line));p=0;{const char a[]="[ SLAVE2 ]  IP=10.66.2.3  STATUS=";for(i=0;a[i]&&p<sizeof(line);i++)line[p++]=a[i];}
    for(i=0;s2[i]&&p<sizeof(line);i++) line[p++]=s2[i];
    {const char a[]="  RX=";for(i=0;a[i]&&p<sizeof(line);i++) line[p++]=a[i];}
    for(i=0;c2[i]&&p<sizeof(line);i++) line[p++]=c2[i];
    telemetry_row(9,line);
    telemetry_render_line(10,"DATA:",payload2,len2);
    text_fill((uint8_t*)line,sizeof(line));p=0;{const char a[]="NETWORK: UDP/IPv4 | MASTER LISTENING | timeout=3s";for(i=0;a[i]&&p<sizeof(line);i++)line[p++]=a[i];}telemetry_row(12,line);
}

static int telemetry_master(const struct net_cfg*c,uint16_t port){
    uint32_t last1=0,last2=0,count1=0,count2=0;
    uint8_t payload1[TELEMETRY_MAX],payload2[TELEMETRY_MAX];
    uint16_t len1=0,len2=0,n;
    uint32_t dashboard_last=0;
    text_fill(payload1,sizeof(payload1));text_fill(payload2,sizeof(payload2));
    telemetry_master_dashboard_init(c,port);
    for(;;){
        int r=ne_recv(rx_frame,&n,1u);uint32_t now=ticks();uint8_t key=0;
        (void)sc(SYS_CONSOLE_POLL,(uint32_t)&key,0,0);
        if(key==0x1bu){telemetry_row(16,"Telemetry master stopped. Returning to shell...");sc(SYS_EXIT,EXE_OK,0,0);for(;;){} }
        if(r<0){telemetry_rx_recover();continue;}
        if(r>0){
            (void)telemetry_arp_reply(c,rx_frame,n);
            {const uint8_t*ip,*udp;uint16_t ul;
             if(parse_ipv4_udp(rx_frame,n,&ip,&udp,&ul)&&be16(udp+2)==port&&ul>0u){
                 const uint8_t*data=udp+8;int role=telemetry_source_role(ip,data,ul);uint16_t m=ul<TELEMETRY_MAX?ul:TELEMETRY_MAX;uint16_t i;
                 if(role==1){for(i=0;i<m;i++)payload1[i]=data[i];len1=m;last1=now;count1++;}
                 else if(role==2){for(i=0;i<m;i++)payload2[i]=data[i];len2=m;last2=now;count2++;}
             }
            }
        }
        if(!dashboard_last||now-dashboard_last>=5u){
            telemetry_master_dashboard_refresh(count1,count2,last1,last2,payload1,len1,payload2,len2,now);
            dashboard_last=now;
        }
    }
    return 0;
}

/* ------------------------------ TCP -------------------------------- */


static uint8_t pending_frame[FRAME_MAX];
static uint16_t pending_len;
static uint32_t pending_valid;
/* Кадр с данными может одновременно содержать ACK на наш сегмент.
 * Такой кадр откладываем, но НЕ увеличиваем peer_seq и НЕ посылаем ACK:
 * recv_file_http обработает его в штатном порядке и подтвердит данные. */

struct tcp_state{
    uint16_t sport,dport;
    uint32_t seq,ack;
    uint32_t peer_seq;
    uint8_t peer_ip[4];
    uint8_t state;
};

#define TCP_CLOSED  0u
#define TCP_SYN     1u
#define TCP_EST     2u
#define TCP_FIN     3u

static int tcp_wait_syn(const struct net_cfg*c,struct tcp_state*t){
    uint32_t end=ticks()+(uint32_t)c->timeout_s*50u;
    while(ticks()<end){
        uint16_t n;
        int r=ne_recv(rx_frame,&n,1u);
        const uint8_t*ip,*tcp;
        uint16_t tl;
        if(r<0)return 0;
        if(r==0)continue;
        if(!parse_ipv4_tcp(rx_frame,n,&ip,&tcp,&tl))continue;
        if(!ip_match(ip+12,t->peer_ip))continue;
        /* Входящий SYN-ACK идёт ОТ сервера К нашему ephemeral-порту:
         * source = server port (t->dport), destination = our port (t->sport).
         * Старый код сравнивал эти два поля наоборот и поэтому отбрасывал
         * каждый корректный SYN-ACK как "чужой" пакет. */
        if(be16(tcp+0)!=t->dport||be16(tcp+2)!=t->sport)continue;
        if(tcp[13]&0x04u)return 0; /* RST */
        if((tcp[13]&0x12u)!=0x12u)continue;
        if(be32(tcp+8)!=t->seq+1u)continue;
        t->peer_seq=be32(tcp+4)+1u;
        t->ack=t->peer_seq;
        return 1;
    }
    return 0;
}

static int tcp_connect(const struct net_cfg*c,const uint8_t dst[4],
                       struct tcp_state*t){
    uint32_t try;
    t->sport=40000u+(uint16_t)(ticks()&1023u);
    t->dport=c->port;
    t->seq=0x13572468u+(ticks()<<4);
    t->ack=0;
    t->peer_seq=0;
    t->state=TCP_SYN;
    pending_valid=0;
    pending_len=0;
    for(try=0;try<3;try++){
        if(!ip_send_tcp(c,dst,t->sport,t->dport,t->seq,0,0x02u,0,0))return 0;
        if(tcp_wait_syn(c,t)){
            if(!ip_send_tcp(c,dst,t->sport,t->dport,t->seq+1u,
                            t->ack,0x10u,0,0))return 0;
            t->seq++;
            t->state=TCP_EST;
            return 1;
        }
    }
    put("NETDRV: no TCP SYN-ACK\n");
    return 0;
}

/* Ожидание ACK для одного отправленного сегмента. */
static int tcp_wait_ack(const struct net_cfg*c,struct tcp_state*t,uint32_t wanted){
    uint32_t end=ticks()+(uint32_t)c->timeout_s*50u;
    while(ticks()<end){
        uint16_t n;
        int r=ne_recv(rx_frame,&n,1u);
        const uint8_t*ip,*tcp;
        uint16_t tl,off,dlen;
        if(r<0)return 0;
        if(r==0)continue;
        if(!parse_ipv4_tcp(rx_frame,n,&ip,&tcp,&tl))continue;
        if(!ip_match(ip+12,t->peer_ip))continue;
        /* Для любого входящего TCP-сегмента направление портов такое же:
         * сервер -> клиент. */
        if(be16(tcp)!=t->dport||be16(tcp+2)!=t->sport)continue;
        off=(uint16_t)((tcp[12]>>4)*4u);
        if(off<20u||off>tl)continue;
        dlen=(uint16_t)(tl-off);
        if(dlen&&be32(tcp+4)==t->peer_seq&&dlen<=FRAME_MAX){
            uint16_t j;
            for(j=0;j<n;j++)pending_frame[j]=rx_frame[j];
            pending_len=n;
            pending_valid=1u;
            /* ACK нашего сегмента уже проверяется ниже.
             * Данные сервера подтверждаем позже, после разбора HTTP. */
        }
        if((tcp[13]&0x10u)&&be32(tcp+8)>=wanted)return 1;
    }
    return 0;
}

static int http_send_segment(const struct net_cfg*c,struct tcp_state*t,
                             const uint8_t*d,uint16_t n){
    uint32_t next=t->seq+(uint32_t)n;
    if(!ip_send_tcp(c,t->peer_ip,t->sport,t->dport,t->seq,t->ack,0x18u,d,n))
        return 0;
    if(n){
        t->seq=next;
        if(!tcp_wait_ack(c,t,next))return 0;
    }
    return 1;
}

static int tcp_close(const struct net_cfg*c,struct tcp_state*t){
    if(!ip_send_tcp(c,t->peer_ip,t->sport,t->dport,t->seq,t->ack,0x11u,0,0))
        return 0;
    t->seq++;
    t->state=TCP_FIN;
    return tcp_wait_ack(c,t,t->seq);
}

/* ----------------------------- HTTP -------------------------------- */

static uint32_t file_size8(const char*name){
    /* Для простоты используем последовательное чтение; отдельный syscall
       SYS_FILE_SIZE в ОС существует, но здесь не нужен ещё один проход API. */
    uint32_t fd,n,total=0;
    uint8_t b[512];
    fd=sc(SYS_FILE_OPEN,(uint32_t)name,FAT16_MODE_READ,0);
    if(fd==SYS_FAIL)return SYS_FAIL;
    for(;;){
        n=sc(SYS_FILE_READ,fd,(uint32_t)b,sizeof(b));
        if(n==SYS_FAIL){sc(SYS_FILE_CLOSE,fd,0,0);return SYS_FAIL;}
        if(n==0)break;
        total+=n;
        if(total>0x7ffff000u){sc(SYS_FILE_CLOSE,fd,0,0);return SYS_FAIL;}
    }
    sc(SYS_FILE_CLOSE,fd,0,0);
    return total;
}

static uint32_t append_dec(char*b,uint32_t p,uint32_t v){
    char t[11];uint32_t n=0;
    do{t[n++]=(char)('0'+v%10u);v/=10u;}while(v);
    while(n)b[p++]=t[--n];
    return p;
}

/* Сборка HTTP-заголовка. Возвращает длину или 0 при переполнении. */
static uint16_t append_host(char*b,uint16_t p,const uint8_t ip[4]){
    char q[12];uint32_t i;
    for(i=0;i<4;i++){
        dec(ip[i],q);
        {uint32_t j=0;while(q[j])b[p++]=q[j++];}
        if(i!=3)b[p++]='.';
    }
    b[p++]='\r';b[p++]='\n';
    return p;
}

static uint16_t make_request(char*b,const struct net_cfg*c,
                             const uint8_t dst[4],int send,uint32_t size){
    uint32_t p=0,i;
    const char *m=send?"POST ":"GET ";
    for(i=0;m[i];i++)b[p++]=m[i];
    for(i=0;c->path[i];i++)b[p++]=c->path[i];
    for(i=0;i<9;i++)b[p++]=(" HTTP/1.0"[i]);
    b[p++]='\r';b[p++]='\n';
    b[p++]='H';b[p++]='o';b[p++]='s';b[p++]='t';b[p++]=':';
    b[p++]=' ';
    p=append_host(b,(uint16_t)p,dst);
    if(send){
        const char x[]="Content-Length: ";
        for(i=0;x[i];i++)b[p++]=x[i];
        p=append_dec(b,p,size);
        b[p++]='\r';b[p++]='\n';
        {const char y[]="Content-Type: application/octet-stream\r\n";
         for(i=0;y[i];i++)b[p++]=y[i];}
    }
    {const char x[]="Connection: close\r\n\r\n";
     for(i=0;x[i];i++)b[p++]=x[i];}
    return (uint16_t)p;
}

/* ------------------------------ Main -------------------------------- */

static int send_file_http(const struct net_cfg*c,const uint8_t dst[4],
                          const char*name,struct tcp_state*t){
    uint32_t fd,n,size=file_size8(name);
    uint8_t filebuf[512];
    uint8_t req[320];
    uint16_t rn,piece;

    if(size==SYS_FAIL){put("NETDRV: SEND input file error\n");return 0;}
    fd=sc(SYS_FILE_OPEN,(uint32_t)name,FAT16_MODE_READ,0);
    if(fd==SYS_FAIL)return 0;

    rn=make_request((char*)req,c,dst,1,size);
    if(!http_send_segment(c,t,req,rn)){sc(SYS_FILE_CLOSE,fd,0,0);return 0;}

    for(;;){
        n=sc(SYS_FILE_READ,fd,(uint32_t)filebuf,sizeof(filebuf));
        if(n==SYS_FAIL){sc(SYS_FILE_CLOSE,fd,0,0);return 0;}
        if(n==0)break;
        /* 512 байт значительно меньше TCP MSS, поэтому один сегмент. */
        piece=(uint16_t)n;
        if(!http_send_segment(c,t,filebuf,piece)){
            sc(SYS_FILE_CLOSE,fd,0,0);return 0;
        }
    }
    sc(SYS_FILE_CLOSE,fd,0,0);
    put("NETDRV: SEND body complete\n");
    return 1;
}

static uint32_t file_size_sys(const char*name){
    return sc(SYS_FILE_SIZE,(uint32_t)name,0,0);
}

/*
 * Запись блока через существующий SYS_FILE_WRITE.
 *
 * Важная деталь ABI Toy OS:
 *   EAX = 8 (SYS_FILE_WRITE)
 *   EBX = дескриптор
 *   ECX = адрес буфера
 *   EDX = число байт
 *
 * Мы не считаем успешным запись, если ядро вернуло меньше запрошенного.
 */
static int write_block(uint32_t fd,const uint8_t*b,uint16_t n){
    uint32_t r;
    if(!n)return 1;
    r=sc(SYS_FILE_WRITE,fd,(uint32_t)b,n);
    return r==(uint32_t)n;
}

/*
 * Разбор HTTP-ответа сделан как поток:
 *   - заголовок может быть разбит между несколькими TCP-сегментами;
 *   - тело может начинаться в том же сегменте, что и конец заголовка;
 *   - FIN с данными обрабатывается до проверки FIN.
 *
 * Это устраняет старую ситуацию, когда соединение считалось успешно
 * завершённым, хотя первый HTTP DATA/FIN-сегмент был потерян.
 */
static int recv_file_http(const struct net_cfg*c,const uint8_t dst[4],
                          const char*name,struct tcp_state*t){
    uint32_t fd,written=0;
    uint8_t req[320];
    uint8_t header[1600];
    uint16_t rn,hused=0;
    int header_done=0;

    fd=sc(SYS_FILE_OPEN,(uint32_t)name,
         FAT16_MODE_WRITE|FAT16_MODE_CREATE|FAT16_MODE_TRUNC,0);
    if(fd==SYS_FAIL){
        put("NETDRV: RECV output file create failed\n");
        return 0;
    }

    rn=make_request((char*)req,c,dst,0,0);
    if(!http_send_segment(c,t,req,rn)){
        sc(SYS_FILE_CLOSE,fd,0,0);
        return 0;
    }

    for(;;){
        uint16_t n,tl,off,dlen;
        int r;
        const uint8_t*ip,*tcp;
        uint32_t seq;

        if(pending_valid){
            n=pending_len;
            for(uint16_t i=0;i<n;i++)rx_frame[i]=pending_frame[i];
            pending_valid=0;
            r=1;
        }else{
            r=ne_recv(rx_frame,&n,(uint32_t)c->timeout_s*50u);
        }

        if(r<0){
            sc(SYS_FILE_CLOSE,fd,0,0);
            return 0;
        }
        if(r==0){
            put("NETDRV: HTTP receive timeout\n");
            sc(SYS_FILE_CLOSE,fd,0,0);
            return 0;
        }

        if(!parse_ipv4_tcp(rx_frame,n,&ip,&tcp,&tl))continue;
        if(!ip_match(ip+12,t->peer_ip))continue;
        if(be16(tcp+0)!=t->dport||be16(tcp+2)!=t->sport)continue;

        off=(uint16_t)((tcp[12]>>4)*4u);
        if(off<20u||off>tl)continue;
        dlen=(uint16_t)(tl-off);
        seq=be32(tcp+4);

        /*
         * TCP sequence handling.
         * Повторный сегмент не записываем второй раз.
         * Сегмент "вперёд" пока не буферизуем: наш сервер Python
         * отправляет последовательно, поэтому подтверждаем текущую позицию.
         */
        if(seq<t->peer_seq){
            ip_send_tcp(c,t->peer_ip,t->sport,t->dport,
                        t->seq,t->peer_seq,0x10u,0,0);
            if(tcp[13]&0x01u)break;
            continue;
        }
        if(seq>t->peer_seq){
            ip_send_tcp(c,t->peer_ip,t->sport,t->dport,
                        t->seq,t->peer_seq,0x10u,0,0);
            continue;
        }

        if(dlen){
            const uint8_t*d=tcp+off;
            uint16_t pos=0;

            /*
             * Пока заголовок не найден, сохраняем только HTTP header.
             * Если в текущем сегменте уже есть body, оно записывается
             * непосредственно через SYS_FILE_WRITE.
             */
            if(!header_done){
                while(pos<dlen){
                    if(hused>=sizeof(header)){
                        put("NETDRV: HTTP header too large\n");
                        sc(SYS_FILE_CLOSE,fd,0,0);
                        return 0;
                    }
                    header[hused++]=d[pos++];
                    if(hused>=4u &&
                       header[hused-4u]=='\r' &&
                       header[hused-3u]=='\n' &&
                       header[hused-2u]=='\r' &&
                       header[hused-1u]=='\n'){
                        header_done=1;
                        break;
                    }
                }

                if(header_done && pos<dlen){
                    uint16_t x=(uint16_t)(dlen-pos);
                    if(!write_block(fd,d+pos,x)){
                        put("NETDRV: SYS_FILE_WRITE failed\n");
                        sc(SYS_FILE_CLOSE,fd,0,0);
                        return 0;
                    }
                    written+=x;
                }
            }else{
                if(!write_block(fd,d,dlen)){
                    put("NETDRV: SYS_FILE_WRITE failed\n");
                    sc(SYS_FILE_CLOSE,fd,0,0);
                    return 0;
                }
                written+=dlen;
            }

            t->peer_seq+=dlen;
            t->ack=t->peer_seq;

            /*
             * DATA всегда подтверждаем. Если в этом же сегменте FIN,
             * сначала подтверждаем DATA+FIN одним ACK.
             */
            if(!ip_send_tcp(c,t->peer_ip,t->sport,t->dport,
                            t->seq,t->ack,0x10u,0,0)){
                sc(SYS_FILE_CLOSE,fd,0,0);
                return 0;
            }

            if(tcp[13]&0x01u){
                t->peer_seq++;
                t->ack=t->peer_seq;
                if(!ip_send_tcp(c,t->peer_ip,t->sport,t->dport,
                                t->seq,t->ack,0x10u,0,0)){
                    sc(SYS_FILE_CLOSE,fd,0,0);
                    return 0;
                }
                break;
            }
        }else if(tcp[13]&0x01u){
            /* FIN без DATA: подтверждаем и завершаем только после EOF. */
            t->peer_seq++;
            t->ack=t->peer_seq;
            ip_send_tcp(c,t->peer_ip,t->sport,t->dport,
                        t->seq,t->ack,0x10u,0,0);
            break;
        }else if(tcp[13]&0x10u){
            /* Чистый ACK от сервера. Никаких данных не теряем. */
            continue;
        }
    }

    sc(SYS_FILE_CLOSE,fd,0,0);

    /*
     * Проверяем результат через тот же системный вызов, которым shell
     * выполняет "filesize". Это одновременно проверяет, что:
     *   1) SYS_FILE_WRITE действительно изменил FAT;
     *   2) SYS_FILE_CLOSE не потерял состояние;
     *   3) каталог содержит ненулевой размер файла.
     */
    {
        uint32_t actual=file_size_sys(name);
        if(actual==SYS_FAIL){
            put("NETDRV: RECV file size check failed\n");
            return 0;
        }
        if(actual!=written){
            put("NETDRV: RECV size mismatch\n");
            return 0;
        }
        put("NETDRV: RECV bytes=");
        {
            char nb[12];
            dec(actual,nb);
            put(nb);
        }
        put("\n");
    }

    if(!header_done){
        put("NETDRV: HTTP header missing\n");
        return 0;
    }

    put("NETDRV: RECV page saved\n");
    return 1;
}

static void fail_exit(void){
    sc(SYS_EXIT,EXE_FAIL,0,0);
    for(;;){}
}

__attribute__((section(".usertext"))) void program_main(void){
    const char *a0,*a1,*a2;
    struct net_cfg c;
    uint8_t dst[4],route_ip[4];
    uint32_t is_udp=0;
    const char *cfg_name="NET.CFG";
    char mode_norm[16],arg1_norm[16];

    __asm__ volatile("" : "=b"(a0), "=c"(a1), "=d"(a2));
    for(uint32_t i=0;i<sizeof(c);i++)((uint8_t*)&c)[i]=0;
    upper_copy(mode_norm,a0,sizeof(mode_norm));
    upper_copy(arg1_norm,a1,sizeof(arg1_norm));
    if(!a0||!a1||!a2||!a0[0]||!a1[0]||!a2[0]){
        put("NETDRV: usage IP SEND|RECV FILE.TXT or TELEMETRY ROLE PEER\n");
        fail_exit();
    }
    if(eq(mode_norm,"TELEMETRY")){
        is_udp=1;
        if(eq(arg1_norm,"MASTER"))cfg_name="NET_MAST.CFG";
        else if(eq(arg1_norm,"SLAVE1"))cfg_name="NET_SLV1.CFG";
        else if(eq(arg1_norm,"SLAVE2"))cfg_name="NET_SLV2.CFG";
    }

    if(!load_cfg_file(&c,cfg_name)){put("NETDRV: config open/parse failed\n");fail_exit();}
    if(c.base<0x100u||c.base>0x3f0u||(c.base&0x0fu)){
        put("NETDRV: invalid NE2000 BASE\n");fail_exit();
    }
    for(uint32_t i=0;i<6;i++)local_mac[i]=c.mac[i];

    put("NETDRV: NE2000 init\n");
    if(!ne_init(&c)){put("NETDRV: NIC init failed\n");fail_exit();}

    if(is_udp){
        if(eq(arg1_norm,"MASTER")){
            int ok=1;int port_ok=0;uint32_t port=0;int p;
            port=number(a2,&p);if(p&&port<=65535u&&port!=0u&&port==TELEMETRY_PORT)port_ok=1;
            if(!port_ok){put("NETDRV: master port must be 5000\n");fail_exit();}
            put("NETDRV: UDP MASTER listening\n");
            ok=telemetry_master(&c,(uint16_t)port);
            sc(SYS_EXIT,ok?EXE_OK:EXE_FAIL,0,0);for(;;){}
        }
        if(eq(arg1_norm,"SLAVE1")||eq(arg1_norm,"SLAVE2")){
            if(!parse_ip(a2,dst)){put("NETDRV: bad telemetry master IP\n");fail_exit();}
            if(((ip_u32(dst)^ip_u32(c.ip))&ip_u32(c.mask))==0u){
                for(uint32_t i=0;i<4;i++)route_ip[i]=dst[i];
            }else{
                for(uint32_t i=0;i<4;i++)route_ip[i]=c.gw[i];
            }
            put("NETDRV: Master ");put_ip(dst);put("\n");
            put("NETDRV: UDP SLAVE running independently\n");
            (void)telemetry_slave(&c,arg1_norm,dst);
            sc(SYS_EXIT,EXE_FAIL,0,0);for(;;){}
        }
        put("NETDRV: telemetry role must be MASTER, SLAVE1 or SLAVE2\n");
        fail_exit();
    }

    if(!parse_ip(a0,dst)){
        put("NETDRV: bad destination IP\n");fail_exit();
    }
    if(!eq(arg1_norm,"SEND")&&!eq(arg1_norm,"RECV")){
        put("NETDRV: direction must be SEND or RECV\n");fail_exit();
    }
    if(((ip_u32(dst)^ip_u32(c.ip))&ip_u32(c.mask))==0u){
        for(uint32_t i=0;i<4;i++)route_ip[i]=dst[i];
    }else{
        for(uint32_t i=0;i<4;i++)route_ip[i]=c.gw[i];
    }
    put("NETDRV: ARP ");put_ip(route_ip);put("\n");
    if(!arp_resolve(&c,route_ip)){put("NETDRV: ARP failed\n");fail_exit();}
    put("NETDRV: ARP OK\n");

    {
        struct tcp_state t;uint32_t ok=0;
        for(uint32_t i=0;i<4;i++)t.peer_ip[i]=dst[i];
        put("NETDRV: TCP connect ");put_ip(dst);put("\n");
        if(!tcp_connect(&c,dst,&t)){put("NETDRV: TCP connect failed\n");fail_exit();}
        if(eq(arg1_norm,"SEND"))ok=send_file_http(&c,dst,a2,&t)?1u:0u;
        else ok=recv_file_http(&c,dst,a2,&t)?1u:0u;
        if(ok){if(t.state==TCP_EST)tcp_close(&c,&t);put("NETDRV: OK\n");}
        else put("NETDRV: operation failed\n");
        sc(SYS_EXIT,ok?EXE_OK:EXE_FAIL,0,0);for(;;){}
    }
}

__asm__(".section .start,\"ax\"\n.global _start\n_start:\njmp _program_main\n");
