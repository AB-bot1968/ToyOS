/* Minimal standalone Ring-3 executable for the Toy OS EXE1 format. */
typedef unsigned int uint32_t;

#define SYS_CONSOLE_WRITE 1u
#define SYS_EXIT          12u
#define SYS_PORT_OUT8    13u
#define SYS_PORT_IN8     14u
#define SYS_EXEC_QUEUE  15u
#define SYS_QUEUE_STOP  16u

__attribute__((section(".usertext"))) static uint32_t syscall3(uint32_t n,uint32_t a,uint32_t b,uint32_t c){
    uint32_t r;
    __asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");
    return r;
}
__attribute__((used,section(".usertext"))) static uint32_t sys_port_out8(uint32_t port,uint32_t value){return syscall3(SYS_PORT_OUT8,port,value,0);}
__attribute__((used,section(".usertext"))) static uint32_t sys_port_in8(uint32_t port){return syscall3(SYS_PORT_IN8,port,0,0);}
__attribute__((used,section(".usertext"))) static uint32_t sys_exec_queue(const char*names,uint32_t count,uint32_t repetitions){return syscall3(SYS_EXEC_QUEUE,(uint32_t)names,count,repetitions);}
__attribute__((used,section(".usertext"))) static uint32_t sys_queue_stop(void){return syscall3(SYS_QUEUE_STOP,0,0,0);}
__attribute__((section(".usertext"))) static void put(const char*s){
    uint32_t n=0;while(s[n])n++;
    syscall3(SYS_CONSOLE_WRITE,(uint32_t)s,n,0);
}
__attribute__((section(".usertext"))) void program_main(void){
    put("Hello from HELLO.EXE (Ring 3)\n");
    syscall3(SYS_EXIT,0,0,0);
    for(;;){}
}
__asm__(
".section .start,\"ax\"\n"
".global _start\n"
"_start:\n"
"jmp _program_main\n"
);
