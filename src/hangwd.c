typedef unsigned int uint32_t;
__attribute__((section(".usertext"))) void program_main(void){for(;;)__asm__ volatile("pause");}
__asm__(".section .start,\"ax\"\n.global _start\n_start:\njmp _program_main\n");
