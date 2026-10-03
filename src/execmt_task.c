/* FIX17 minimal EXECMT task.
   The same source is compiled 25 times with -DEXECMT_ID=1..25.
   Background MT tasks never access the console, block/wake other tasks, or
   exit by themselves.  They publish one small RUN snapshot to their own
   kernel MTDATA slot, then execute forever and are switched only by the
   kernel MT time quantum (or stopped explicitly by MTSTOP). */
typedef unsigned int uint32_t;
#ifndef EXECMT_ID
#define EXECMT_ID 0
#endif
#define SYS_MT_DATA 53u
static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}
#define MT_STR1(x) #x
#define MT_STR(x) MT_STR1(x)
#if EXECMT_ID < 10
__attribute__((section(".usercode_data"),used)) static const char mt_run[] = "MT0" MT_STR(EXECMT_ID) ":RUN;";
#else
__attribute__((section(".usercode_data"),used)) static const char mt_run[] = "MT" MT_STR(EXECMT_ID) ":RUN;";
#endif
struct mt_data_request {uint32_t op,mt_id,buffer,length,seq,session;};
static void mt_publish(const char *p,uint32_t n){
    struct mt_data_request q;
    q.op=0u;q.mt_id=0u;q.buffer=(uint32_t)p;q.length=n;q.seq=0u;q.session=0u;
    (void)sc(SYS_MT_DATA,(uint32_t)&q,0u,0u);
}
__attribute__((section(".usertext"))) void program_main(void){
    volatile uint32_t spin=(uint32_t)EXECMT_ID;
    mt_publish(mt_run,(uint32_t)(sizeof(mt_run)-1u));
    for(;;){
        /* Deliberately no syscall here.  Volatile work prevents optimization
           into an empty/removed function while keeping the task CPU-bound. */
        spin=spin*1664525u+1013904223u;
    }
}
__asm__(".section .start,\"ax\"\n.global _start\n_start:\njmp _program_main\n");
