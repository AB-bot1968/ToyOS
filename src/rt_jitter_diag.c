/* JITTER.EXE - Stage 6.2 executable runtime verification.
 *
 * The program is an ordinary EXE1 launched through RTD.EXE.  It observes the
 * kernel's measured release-service timestamps rather than assuming that the
 * requested period is the actual period.  For each new RT job it reports:
 *   - scheduled release tick;
 *   - observed release-service tick;
 *   - interval from the previous observed release;
 *   - release lateness;
 *   - first-dispatch latency measured from the same release.
 *
 * One RT tick is 10 ms in Stage 6.1.  The test deliberately uses a 10 ms
 * period so an interval of exactly one tick is the nominal result.
 */
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
#define SYS_CONSOLE_WRITE 1u
#define SYS_CONSOLE_POLL 37u
#define SYS_RT_WAIT 40u
#define SYS_RT_INFO 39u
#define SYS_RT_JITTER_INFO 46u
#define SYS_EXIT 12u

static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){
    uint32_t r;
    __asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");
    return r;
}
static void put(const char*s){uint32_t n=0;while(s[n])n++;sc(SYS_CONSOLE_WRITE,(uint32_t)s,n,0);}
static void dec(uint32_t v,char*b){
    static const uint32_t p[10]={1000000000u,100000000u,10000000u,1000000u,100000u,10000u,1000u,100u,10u,1u};
    uint32_t d,st=0,i;
    for(i=0;i<10;i++){d=0;while(v>=p[i]){v-=p[i];d++;}if(d||st||i==9){*b++=(char)('0'+d);st=1;}}
    *b=0;
}
static uint32_t jitter(uint32_t*v){return sc(SYS_RT_JITTER_INFO,(uint32_t)v,0,0);}
static void line(uint32_t sample,const uint32_t*v){
    char b[12];
    put("JITTER: sample=");dec(sample,b);put(b);
    put(" job=");dec(v[0],b);put(b);
    put(" release=");dec(v[2],b);put(b);
    put(" observed=");dec(v[3],b);put(b);
    put(" interval_ticks=");dec(v[4],b);put(b);
    put(" interval_ms=");dec(v[4]*10u,b);put(b);
    put(" late_ticks=");dec(v[7],b);put(b);
    put(" dispatch_latency_ticks=");dec(v[10],b);put(b);
    put("\n");
}
__attribute__((section(".usertext"))) void program_main(void){
    uint32_t v[13],last_job=0u,samples=0u,interval_count=0u,sum=0u,min=0xffffffffu,max=0u,maxlate=0u,maxdispatch=0u;
    uint8_t key=0u;
    put("JITTER: Stage 6.2 release/dispatch verification\n");
    put("JITTER: target period = 10 ms (1 RT tick); collecting 50 releases\n");
    for(;;){
        if(sc(SYS_CONSOLE_POLL,(uint32_t)&key,0,0)==0x1bu&&key==0x1bu){
            put("JITTER: ESC -> stopped\n");sc(SYS_EXIT,0,0,0);for(;;){}
        }
        if(jitter(v)!=0u){put("JITTER: info FAIL\n");sc(SYS_EXIT,1,0,0);for(;;){}
        }
        if(v[0]!=last_job){
            last_job=v[0];samples++;
            if(samples>1u){
                uint32_t x=v[4];sum+=x;interval_count++;
                if(x<min)min=x;
                if(x>max)max=x;
            }
            if(v[8]>maxlate)maxlate=v[8];
            if(v[10]>maxdispatch)maxdispatch=v[10];
            line(samples,v);
            if(samples>=50u)break;
        }
        if(sc(SYS_RT_WAIT,0,0,0)==0xffffffffu){put("JITTER: RT wait FAIL\n");sc(SYS_EXIT,1,0,0);for(;;){}
        }
    }
    put("JITTER: SUMMARY\n");
    {char b[12];
        put("JITTER: samples=");dec(samples,b);put(b);
        put(" intervals=");dec(interval_count,b);put(b);
        put(" avg_interval_ticks=");dec(interval_count?sum/interval_count:0u,b);put(b);
        put(" avg_interval_ms=");dec(interval_count?(sum*10u)/interval_count:0u,b);put(b);
        put(" min_interval_ticks=");dec(interval_count?min:0u,b);put(b);
        put(" max_interval_ticks=");dec(max,b);put(b);
        put(" max_release_late_ticks=");dec(maxlate,b);put(b);
        put(" max_dispatch_latency_ticks=");dec(maxdispatch,b);put(b);
        put("\n");
    }
    if(interval_count&&min==1u&&max==1u&&maxlate==0u){
        put("JITTER: PASS nominal 10-ms release interval observed without release lateness\n");
        sc(SYS_EXIT,0,0,0);
    }else{
        put("JITTER: OBSERVE deviation from nominal 10-ms period (see samples/summary)\n");
        sc(SYS_EXIT,2,0,0);
    }
    for(;;){}
}
__asm__(
".section .start,\"ax\"\n"
".global _start\n"
"_start:\n"
"jmp _program_main\n"
);
