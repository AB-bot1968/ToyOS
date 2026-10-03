/* QSTOP.EXE: immediate SYS_QUEUE_STOP test. When queued, the kernel restores the shell. */
typedef unsigned int uint32_t;
#define SYS_CONSOLE_WRITE 1u
#define SYS_QUEUE_STOP 16u
static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}
static void put(const char*s){uint32_t n=0;while(s[n])n++;sc(SYS_CONSOLE_WRITE,(uint32_t)s,n,0);}
__attribute__((section(".usertext"))) void program_main(void){uint32_t r;put("QSTOP: requesting queue stop\n");r=sc(SYS_QUEUE_STOP,0,0,0);if(r==0xffffffffu)put("QSTOP: stop request rejected\n");for(;;){}}
__asm__(".section .start,\"ax\"\n.global _start\n_start:\njmp _program_main\n");
