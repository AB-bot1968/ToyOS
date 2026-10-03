/* QSIZE.EXE: verifies SYS_FILE_SIZE from Ring 3. It queries README.TXT and
   returns the exact byte count reported by the FAT16 directory entry. */
typedef unsigned int uint32_t;
#define SYS_CONSOLE_WRITE 1u
#define SYS_FILE_SIZE 17u
#define SYS_EXIT 12u
static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}
static uint32_t sl(const char*s){uint32_t n=0;while(s[n])n++;return n;}
static void put(const char*s){sc(SYS_CONSOLE_WRITE,(uint32_t)s,sl(s),0);}
static void dec(uint32_t v,char*b){static const uint32_t p[10]={1000000000u,100000000u,10000000u,1000000u,100000u,10000u,1000u,100u,10u,1u};uint32_t d,st=0,i;for(i=0;i<10;i++){d=0;while(v>=p[i]){v-=p[i];d++;}if(d||st||i==9){*b++=(char)('0'+d);st=1;}}*b=0;}
__attribute__((section(".usertext"))) void program_main(void){
    static const char name[]="README.TXT";
    uint32_t r; char b[12];
    put("QSIZE: SYS_FILE_SIZE README.TXT = ");
    r=sc(SYS_FILE_SIZE,(uint32_t)name,0,0);
    if(r==0xffffffffu){put("FAIL\n");sc(SYS_EXIT,0,0,0);for(;;){}}
    dec(r,b);put(b);put(" bytes\n");
    r=sc(SYS_FILE_SIZE,(uint32_t)"NOFILE.TXT",0,0);
    if(r!=0xffffffffu){put("QSIZE: missing-file guard FAIL\n");sc(SYS_EXIT,0,0,0);for(;;){}}
    put("QSIZE: missing-file guard PASS\nQSIZE: PASS\n");
    sc(SYS_EXIT,0,0,0);for(;;){}
}
__asm__(".section .start,\"ax\"\n.global _start\n_start:\njmp _program_main\n");
