typedef unsigned int uint32_t;
#define SYS_EXIT 12u
#define SYS_MT_DATA 53u
struct mt_data_request {uint32_t op,mt_id,buffer,length,seq,session;};
static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}
static uint32_t add(char*d,uint32_t n,uint32_t cap,const char*s){uint32_t i=0;while(s[i]&&n+1u<cap)d[n++]=s[i++];d[n]=0;return n;}
__attribute__((section(".usertext"))) void program_main(void){
    uint32_t argc;char**argv;uint32_t i,n=0;char line[240];struct mt_data_request q;
    __asm__ volatile("movl %%ebx,%0; movl %%ecx,%1":"=r"(argc),"=r"(argv));line[0]=0;n=add(line,n,sizeof(line),"ARGV:");
    for(i=0;i<argc;i++){if(i)n=add(line,n,sizeof(line),"|");n=add(line,n,sizeof(line),argv[i]);}
    q.op=0u;q.mt_id=0u;q.buffer=(uint32_t)line;q.length=n;q.seq=0u;q.session=0u;(void)sc(SYS_MT_DATA,(uint32_t)&q,0,0);
    sc(SYS_EXIT,0,0,0);for(;;){}
}
__asm__(".section .start,\"ax\"\n.global _start\n_start:\njmp _program_main\n");
