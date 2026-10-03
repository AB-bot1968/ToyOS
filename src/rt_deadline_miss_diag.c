/* RTDMISS.EXE - Stage 5.2 soft-deadline diagnostic EXE1.
 * Ordinary ToyOS EXE1: deliberately keeps one job alive after its absolute
 * deadline so the scheduler's best-effort policy can be observed.  A missed
 * job must continue only when no on-time RT job needs the CPU, and it must
 * eventually complete through SYS_RT_WAIT.
 */
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
#define SYS_CONSOLE_WRITE 1u
#define SYS_TIMER_GET     3u
#define SYS_RT_TIME_GET  45u
#define SYS_CONSOLE_POLL 37u
#define SYS_RT_WAIT      40u
#define SYS_RT_DEADLINE_INFO 42u
#define SYS_RT_TRACE      41u
#define SYS_EXIT         12u
static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}
static void put(const char*s){uint32_t n=0u;while(s[n])n++;sc(SYS_CONSOLE_WRITE,(uint32_t)s,n,0);}
static void put_u32(uint32_t v){char b[12];uint32_t i=0u,j;char t;if(v==0u){put("0");return;}while(v&&i<10u){b[i++]=(char)('0'+v%10u);v/=10u;}for(j=0;j<i/2u;j++){t=b[j];b[j]=b[i-1u-j];b[i-1u-j]=t;}b[i]=0;put(b);}
static uint32_t next_tick(uint32_t start){uint32_t now=start;while(now==start)now=sc(SYS_RT_TIME_GET,0,0,0);return now;}
static void trace(void){uint32_t t[5];if(sc(SYS_RT_TRACE,(uint32_t)t,0,0)!=0u)return;put("RTDMISS: dispatch=");put_u32(t[3]);put(" slot=");put_u32(t[0]);put(" priority=");put_u32(t[1]);put(" switches=");put_u32(t[2]);put("\n");}
__attribute__((section(".usertext"))) void program_main(void){
    uint32_t info[5],now,loops=0u;uint8_t key=0u;uint32_t saw_miss=0u;
    if(sc(SYS_RT_DEADLINE_INFO,(uint32_t)info,0,0)!=0u){put("RTDMISS: info FAIL\n");sc(SYS_EXIT,1,0,0);for(;;){}}
    now=info[2];
    put("RTDMISS: started release=");put_u32(info[0]);put(" deadline=");put_u32(info[1]);put(" now=");put_u32(now);put("\n");
    trace();
    while(loops<6u){
        if(sc(SYS_CONSOLE_POLL,(uint32_t)&key,0,0)==0x1bu&&key==0x1bu){put("RTDMISS: ESC -> stopped\n");sc(SYS_EXIT,0,0,0);for(;;){}}
        now=next_tick(now);
        if(sc(SYS_RT_DEADLINE_INFO,(uint32_t)info,0,0)!=0u)break;
        put("RTDMISS: sample release=");put_u32(info[0]);put(" deadline=");put_u32(info[1]);put(" now=");put_u32(info[2]);put(" missed=");put_u32(info[4]);put("\n");
        trace();
        if(info[4]!=0u&&!saw_miss){put("RTDMISS: job missed -> best-effort\n");saw_miss=1u;}
        loops++;
    }
    {uint32_t r=sc(SYS_RT_WAIT,0,0,0);if(r==1u)put("RTDMISS: completed with deadline miss\n");else if(r==0xffffffffu)put("RTDMISS: RT wait FAIL\n");else put("RTDMISS: completed before deadline\n");}
    sc(SYS_EXIT,0,0,0);for(;;){}
}
__asm__(
".section .start,\"ax\"\n"
".global _start\n"
"_start:\n"
"jmp _program_main\n"
);
