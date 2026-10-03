typedef unsigned char uint8_t; typedef unsigned short uint16_t; typedef unsigned int uint32_t; typedef signed int int32_t;
#define SYS_EXIT 12u
#define SYS_PROCESS_HEARTBEAT 60u
#define SYS_UART_TRANSPORT 70u
#define SYS_DATA_CHANNEL 71u
#define MB_OK 1u
#define MB_MORE 0u
#define MB_EXCEPTION 0xfffffff3u
#define UART_TIMEOUT_US 100000u
#define UART_RETRIES 2u
struct sonar_sample {int32_t x_mm,y_mm,z_mm;uint32_t status;};
extern uint16_t mb_crc16(const uint8_t*,uint32_t);
extern uint32_t mb_build_read_input(uint8_t,uint16_t,uint16_t,uint8_t*,uint32_t);
extern uint32_t mb_parse_read_input(const uint8_t*,uint32_t,uint8_t,uint16_t,uint16_t*,uint32_t,uint32_t*);
extern uint32_t sonar_decode_xyz(const uint16_t*,uint32_t,struct sonar_sample*);
extern uint32_t sonar_pack_channel(uint32_t,const struct sonar_sample*,uint32_t*);
static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}
static uint32_t publish_xyz(uint32_t ch,const struct sonar_sample*s){uint32_t q[11];if(sonar_pack_channel(ch,s,q)!=1u)return 0xffffffffu;return sc(SYS_DATA_CHANNEL,(uint32_t)q,2u,0u);}
#ifdef SONAR_TEST_INJECT
static uint32_t inject_test_response(void){
    uint8_t f[17];uint16_t crc;
    f[0]=1u;f[1]=4u;f[2]=12u;
    f[3]=0x00u;f[4]=0x01u;f[5]=0xe2u;f[6]=0x40u;
    f[7]=0xffu;f[8]=0xf6u;f[9]=0x04u;f[10]=0x0fu;
    f[11]=0x77u;f[12]=0x35u;f[13]=0x94u;f[14]=0x00u;
    crc=mb_crc16(f,15u);f[15]=(uint8_t)crc;f[16]=(uint8_t)(crc>>8);
    return sc(SYS_UART_TRANSPORT,(uint32_t)f,5u,17u);
}
#endif
__attribute__((section(".usertext"))) void program_main(void){uint8_t req[8],rx[32];uint16_t regs[6];struct sonar_sample s;uint32_t n,r,exc,deadline,retry,i;const uint32_t ch=0u;
#ifdef SONAR_TEST_INJECT
/* FIX60P: deterministic endurance must not depend on or emit to the host COM
   backend. Keep transport disabled; op5/op6 exercise the fixed rings only. */
if(sc(SYS_UART_TRANSPORT,0u,7u,0u)!=0u||sc(SYS_UART_TRANSPORT,0u,3u,0u)!=0u)goto fail;
#else
if(sc(SYS_UART_TRANSPORT,1u,7u,0u)!=0u)goto fail;
#endif
if(sc(SYS_DATA_CHANNEL,ch,1u,0u)==0xffffffffu)goto fail;if(mb_build_read_input(1u,0u,6u,req,sizeof(req))!=8u)goto fail;for(;;){n=0u;retry=0u;send_request:/* FIX60E: each Modbus attempt starts with an empty RX ring so a timed-out/CRC-bad tail cannot contaminate the retry. */if(sc(SYS_UART_TRANSPORT,0u,8u,0u)!=0u)goto fail;if(sc(SYS_UART_TRANSPORT,(uint32_t)req,2u,8u)!=8u)goto fail;
#ifdef SONAR_TEST_INJECT
{uint8_t sink[8];if(sc(SYS_UART_TRANSPORT,(uint32_t)sink,6u,8u)!=8u)goto fail;}
if(inject_test_response()!=17u)goto fail;
#endif
deadline=sc(SYS_UART_TRANSPORT,0u,10u,0u)+UART_TIMEOUT_US;for(;;){uint32_t now,k;if(n<sizeof(rx)){k=sc(SYS_UART_TRANSPORT,(uint32_t)(rx+n),1u,sizeof(rx)-n);if(k!=0xffffffffu)n+=k;}exc=0u;r=mb_parse_read_input(rx,n,1u,6u,regs,6u,&exc);if(r==MB_OK){if(sonar_decode_xyz(regs,6u,&s)!=1u)goto fail;/* FIX60Y: publish-count and heartbeat are observed independently by the shell.
   Commit heartbeat first so observing publication N can never race before heartbeat N. */if(sc(SYS_PROCESS_HEARTBEAT,0u,0u,0u)==0xffffffffu)goto fail;if(publish_xyz(ch,&s)==0xffffffffu)goto fail;/* FIX60M: do not invoke cooperative SYS_MT_YIELD here. The successful-response path was the only path that entered sched_yield(), which restored a saved foreground interrupt frame from inside a nested int80 return and could lose the live shell continuation. Detached MT tasks are already preempted by IRQ0 at the fixed 20-ms quantum; keep SONARDRV on that single scheduler path. */break;}if(r!=MB_MORE)break;now=sc(SYS_UART_TRANSPORT,0u,10u,0u);if((int)(now-deadline)>=0)break;for(i=0u;i<1000u;i++)__asm__ volatile("pause");}if(r!=MB_OK){if(retry++<UART_RETRIES){n=0u;goto send_request;}for(i=0u;i<50000u;i++)__asm__ volatile("pause");}}
fail:(void)sc(SYS_UART_TRANSPORT,0u,7u,0u);sc(SYS_EXIT,8u,0u,0u);for(;;){}
}
__asm__(".section .start,\"ax\"\n.global _start\n_start:\njmp _program_main\n");
