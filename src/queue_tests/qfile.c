/* QFILE.EXE: demonstrates CREATE/TRUNC write followed by a separate APPEND
   open/write/close operation on the same FAT16 text file. */
typedef unsigned int uint32_t;
#define SYS_CONSOLE_WRITE 1u
#define SYS_FILE_OPEN 6u
#define SYS_FILE_WRITE 8u
#define SYS_FILE_CLOSE 9u
#define SYS_EXIT 12u
#define MODE_WRITE 0x02u
#define MODE_CREATE 0x04u
#define MODE_TRUNC 0x08u
#define MODE_APPEND 0x10u
static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}
static uint32_t sl(const char*s){uint32_t n=0;while(s[n])n++;return n;}
static void put(const char*s){sc(SYS_CONSOLE_WRITE,(uint32_t)s,sl(s),0);}
static const char name[]="EXELOG.TXT";
static const char first[]="QFILE: first write\n";
static const char second[]="QFILE: append write\n";
__attribute__((section(".usertext"))) void program_main(void){
    uint32_t fd,n1,n2;
    put("QFILE: writing EXELOG.TXT\n");
    fd=sc(SYS_FILE_OPEN,(uint32_t)name,MODE_WRITE|MODE_CREATE|MODE_TRUNC,0);
    if(fd==0xffffffffu){put("QFILE: open write FAIL\n");sc(SYS_EXIT,0,0,0);for(;;){}}
    n1=sc(SYS_FILE_WRITE,fd,(uint32_t)first,sl(first));
    sc(SYS_FILE_CLOSE,fd,0,0);
    fd=sc(SYS_FILE_OPEN,(uint32_t)name,MODE_WRITE|MODE_APPEND,0);
    if(fd==0xffffffffu){put("QFILE: open append FAIL\n");sc(SYS_EXIT,0,0,0);for(;;){}}
    n2=sc(SYS_FILE_WRITE,fd,(uint32_t)second,sl(second));
    sc(SYS_FILE_CLOSE,fd,0,0);
    put((n1==sl(first)&&n2==sl(second))?"QFILE: write+append PASS\n":"QFILE: write+append FAIL\n");
    sc(SYS_EXIT,0,0,0);
    for(;;){}
}
__asm__(".section .start,\"ax\"\n.global _start\n_start:\njmp _program_main\n");
