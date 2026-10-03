/* RTD.EXE - Stage 1 Real-Time launcher for ToyOS.
 *
 * User-facing command:
 *   exec RTD.EXE SENSOR.EXE 50 50 3
 *   exec RTD.EXE SENSOR.EXE 10 10 3 3000
 *
 * The existing EXE1 ABI has three 16-byte argument slots. The shell therefore
 * passes SENSOR.EXE in slot 0 and "50 50 3" in slot 1. RTD validates the text
 * and asks the kernel to create one detached Ring-3 RT task. Period/deadline/
 * priority are applied by the RT scheduler without changing this command ABI.
 */
typedef unsigned int uint32_t;

#define SYS_CONSOLE_WRITE 1u
#define SYS_TIMER_GET     3u
#define SYS_RT_TIME_GET  45u
#define SYS_EXIT          12u
#define SYS_EXEC_ARG      34u
#define SYS_RT_START      38u
#define SYS_RT_STATUS     43u
#define EXEC_ARG_SIZE     16u
#define EXEC_ARG_COUNT    3u
#define QUEUE_NAME_SIZE   13u

typedef struct {
    char name[QUEUE_NAME_SIZE];
    uint32_t period_ms;
    uint32_t deadline_ms;
    uint32_t priority;
} rt_start_request_t;

static uint32_t syscall3(uint32_t n,uint32_t a,uint32_t b,uint32_t c){
    uint32_t r;
    __asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");
    return r;
}
static void put(const char*s){uint32_t n=0;while(s[n])n++;syscall3(SYS_CONSOLE_WRITE,(uint32_t)s,n,0);}
static void put_u32(uint32_t v){char b[12];uint32_t i=0,j;char t;if(v==0u){put("0");return;}while(v&&i<10u){b[i++]=(char)('0'+(v%10u));v/=10u;}for(j=0;j<i/2u;j++){t=b[j];b[j]=b[i-1u-j];b[i-1u-j]=t;}b[i]=0;put(b);}
static int is_digit(char c){return c>='0'&&c<='9';}
static int parse_u32(const char*s,uint32_t*out){uint32_t v=0,n=0;if(!s||!s[0])return 0;while(s[n]){uint32_t d;if(!is_digit(s[n]))return 0;d=(uint32_t)(s[n]-'0');if(v>429496729u||(v==429496729u&&d>5u))return 0;v=v*10u+d;n++;}*out=v;return 1;}
static void skip(const char**pp){const char*p=*pp;while(*p==' '||*p=='\t')p++;*pp=p;}
static int next_token(const char**pp,char*out,uint32_t cap){const char*p=*pp;uint32_t n=0;skip(&p);if(!*p)return 0;while(*p&&*p!=' '&&*p!='\t'){if(n+1u>=cap)return 0;out[n++]=*p++;}out[n]=0;*pp=p;return 1;}
static int get_arg(uint32_t index,char*out){uint32_t r;if(index>=EXEC_ARG_COUNT)return 0;if(index==0u){/* target */}r=syscall3(SYS_EXEC_ARG,index,(uint32_t)out,EXEC_ARG_SIZE);return r!=0xffffffffu;}

__attribute__((section(".usertext"))) void program_main(void){
    char target[EXEC_ARG_SIZE];
    char spec[EXEC_ARG_SIZE];
    const char*p;
    char tok[EXEC_ARG_SIZE];
    rt_start_request_t req;
    /* SENSOR1..SENSOR4 are detached background RT tasks.  They never
       have a console output mode; RTSTAT is the sole observability
       path.  The only optional RTD argument is the start delay. */
    uint32_t r,delay_ms=0;

    if(!get_arg(0u,target)||!get_arg(1u,spec)){
        put("RTD: argument read FAIL\n");
        syscall3(SYS_EXIT,1,0,0);
        for(;;){}
    }
    /* Initialize the request before writing parsed numeric fields.  The old
       order cleared the whole structure here and erased period/deadline/
       priority, so the kernel correctly rejected the request as -2. */
    uint32_t i;
    for(i=0;i<sizeof(req);i++)((unsigned char*)&req)[i]=0;
    p=spec;
    if(!next_token(&p,tok,sizeof(tok))||!parse_u32(tok,&req.period_ms)||
       !next_token(&p,tok,sizeof(tok))||!parse_u32(tok,&req.deadline_ms)||
       !next_token(&p,tok,sizeof(tok))||!parse_u32(tok,&req.priority)){
        put("RTD: usage exec RTD.EXE SENSOR.EXE PERIOD_MS DEADLINE_MS PRIORITY\n");
        syscall3(SYS_EXIT,1,0,0);
        for(;;){}
    }
    skip(&p);
    if(*p){
        if(!next_token(&p,tok,sizeof(tok))||!parse_u32(tok,&delay_ms)){
            put("RTD: usage exec RTD.EXE SENSOR.EXE PERIOD_MS DEADLINE_MS PRIORITY [DELAY_MS]\n");
            syscall3(SYS_EXIT,1,0,0);
            for(;;){}
        }
        skip(&p);
    }
    if(*p){
        put("RTD: invalid option\n");
        syscall3(SYS_EXIT,1,0,0);
        for(;;){}
    }
    if(*p||req.period_ms==0u||req.deadline_ms==0u||req.deadline_ms>req.period_ms||req.priority>255u){
        put("RTD: invalid period/deadline/priority\n");
        syscall3(SYS_EXIT,1,0,0);
        for(;;){}
    }
    /* v67.2: имя RT-цели нормализуется к ASCII upper-case до передачи
       в SYS_RT_START. Это сохраняет регистронезависимый интерфейс shell
       независимо от того, каким регистром пользователь ввёл SENSOR.EXE.
       Важно: меняется только имя исполняемого файла; числовые аргументы
       и другие данные пользователя не преобразуются. */
    for(i=0;i<QUEUE_NAME_SIZE-1u&&target[i];i++){
        char c=target[i];
        if(c>='a'&&c<='z')c=(char)(c-'a'+'A');
        req.name[i]=c;
    }
    if(i==0u){
        put("RTD: empty target\n");
        syscall3(SYS_EXIT,1,0,0);
        for(;;){}
    }
    if(delay_ms){
        uint32_t t0=syscall3(SYS_RT_TIME_GET,0,0,0),now;
        do { now=syscall3(SYS_RT_TIME_GET,0,0,0); } while((now-t0) < (delay_ms/10u + ((delay_ms%10u)!=0u)));
    }
    r=syscall3(SYS_RT_START,(uint32_t)&req,0,0);
    if(r!=0u){
        put("RTD: start FAIL code=");put_u32(r);
        if(r==0xffffffffu){
            uint32_t st[4];
            if(syscall3(SYS_RT_STATUS,(uint32_t)st,0,0)==0u){
                put(" active=");put_u32(st[0]);put(" free=");put_u32(st[1]);
            }
        }
        put("\n");
        syscall3(SYS_EXIT,1,0,0);
        for(;;){}
    }
    /* The shell/F10 launcher owns the user-visible success message.  RTD may
       be preempted immediately after SYS_RT_START; printing from RTD made the
       message race with the next queued launch and occasionally mixed names.
       Keep the launcher silent on success; failures are still reported above. */
    syscall3(SYS_EXIT,0,0,0);
    for(;;){}
}

__asm__(
".section .start,\"ax\"\n"
".global _start\n"
"_start:\n"
"jmp _program_main\n"
);
