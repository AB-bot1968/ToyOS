typedef unsigned int uint32_t;
__attribute__((section(".usertext"))) void program_main(void){
#if FAULT_KIND==6
    __asm__ volatile("ud2");
#elif FAULT_KIND==13
    __asm__ volatile("cli");
#elif FAULT_KIND==14
    *(volatile uint32_t*)0x50000000u=0x12345678u;
#else
#error FAULT_KIND must be 6, 13 or 14
#endif
    for(;;){}
}
__asm__(".section .start,\"ax\"\n.global _start\n_start:\njmp _program_main\n");
