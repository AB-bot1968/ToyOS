typedef unsigned int uint32_t;
#ifndef EXIT_CODE
#define EXIT_CODE 0
#endif
#define SYS_EXIT 12u
static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}
__attribute__((section(".usertext"))) void program_main(void){sc(SYS_EXIT,(uint32_t)EXIT_CODE,0u,0u);for(;;){}}
__asm__(".section .start,\"ax\"\n.global _start\n_start:\njmp _program_main\n");
