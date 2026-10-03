/* DEADLINE.EXE - Stage 5.1 absolute-deadline diagnostic EXE1.
 * It is an ordinary ToyOS EXE1 program.  The kernel creates one RT job with
 * an absolute release tick and deadline tick; this program prints both while
 * the current job is active so the deadline can be checked against rt_time_ticks.
 */
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
#define SYS_CONSOLE_WRITE 1u
#define SYS_TIMER_GET     3u
#define SYS_RT_TIME_GET  45u
#define SYS_CONSOLE_POLL 37u
#define SYS_RT_INFO      39u
#define SYS_RT_WAIT      40u
#define SYS_RT_DEADLINE_INFO 42u
#define SYS_EXIT         12u
static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}
static void put(const char*s){uint32_t n=0u;while(s[n])n++;sc(SYS_CONSOLE_WRITE,(uint32_t)s,n,0);}
static void put_u32(uint32_t v){char b[12];uint32_t i=0u,j;char t;if(v==0u){put("0");return;}while(v&&i<10u){b[i++]=(char)('0'+v%10u);v/=10u;}for(j=0;j<i/2u;j++){t=b[j];b[j]=b[i-1u-j];b[i-1u-j]=t;}b[i]=0;put(b);}
static uint32_t timer_wait_change(uint32_t start){uint32_t now=start;while(now==start)now=sc(SYS_RT_TIME_GET,0,0,0);return now;}
__attribute__((section(".usertext"))) void program_main(void){
    uint32_t info[5],now,release,deadline,waits=0u;uint8_t key=0u;
    if(sc(SYS_RT_DEADLINE_INFO,(uint32_t)info,0,0)!=0u){put("DEADLINE: info FAIL\n");sc(SYS_EXIT,1,0,0);for(;;){}}
    release=info[0];deadline=info[1];now=info[2];
    put("DEADLINE: started release=");put_u32(release);put(" deadline=");put_u32(deadline);put(" now=");put_u32(now);put(" missed=");put_u32(info[4]);put("\n");
    while(waits<3u){
        if(sc(SYS_CONSOLE_POLL,(uint32_t)&key,0,0)==0x1bu&&key==0x1bu){put("DEADLINE: ESC -> stopped\n");sc(SYS_EXIT,0,0,0);for(;;){}}
        now=timer_wait_change(now);
        if(sc(SYS_RT_DEADLINE_INFO,(uint32_t)info,0,0)!=0u)break;
        put("DEADLINE: sample release=");put_u32(info[0]);put(" deadline=");put_u32(info[1]);put(" now=");put_u32(info[2]);put(" missed=");put_u32(info[4]);put("\n");
        waits++;
    }
    {uint32_t r=sc(SYS_RT_WAIT,0,0,0);if(r==1u)put("DEADLINE: job completed after deadline (miss)\n");else if(r==0xffffffffu)put("DEADLINE: RT wait FAIL\n");else put("DEADLINE: job completed before deadline\n");}
    sc(SYS_EXIT,0,0,0);for(;;){}
}
__asm__(
".section .start,\"ax\"\n"
".global _start\n"
"_start:\n"
"jmp _program_main\n"
);
