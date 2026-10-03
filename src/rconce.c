typedef unsigned int uint32_t;
#define SYS_TIMER_GET 3u
#define SYS_FILE_OPEN 6u
#define SYS_FILE_WRITE 8u
#define SYS_FILE_CLOSE 9u
#define SYS_FILE_DELETE 10u
#define SYS_EXIT 12u
#define SYS_PROCESS_HEARTBEAT 60u
#define FAT16_MODE_READ 0x01u
#define FAT16_MODE_WRITE 0x02u
#define FAT16_MODE_CREATE 0x04u
#define FAT16_MODE_TRUNC 0x08u
static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}
__attribute__((section(".usertext"))) void program_main(void){
    static const char marker[]="RCONCE.DAT";uint32_t fd,leak,start,last,now,i;char one='1';
    fd=sc(SYS_FILE_OPEN,(uint32_t)marker,FAT16_MODE_READ,0u);
    if(fd==0xffffffffu){
        fd=sc(SYS_FILE_OPEN,(uint32_t)marker,FAT16_MODE_WRITE|FAT16_MODE_CREATE|FAT16_MODE_TRUNC,0u);
        if(fd==0xffffffffu){sc(SYS_EXIT,20u,0u,0u);for(;;){}}
        if(sc(SYS_FILE_WRITE,fd,(uint32_t)&one,1u)!=1u){sc(SYS_FILE_CLOSE,fd,0u,0u);sc(SYS_EXIT,21u,0u,0u);for(;;){}}
        (void)sc(SYS_FILE_CLOSE,fd,0u,0u);
        /* Deliberately leak one owned handle. Fault containment must reclaim it. */
        leak=sc(SYS_FILE_OPEN,(uint32_t)"/TST/TESTCORE.TST",FAT16_MODE_READ,0u);(void)leak;
        __asm__ volatile("ud2");
        for(;;){}
    }
    (void)sc(SYS_FILE_CLOSE,fd,0u,0u);
    start=sc(SYS_TIMER_GET,0u,0u,0u);last=start;
    for(;;){now=sc(SYS_TIMER_GET,0u,0u,0u);if(now!=last){last=now;(void)sc(SYS_PROCESS_HEARTBEAT,0u,0u,0u);}if((uint32_t)(now-start)>=25u)break;for(i=0;i<2000u;i++)__asm__ volatile("pause");}
    (void)sc(SYS_FILE_DELETE,(uint32_t)marker,0u,0u);
    sc(SYS_EXIT,0u,0u,0u);for(;;){}
}
__asm__(".section .start,\"ax\"\n.global _start\n_start:\njmp _program_main\n");
