/* RTTIME.EXE - Stage 6.1 RT timebase diagnostic EXE1.
 * Ordinary ToyOS EXE1. Verifies the dedicated 10 ms RT timebase while the
 * legacy SYS_TIMER_GET clock remains 20 ms per tick. */
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
#define SYS_CONSOLE_WRITE 1u
#define SYS_TIMER_GET 3u
#define SYS_CONSOLE_POLL 37u
#define SYS_RT_TIME_GET 45u
#define SYS_EXIT 12u
static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}
static void put(const char*s){uint32_t n=0;while(s[n])n++;sc(SYS_CONSOLE_WRITE,(uint32_t)s,n,0);}
static void put_u32(uint32_t v){char b[12];uint32_t i=0,j;char t;if(v==0u){put("0");return;}while(v&&i<10u){b[i++]=(char)('0'+v%10u);v/=10u;}for(j=0;j<i/2u;j++){t=b[j];b[j]=b[i-1u-j];b[i-1u-j]=t;}b[i]=0;put(b);}
static uint32_t wait_rt_change(uint32_t base){uint32_t now;do{now=sc(SYS_RT_TIME_GET,0,0,0);}while(now==base);return now;}
__attribute__((section(".usertext"))) void program_main(void){
    uint32_t rt0=sc(SYS_RT_TIME_GET,0,0,0),sy0=sc(SYS_TIMER_GET,0,0,0),rt,sy,i;uint8_t key=0u;
    put("RTTIME: started ordinary EXE1 (ESC to stop) RT=100Hz/10ms SYSTEM=50Hz/20ms\n");
    for(i=0u;i<12u;i++){
        if(sc(SYS_CONSOLE_POLL,(uint32_t)&key,0,0)==0x1bu&&key==0x1bu){put("RTTIME: ESC -> stopped\n");sc(SYS_EXIT,0,0,0);for(;;){}}
        rt=wait_rt_change(rt0);sy=sc(SYS_TIMER_GET,0,0,0);
        put("RTTIME: rt_delta=");put_u32(rt-rt0);put(" system_delta=");put_u32(sy-sy0);put(" rt=");put_u32(rt);put(" system=");put_u32(sy);put("\n");
        rt0=rt;sy0=sy;
    }
    put("RTTIME: 12 RT ticks observed; expected 12 RT ticks and about 6 system ticks\n");
    sc(SYS_EXIT,0,0,0);for(;;){}
}
__asm__(
".section .start,\"ax\"\n"
".global _start\n"
"_start:\n"
"jmp _program_main\n"
);
