/* QPORT.EXE: proves queue members can use privileged I/O through syscalls. */
typedef unsigned int uint32_t;
#define SYS_CONSOLE_WRITE 1u
#define SYS_EXIT 12u
#define SYS_PORT_OUT8 13u
#define SYS_PORT_IN8 14u
static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}
static void put(const char*s){uint32_t n=0;while(s[n])n++;sc(SYS_CONSOLE_WRITE,(uint32_t)s,n,0);}
__attribute__((section(".usertext"))) void program_main(void){uint32_t r;put("QPORT: port I/O test\n");r=sc(SYS_PORT_IN8,0x64u,0,0);if(r==0xffffffffu)put("QPORT: IN8 FAIL\n");else put("QPORT: IN8 OK\n");r=sc(SYS_PORT_OUT8,0x80u,0,0);put(r==0?"QPORT: OUT8 OK\n":"QPORT: OUT8 FAIL\n");sc(SYS_EXIT,0,0,0);for(;;){}}
__asm__(".section .start,\"ax\"\n.global _start\n_start:\njmp _program_main\n");
