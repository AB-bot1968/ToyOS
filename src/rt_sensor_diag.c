/* SENSOR1..SENSOR8.EXE - detached RT diagnostic sensors with RTDATA publish.
 *
 * Each binary is the same EXE1 program compiled with RT_SENSOR_DIAG_ID=1..8.
 * It never writes to the console and never reads shell input.  Once per RT
 * job it obtains the kernel timing sample, formats its own application result
 * as an opaque NAME:VALUE; record and publishes the complete snapshot through
 * SYS_RT_DATA.  The kernel binds the snapshot to the current RT slot.
 *
 * Current diagnostic SENSOR payload:
 *   D1 sensor id; D2 job sequence; D3 observed interval (10-ms ticks);
 *   D4 signed jitter; D5 release lateness; D6 dispatch latency;
 *   D7 cumulative deadline misses; D8 cumulative skipped releases.
 */
#ifndef RT_SENSOR_DIAG_ID
#define RT_SENSOR_DIAG_ID 1
#endif
#if RT_SENSOR_DIAG_ID < 1 || RT_SENSOR_DIAG_ID > 8
#error "RT_SENSOR_DIAG_ID must be 1..8"
#endif
#define SYS_RT_INFO 39u
#define SYS_RT_WAIT 40u
#define SYS_RT_EXEC_INFO 47u
#define SYS_RT_DATA 50u
#define SYS_EXIT 12u
#define RT_DATA_MAX 255u
typedef unsigned int uint32_t;
typedef signed int int32_t;
static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}
static void addc(char*b,uint32_t*p,char c){if(*p<RT_DATA_MAX)b[(*p)++]=c;}
static void adds(char*b,uint32_t*p,const char*s){while(*s&&*p<RT_DATA_MAX)b[(*p)++]=*s++;}
static void addu(char*b,uint32_t*p,uint32_t v){char t[10];uint32_t n=0u;if(v==0u){addc(b,p,'0');return;}while(v&&n<10u){t[n++]=(char)('0'+v%10u);v/=10u;}while(n)addc(b,p,t[--n]);}
static void addi(char*b,uint32_t*p,int32_t v){uint32_t u;if(v<0){addc(b,p,'-');u=(uint32_t)(-(v+1))+1u;}else u=(uint32_t)v;addu(b,p,u);}
static void field_u(char*b,uint32_t*p,const char*n,uint32_t v){adds(b,p,n);addc(b,p,':');addu(b,p,v);addc(b,p,';');}
static void field_i(char*b,uint32_t*p,const char*n,int32_t v){adds(b,p,n);addc(b,p,':');addi(b,p,v);addc(b,p,';');}
__attribute__((section(".usertext"))) void program_main(void){
    uint32_t v[14];char data[RT_DATA_MAX+1u];
    for(;;){
        uint32_t n=0u;int32_t jitter;
        /* FIX60Z: RT diagnostic telemetry is best-effort.  A transient
           diagnostic-info failure must not kill the real-time task: doing so
           destroys scheduler progress and makes SLOT stop semantics observe
           an EXITed task instead of the configured RT workload. */
        if(sc(SYS_RT_EXEC_INFO,(uint32_t)v,0,0)!=0u){
            (void)sc(SYS_RT_WAIT,0,0,0);
            continue;
        }
        jitter=(int32_t)v[4]-(int32_t)v[1];
        field_u(data,&n,"D1",(uint32_t)RT_SENSOR_DIAG_ID);
        field_u(data,&n,"D2",v[0]);
        field_u(data,&n,"D3",v[4]);
        field_i(data,&n,"D4",jitter);
        field_u(data,&n,"D5",v[5]);
        field_u(data,&n,"D6",v[7]);
        field_u(data,&n,"D7",v[13]);
        field_u(data,&n,"D8",v[12]);
        data[n]=0;
        /* Publish the finished local snapshot before ending this RT job. */
        /* Telemetry publication is also diagnostic-only.  Completing the
           RT job remains mandatory even if its optional snapshot cannot be
           published; RT_WAIT is the scheduler lifecycle boundary. */
        (void)sc(SYS_RT_DATA,(uint32_t)data,n,0);
        if(sc(SYS_RT_WAIT,0,0,0)==0xffffffffu){
            /* If the scheduler rejects WAIT, retry from the task context
               rather than converting a diagnostic failure into task death. */
            continue;
        }
    }
}
__asm__(".section .start,\"ax\"\n.global _start\n_start:\njmp _program_main\n");
