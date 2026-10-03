typedef unsigned int uint32_t;
#define SYS_EXIT 12u
#define SYS_FILE_OPEN 6u
#define FAT16_MODE_READ 1u
static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}
__attribute__((section(".usertext"))) void program_main(void){
    uint32_t fd=sc(SYS_FILE_OPEN,(uint32_t)"/TST/TESTCORE.TST",FAT16_MODE_READ,0u);
    sc(SYS_EXIT,fd==0xffffffffu?7u:0u,0u,0u);
    for(;;){}
}
__asm__(".section .start,\"ax\"\n.global _start\n_start:\njmp _program_main\n");
