/* QNESTED.EXE: starts a child queue from an EXE that is already a member
   of a parent execq. When QPASS/QPORT finish, the parent queue must resume
   with its next member. */
typedef unsigned int uint32_t;
#define SYS_CONSOLE_WRITE 1u
#define SYS_EXEC_QUEUE 15u
#define SYS_EXIT 12u
static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}
static void put(const char*s){uint32_t n=0;while(s[n])n++;sc(SYS_CONSOLE_WRITE,(uint32_t)s,n,0);}
static const char names[26]={
    'Q','P','A','S','S','.', 'E','X','E',0,0,0,0,
    'Q','P','O','R','T','.', 'E','X','E',0,0,0,0
};
__attribute__((section(".usertext"))) void program_main(void){
    uint32_t r;
    put("QNESTED: starting child queue QPASS -> QPORT\n");
    r=sc(SYS_EXEC_QUEUE,(uint32_t)names,2,1);
    if(r!=0)put("QNESTED: child queue start returned error\n");
    sc(SYS_EXIT,0,0,0);
    for(;;){}
}
__asm__(".section .start,\"ax\"\n.global _start\n_start:\njmp _program_main\n");
