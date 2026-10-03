typedef unsigned char uint8_t; typedef unsigned int uint32_t; typedef signed int int32_t;
#define SYS_CONSOLE_WRITE 1u
#define SYS_FILE_OPEN 6u
#define SYS_FILE_CLOSE 9u
#define SYS_EXIT 12u
#define SYS_FILE_SIZE 17u
#define SYS_FILE_PREAD 72u
#define MODE_READ 1u
#define TLOG_CAPACITY 128u
#define SONARVIEW_MAX_ROWS 8u
#define TLOG_FILE_BYTES (64u+TLOG_CAPACITY*32u)
struct tlog_header {uint32_t magic,version,generation,next_sequence,write_index,valid_count,capacity,crc;};
struct tlog_record {uint32_t sequence,timestamp_us;int32_t x_mm,y_mm,z_mm;uint32_t status,generation,crc;};
extern uint32_t tlog_header_valid(const struct tlog_header*,uint32_t);
extern uint32_t tlog_record_valid(const struct tlog_record*);
extern uint32_t tlog_newer(uint32_t,uint32_t);
static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}
static uint32_t pread(uint32_t fd,void*p,uint32_t n,uint32_t off){uint32_t q[3];q[0]=fd;q[1]=(uint32_t)p;q[2]=n;return sc(SYS_FILE_PREAD,(uint32_t)q,off,0u);}
static void put(const char*s){uint32_t n=0u;while(s[n])n++;(void)sc(SYS_CONSOLE_WRITE,(uint32_t)s,n,0u);}
static char*udec(uint32_t v,char*p){char t[10];uint32_t n=0u;if(!v){*p++='0';return p;}while(v){t[n++]=(char)('0'+v%10u);v/=10u;}while(n)*p++=t[--n];return p;}
static char*sdec(int32_t v,char*p){uint32_t u;if(v<0){*p++='-';u=0u-(uint32_t)v;}else u=(uint32_t)v;return udec(u,p);}
static char*hex8(uint32_t v,char*p){static const char h[]="0123456789ABCDEF";int i;for(i=7;i>=0;i--)*p++=h[(v>>((uint32_t)i*4u))&15u];return p;}
static void summary(const struct tlog_header*h){char b[128],*p=b;p=udec(h->valid_count,p);*p++=' ';*p++='r';*p++='e';*p++='c';*p++='o';*p++='r';*p++='d';*p++='s';*p++=',';*p++=' ';*p++='n';*p++='e';*p++='x';*p++='t';*p++='=';p=udec(h->next_sequence,p);*p++=',';*p++=' ';*p++='g';*p++='e';*p++='n';*p++='=';p=udec(h->generation,p);*p++='\n';*p=0;put(b);}
static void row(const struct tlog_record*r,uint32_t ok){char b[176],*p=b;if(!ok){put("CORRUPT RECORD\n");return;}p=udec(r->sequence,p);*p++=' ';p=udec(r->timestamp_us,p);*p++=' ';p=sdec(r->x_mm,p);*p++=' ';p=sdec(r->y_mm,p);*p++=' ';p=sdec(r->z_mm,p);*p++=' ';*p++='0';*p++='x';p=hex8(r->status,p);*p++=' ';p=udec(r->generation,p);*p++='\n';*p=0;put(b);}
__attribute__((section(".usertext"))) void program_main(void){
 const char name[]="/SONAR.LOG";uint32_t fd,size,first,i,slot,shown;struct tlog_header a,b,h;struct tlog_record r;
 fd=sc(SYS_FILE_OPEN,(uint32_t)name,MODE_READ,0u);if(fd==0xffffffffu){put("SONARVIEW: /SONAR.LOG not found\n");goto fail;}
 size=sc(SYS_FILE_SIZE,(uint32_t)name,0u,0u);if(size!=TLOG_FILE_BYTES){put("SONARVIEW: invalid file size\n");goto fail_close;}
 if(pread(fd,&a,sizeof(a),0u)!=sizeof(a)||pread(fd,&b,sizeof(b),32u)!=sizeof(b)){put("SONARVIEW: header read failed\n");goto fail_close;}
 if(tlog_header_valid(&a,TLOG_CAPACITY)&&tlog_header_valid(&b,TLOG_CAPACITY))h=tlog_newer(b.generation,a.generation)?b:a;
 else if(tlog_header_valid(&a,TLOG_CAPACITY))h=a;else if(tlog_header_valid(&b,TLOG_CAPACITY))h=b;else{put("SONARVIEW: no valid header\n");goto fail_close;}
 put("SONARVIEW: seq time_us x_mm y_mm z_mm status generation\n");summary(&h);
 shown=h.valid_count; if(shown>SONARVIEW_MAX_ROWS)shown=SONARVIEW_MAX_ROWS;
 first=(h.write_index+TLOG_CAPACITY-shown)%TLOG_CAPACITY;
 for(i=0u;i<shown;i++){slot=(first+i)%TLOG_CAPACITY;if(pread(fd,&r,sizeof(r),64u+slot*32u)!=sizeof(r)){put("SONARVIEW: record read failed\n");goto fail_close;}row(&r,tlog_record_valid(&r));}
 (void)sc(SYS_FILE_CLOSE,fd,0u,0u);sc(SYS_EXIT,0u,0u,0u);for(;;){}
fail_close:(void)sc(SYS_FILE_CLOSE,fd,0u,0u);fail:sc(SYS_EXIT,11u,0u,0u);for(;;){}
}
__asm__(".section .start,\"ax\"\n.global _start\n_start:\njmp _program_main\n");
