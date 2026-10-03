typedef unsigned int uint32_t;
#define SYS_TIMER_GET 3u
#define SYS_EXIT 12u
#define SYS_PROCESS_HEARTBEAT 60u
static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}
__attribute__((section(".usertext"))) void program_main(void){uint32_t start=sc(SYS_TIMER_GET,0,0,0),last=start,i;for(;;){uint32_t now=sc(SYS_TIMER_GET,0,0,0);if(now!=last){last=now;(void)sc(SYS_PROCESS_HEARTBEAT,0,0,0);}if((uint32_t)(now-start)>=150u)break;for(i=0;i<2000u;i++)__asm__ volatile("pause");}sc(SYS_EXIT,0,0,0);for(;;){}}
__asm__(".section .start,\"ax\"\n.global _start\n_start:\njmp _program_main\n");
