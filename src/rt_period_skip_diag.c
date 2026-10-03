/* RTDSKIP.EXE - Stage 5.3 periodic no-backlog diagnostic EXE1.
 * Ordinary ToyOS EXE1. The program deliberately runs one job for several
 * periods before SYS_RT_WAIT, so the kernel must skip stale release times and
 * create the next job from the first release strictly after completion.
 * Skipped releases are reported separately from actual deadline misses. */
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
#define SYS_CONSOLE_WRITE 1u
#define SYS_TIMER_GET     3u
#define SYS_RT_TIME_GET  45u
#define SYS_CONSOLE_POLL 37u
#define SYS_RT_WAIT      40u
#define SYS_RT_JOB_INFO  44u
#define SYS_EXIT         12u
static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}
static void put(const char*s){uint32_t n=0u;while(s[n])n++;sc(SYS_CONSOLE_WRITE,(uint32_t)s,n,0);}
static void put_u32(uint32_t v){char b[12];uint32_t i=0u,j;char t;if(v==0u){put("0");return;}while(v&&i<10u){b[i++]=(char)('0'+v%10u);v/=10u;}for(j=0;j<i/2u;j++){t=b[j];b[j]=b[i-1u-j];b[i-1u-j]=t;}b[i]=0;put(b);}
static uint32_t next_tick(uint32_t start){uint32_t now=start;while(now==start)now=sc(SYS_RT_TIME_GET,0,0,0);return now;}
static void print_info(const char*tag){uint32_t v[9];if(sc(SYS_RT_JOB_INFO,(uint32_t)v,0,0)!=0u){put("RTDSKIP: info FAIL\n");return;}put("RTDSKIP: ");put(tag);put(" job=");put_u32(v[0]);put(" release=");put_u32(v[1]);put(" deadline=");put_u32(v[2]);put(" now=");put_u32(v[3]);put(" next=");put_u32(v[4]);put(" active=");put_u32(v[5]);put(" missed=");put_u32(v[6]);put(" deadline_misses=");put_u32(v[7]);put(" skipped=");put_u32(v[8]);put("\n");}
__attribute__((section(".usertext"))) void program_main(void){
    uint32_t info[9],now,loops,wait_result;uint8_t key=0u;
    if(sc(SYS_RT_JOB_INFO,(uint32_t)info,0,0)!=0u){put("RTDSKIP: info FAIL\n");sc(SYS_EXIT,1,0,0);for(;;){}}
    print_info("start");
    for(;;){
        if(sc(SYS_CONSOLE_POLL,(uint32_t)&key,0,0)==0x1bu&&key==0x1bu){put("RTDSKIP: ESC -> stopped\n");sc(SYS_EXIT,0,0,0);for(;;){}}
        now=sc(SYS_RT_TIME_GET,0,0,0);loops=0u;
        /* Run for several release periods. With period=20ms this is five
           RT ticks (50ms), so at least several nominal releases become stale. */
        while(loops<5u){now=next_tick(now);loops++;}
        print_info("before-wait");
        wait_result=sc(SYS_RT_WAIT,0,0,0);
        if(wait_result==0xffffffffu){put("RTDSKIP: RT wait FAIL\n");sc(SYS_EXIT,1,0,0);for(;;){}}
        if(wait_result==1u)put("RTDSKIP: completed with deadline miss\n");
        else put("RTDSKIP: completed before deadline\n");
        print_info("next-job");
        /* One demonstrated transition is enough; the next job remains alive
           for a manual ESC test and can be inspected after its release. */
        sc(SYS_EXIT,0,0,0);for(;;){}
    }
}
__asm__(
".section .start,\"ax\"\n"
".global _start\n"
"_start:\n"
"jmp _program_main\n"
);
