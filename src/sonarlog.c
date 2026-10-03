typedef unsigned char uint8_t; typedef unsigned int uint32_t; typedef signed int int32_t;
#define SYS_FILE_OPEN 6u
#define SYS_FILE_WRITE 8u
#define SYS_FILE_CLOSE 9u
#define SYS_FILE_SIZE 17u
#define SYS_EXIT 12u
#define SYS_PROCESS_HEARTBEAT 60u
#define SYS_DATA_CHANNEL 71u
#define SYS_FILE_PREAD 72u
#define SYS_FILE_PWRITE 73u
#define MODE_READ 1u
#define MODE_WRITE 2u
#define MODE_CREATE 4u
#define MODE_TRUNC 8u
#define TLOG_CAPACITY 128u
#define TLOG_HEADER_BYTES 32u
#define TLOG_RECORD_BYTES 32u
#define TLOG_FILE_BYTES (64u+TLOG_CAPACITY*TLOG_RECORD_BYTES)
#define DATA_READ_SAMPLE 1u
#define DATA_READ_OVERRUN 2u
#define DATA_READ_GENERATION 3u
struct tlog_header {uint32_t magic,version,generation,next_sequence,write_index,valid_count,capacity,crc;};
struct tlog_record {uint32_t sequence,timestamp_us;int32_t x_mm,y_mm,z_mm;uint32_t status,generation,crc;};
extern void tlog_header_make(struct tlog_header*,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern uint32_t tlog_header_valid(const struct tlog_header*,uint32_t);
extern void tlog_record_make(struct tlog_record*,uint32_t,uint32_t,int32_t,int32_t,int32_t,uint32_t,uint32_t);
extern uint32_t tlog_newer(uint32_t,uint32_t);
static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}
static uint32_t pread(uint32_t fd,void*p,uint32_t n,uint32_t off){uint32_t q[3];q[0]=fd;q[1]=(uint32_t)p;q[2]=n;return sc(SYS_FILE_PREAD,(uint32_t)q,off,0u);}
static uint32_t pwrite(uint32_t fd,const void*p,uint32_t n,uint32_t off){uint32_t q[3];q[0]=fd;q[1]=(uint32_t)p;q[2]=n;return sc(SYS_FILE_PWRITE,(uint32_t)q,off,0u);}
static void zero(uint8_t*p,uint32_t n){while(n--)*p++=0u;}
static void idle_delay(void){uint32_t i;for(i=0u;i<2000u;i++)__asm__ volatile("pause");}
/* FIX60R: create/recreate the bounded file as one explicit operation.  A stale
   short file or two invalid headers must not permanently kill an autonomous
   logger after an interrupted first initialization. */
static uint32_t init_file(const char*name,uint32_t*fd_out,struct tlog_header*h){
 uint32_t fd,rh;uint8_t z[64];
 fd=sc(SYS_FILE_OPEN,(uint32_t)name,MODE_READ|MODE_WRITE|MODE_CREATE|MODE_TRUNC,0u);if(fd==0xffffffffu)return 0u;
 zero(z,sizeof(z));for(rh=0u;rh<TLOG_FILE_BYTES;rh+=sizeof(z)){uint32_t n=TLOG_FILE_BYTES-rh;if(n>sizeof(z))n=sizeof(z);if(sc(SYS_FILE_WRITE,fd,(uint32_t)z,n)!=n){(void)sc(SYS_FILE_CLOSE,fd,0u,0u);return 0u;}}
 tlog_header_make(h,1u,1u,0u,0u,TLOG_CAPACITY);
 if(pwrite(fd,h,sizeof(*h),0u)!=sizeof(*h)||pwrite(fd,h,sizeof(*h),32u)!=sizeof(*h)){(void)sc(SYS_FILE_CLOSE,fd,0u,0u);return 0u;}
 *fd_out=fd;return 1u;
}
/* FIX60ZED: autonomous SONARLOG must not die on a transient file error under
   sustained RT/MT load. Reopen and revalidate the bounded cyclic file; if it
   is corrupt or short, recreate it transactionally. */
static uint32_t open_log(const char*name,uint32_t*fd_out,struct tlog_header*h,uint32_t*active_out){
 uint32_t fd,size,active=0u;struct tlog_header a,b;
 fd=sc(SYS_FILE_OPEN,(uint32_t)name,MODE_READ|MODE_WRITE|MODE_CREATE,0u);if(fd==0xffffffffu)return 0u;
 size=sc(SYS_FILE_SIZE,(uint32_t)name,0u,0u);
 if(size!=TLOG_FILE_BYTES){(void)sc(SYS_FILE_CLOSE,fd,0u,0u);fd=0xffffffffu;if(!init_file(name,&fd,h))return 0u;active=1u;}
 else{
  if(pread(fd,&a,sizeof(a),0u)!=sizeof(a)||pread(fd,&b,sizeof(b),32u)!=sizeof(b)){(void)sc(SYS_FILE_CLOSE,fd,0u,0u);return 0u;}
  if(tlog_header_valid(&a,TLOG_CAPACITY)&&tlog_header_valid(&b,TLOG_CAPACITY))active=tlog_newer(b.generation,a.generation)?1u:0u;
  else if(tlog_header_valid(&a,TLOG_CAPACITY))active=0u;
  else if(tlog_header_valid(&b,TLOG_CAPACITY))active=1u;
  else{(void)sc(SYS_FILE_CLOSE,fd,0u,0u);fd=0xffffffffu;if(!init_file(name,&fd,h))return 0u;active=1u;*fd_out=fd;*active_out=active;return 1u;}
  *h=active?b:a;
 }
 *fd_out=fd;*active_out=active;return 1u;
}
__attribute__((section(".usertext"))) void program_main(void){
 /* The telemetry service owns a system log, not a shell-cwd-relative file. */
 const char name[]="/SONAR.LOG";uint32_t fd=0xffffffffu,ch[14],reader=0xffffffffu,active=0u;struct tlog_header h;struct tlog_record rec;
 /* File availability is a recoverable service condition. Keep the process
    alive and heartbeat while storage is temporarily unavailable. */
 while(!open_log(name,&fd,&h,&active)){(void)sc(SYS_PROCESS_HEARTBEAT,0u,0u,0u);idle_delay();}
 /* FIX60R: Data Channel producer and consumer are independent services.  The
    consumer waits for channel generation instead of exiting if it happens to
    run before/rebetween producer initialization. */
 for(;;){
  uint32_t rr;
  if(reader==0xffffffffu){reader=sc(SYS_DATA_CHANNEL,0u,3u,0u);if(reader==0xffffffffu){idle_delay();continue;}(void)sc(SYS_PROCESS_HEARTBEAT,0u,0u,0u);}
  rr=sc(SYS_DATA_CHANNEL,(uint32_t)ch,4u,reader);
  if(rr==DATA_READ_SAMPLE&&ch[5]>=12u){
   uint8_t*p=(uint8_t*)&ch[6];int32_t x=*(int32_t*)(p+0),y=*(int32_t*)(p+4),zv=*(int32_t*)(p+8);uint32_t slot=h.write_index,hoff;
   tlog_record_make(&rec,h.next_sequence,ch[3],x,y,zv,ch[4],ch[1]);if(pwrite(fd,&rec,sizeof(rec),64u+slot*TLOG_RECORD_BYTES)!=sizeof(rec)){(void)sc(SYS_FILE_CLOSE,fd,0u,0u);fd=0xffffffffu;while(!open_log(name,&fd,&h,&active)){(void)sc(SYS_PROCESS_HEARTBEAT,0u,0u,0u);idle_delay();}continue;}
   h.generation++;h.next_sequence++;h.write_index=(slot+1u)%TLOG_CAPACITY;if(h.valid_count<TLOG_CAPACITY)h.valid_count++;
   tlog_header_make(&h,h.generation,h.next_sequence,h.write_index,h.valid_count,TLOG_CAPACITY);active^=1u;hoff=active?32u:0u;if(pwrite(fd,&h,sizeof(h),hoff)!=sizeof(h)){(void)sc(SYS_FILE_CLOSE,fd,0u,0u);fd=0xffffffffu;while(!open_log(name,&fd,&h,&active)){(void)sc(SYS_PROCESS_HEARTBEAT,0u,0u,0u);idle_delay();}continue;}
   (void)sc(SYS_PROCESS_HEARTBEAT,0u,0u,0u);
  }else if(rr==0xffffffffu){/* reader was invalidated/reset: reacquire it */(void)sc(SYS_DATA_CHANNEL,reader,5u,0u);reader=0xffffffffu;idle_delay();}
  else if(rr==DATA_READ_OVERRUN||rr==DATA_READ_GENERATION){/* kernel adjusted cursor/generation; retry */}
  else idle_delay();
 }
}
__asm__(".section .start,\"ax\"\n.global _start\n_start:\njmp _program_main\n");
