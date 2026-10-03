typedef unsigned int uint32_t; typedef signed int int32_t;
#define SYS_TIMER_GET 3u
#define SYS_EXIT 12u
#define SYS_PROCESS_HEARTBEAT 60u
#define SYS_DATA_CHANNEL 71u
static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}
/* FIX60S deterministic telemetry producer for SONARLOG lifecycle tests.
   No UART, Modbus, FAT or console dependency.  Publish at most once per
   50-Hz system tick so the independent consumer can keep up without turning
   this test into a Data Channel overrun stress test. */
__attribute__((section(".usertext"))) void program_main(void){
 uint32_t q[11],last=0xffffffffu,seq=0u,t,i;
 if(sc(SYS_DATA_CHANNEL,0u,1u,0u)==0xffffffffu)goto fail;
 for(;;){
  t=sc(SYS_TIMER_GET,0u,0u,0u);
  /* FIX60Y: FAT cyclic logging performs two bounded pwrite commits per sample.
     Pace the deterministic producer below 25 Hz so the independent logger can
     consume each sample without turning TESTLGR into an overrun benchmark. */
  if(last==0xffffffffu||(uint32_t)(t-last)>=3u){
   int32_t x=(int32_t)(100000u+(seq%2000u)*25u);
   int32_t y=(int32_t)(-50000+(int32_t)((seq%1000u)*10u));
   int32_t z=(int32_t)(200000u+(seq%500u)*20u);
   q[0]=0u;q[1]=0u;q[2]=12u;q[3]=(uint32_t)x;q[4]=(uint32_t)y;q[5]=(uint32_t)z;
   for(i=6u;i<11u;i++)q[i]=0u;
   if(sc(SYS_DATA_CHANNEL,(uint32_t)q,2u,0u)==0xffffffffu)goto fail;
   (void)sc(SYS_PROCESS_HEARTBEAT,0u,0u,0u);seq++;last=t;
  }
  for(i=0u;i<1000u;i++)__asm__ volatile("pause");
 }
fail:sc(SYS_EXIT,10u,0u,0u);for(;;){}
}
__asm__(".section .start,\"ax\"\n.global _start\n_start:\njmp _program_main\n");
