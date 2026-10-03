/* FIX34 diagnostic EXE1: prove the kernel's current-process resolver from all
   three Ring-3 execution domains without background console output. */
typedef unsigned int uint32_t;
#define SYS_CONSOLE_WRITE 1u
#define SYS_EXIT 12u
#define SYS_RT_WAIT 40u
#define SYS_RT_DATA 50u
#define SYS_MT_DATA 53u
#define SYS_PROCESS_INFO 55u
#define TYPE_FG 1u
#define TYPE_MT 2u
#define TYPE_RT 3u
static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}
static uint32_t sl(const char*s){uint32_t n=0;while(s[n])n++;return n;}
static void put(const char*s){(void)sc(SYS_CONSOLE_WRITE,(uint32_t)s,sl(s),0u);}
static void addc(char*b,uint32_t*p,char c){if(*p<254u)b[(*p)++]=c;}
static void adds(char*b,uint32_t*p,const char*s){while(*s&&*p<254u)b[(*p)++]=*s++;}
static void addu(char*b,uint32_t*p,uint32_t v){char t[10];uint32_t n=0;if(!v){addc(b,p,'0');return;}while(v&&n<10u){t[n++]=(char)('0'+v%10u);v/=10u;}while(n)addc(b,p,t[--n]);}
static void make_line(char*b,uint32_t*q){uint32_t p=0;adds(b,&p,"PID:");addu(b,&p,q[0]);adds(b,&p,";TYPE:");adds(b,&p,q[1]==TYPE_FG?"FG":(q[1]==TYPE_MT?"MT":"RT"));adds(b,&p,";SLOT:");if(q[3]==0xffffffffu)addc(b,&p,'-');else{adds(b,&p,q[1]==TYPE_MT?"MT":"SLOT");addu(b,&p,q[3]+1u);}addc(b,&p,';');b[p]=0;}
struct mt_data_request{uint32_t op,mt_id,buffer,length,seq,session;};
static void publish_mt(char*b){struct mt_data_request r;r.op=0u;r.mt_id=0u;r.buffer=(uint32_t)b;r.length=sl(b);r.seq=0u;r.session=0u;(void)sc(SYS_MT_DATA,(uint32_t)&r,0u,0u);}
__attribute__((section(".usertext"))) void program_main(void){uint32_t q[10],r;char line[255];volatile uint32_t spin=1u;for(;;){r=sc(SYS_PROCESS_INFO,(uint32_t)q,1u,0u);if(r!=1u){if(q[1]==TYPE_FG)put("PROCID: resolver failed\n");sc(SYS_EXIT,1u,0u,0u);for(;;){}}make_line(line,q);if(q[1]==TYPE_FG){put("PROCID ");put(line);put("\n");sc(SYS_EXIT,0u,0u,0u);for(;;){}}if(q[1]==TYPE_MT){publish_mt(line);for(;;)spin=spin*1664525u+1013904223u;}if(q[1]==TYPE_RT){if(sc(SYS_RT_DATA,(uint32_t)line,sl(line),0u)==0xffffffffu){sc(SYS_EXIT,1u,0u,0u);for(;;){}}if(sc(SYS_RT_WAIT,0u,0u,0u)==0xffffffffu){sc(SYS_EXIT,1u,0u,0u);for(;;){}}continue;}sc(SYS_EXIT,1u,0u,0u);for(;;){}}}
__asm__(".section .start,\"ax\"\n.global _start\n_start:\njmp _program_main\n");
