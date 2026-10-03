/* SENSOR.EXE - quiet background RT application for ToyOS.
 *
 * v67.11-B-FIX1:
 * A detached RT task must never write periodic status text directly to the
 * interactive shell. Such writes race with the shell prompt/input cursor and
 * can make a valid command appear corrupted. Timing information belongs to
 * RTSTAT; application data belongs to RTDATA. SENSOR therefore remains silent
 * and only exits when the kernel marks its per-task stop request.
 */
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
#define SYS_RT_INFO      39u
#define SYS_RT_WAIT      40u
#define SYS_EXIT         12u

static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}

__attribute__((section(".usertext"))) void program_main(void){
    /* SENSOR is a true background task: it has no console/keyboard path.
       rtstat stop SLOTn changes the kernel task state directly, so polling
       the console here is both unnecessary and harmful to isolation. */
    for(;;){
        if(sc(SYS_RT_WAIT,0,0,0)==0xffffffffu){
            sc(SYS_EXIT,1,0,0);
            for(;;){}
        }
    }
}
__asm__(
".section .start,\"ax\"\n"
".global _start\n"
"_start:\n"
"jmp _program_main\n"
);
