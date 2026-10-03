/* QPASS.EXE: minimal queue member. It must terminate with SYS_EXIT. */
typedef unsigned int uint32_t;
#define SYS_CONSOLE_WRITE 1u
#define SYS_EXIT 12u
#define SYS_LS 24u
static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}
static void put(const char*s){uint32_t n=0;while(s[n])n++;sc(SYS_CONSOLE_WRITE,(uint32_t)s,n,0);}
__attribute__((section(".usertext"))) void program_main(void){put("QPASS: queue member executed; calling SYS_LS\n");if(sc(SYS_LS,0,0,0)==0)put("QPASS: SYS_LS PASS\n");else put("QPASS: SYS_LS FAIL\n");sc(SYS_EXIT,0,0,0);for(;;){}}
__asm__(".section .start,\"ax\"\n.global _start\n_start:\njmp _program_main\n");
