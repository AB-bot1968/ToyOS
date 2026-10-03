typedef unsigned char uint8_t; typedef unsigned short uint16_t; typedef unsigned int uint32_t;
#define SYS_EXIT 12u
#define SYS_PROCESS_HEARTBEAT 60u
#define SYS_UART_TRANSPORT 70u
#define UART_TIMEOUT_US 100000u
static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}
static uint16_t crc16(const uint8_t*p,uint32_t n){uint16_t c=0xffffu;uint32_t i,j;for(i=0u;i<n;i++){c^=p[i];for(j=0u;j<8u;j++)c=(c&1u)?(uint16_t)((c>>1)^0xa001u):(uint16_t)(c>>1);}return c;}
/* FIX60P physical isolation probe. It deliberately performs no Modbus response
   parsing, XYZ decode, Data Channel publication, FAT I/O or console output.
   A heartbeat means only that at least 17 physical RX bytes arrived after a
   valid 8-byte F04 request. */
__attribute__((section(".usertext"))) void program_main(void){
    uint8_t req[8],rx[32];uint16_t c;uint32_t n,k,deadline,i;
    req[0]=1u;req[1]=4u;req[2]=0u;req[3]=0u;req[4]=0u;req[5]=6u;c=crc16(req,6u);req[6]=(uint8_t)c;req[7]=(uint8_t)(c>>8);
#ifdef UART_RX_TEST_INJECT
    /* FIX60Q: automatic TST must never enter the host/QEMU physical serial
       backend.  Build UARTRXT from this same source with transport disabled;
       op6 consumes the request and op5 injects a complete 17-byte RX frame. */
    if(sc(SYS_UART_TRANSPORT,0u,7u,0u)!=0u||sc(SYS_UART_TRANSPORT,0u,3u,0u)!=0u)goto fail;
#else
    if(sc(SYS_UART_TRANSPORT,1u,7u,0u)!=0u)goto fail;
#endif
    for(;;){
        n=0u;if(sc(SYS_UART_TRANSPORT,0u,8u,0u)!=0u)goto fail;
        if(sc(SYS_UART_TRANSPORT,(uint32_t)req,2u,8u)!=8u)goto fail;
#ifdef UART_RX_TEST_INJECT
        {uint8_t sink[8],fake[17];uint32_t j;
         if(sc(SYS_UART_TRANSPORT,(uint32_t)sink,6u,8u)!=8u)goto fail;
         for(j=0u;j<17u;j++)fake[j]=(uint8_t)(0xa0u+j);
         if(sc(SYS_UART_TRANSPORT,(uint32_t)fake,5u,17u)!=17u)goto fail;}
#endif
        deadline=sc(SYS_UART_TRANSPORT,0u,10u,0u)+UART_TIMEOUT_US;
        while(n<17u){
            k=sc(SYS_UART_TRANSPORT,(uint32_t)(rx+n),1u,(uint32_t)(sizeof(rx)-n));if(k==0xffffffffu)goto fail;n+=k;
            if(n>=17u)break;
            if((int)(sc(SYS_UART_TRANSPORT,0u,10u,0u)-deadline)>=0)break;
            for(i=0u;i<1000u;i++)__asm__ volatile("pause");
        }
        if(n>=17u)(void)sc(SYS_PROCESS_HEARTBEAT,0u,0u,0u);
        else for(i=0u;i<50000u;i++)__asm__ volatile("pause");
    }
fail:(void)sc(SYS_UART_TRANSPORT,0u,7u,0u);sc(SYS_EXIT,8u,0u,0u);for(;;){}
}
__asm__(".section .start,\"ax\"\n.global _start\n_start:\njmp _program_main\n");
