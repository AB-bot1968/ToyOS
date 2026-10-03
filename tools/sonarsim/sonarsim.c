/* FIX59 Windows 7 Modbus RTU sonar simulator. Build with W64DevKit/MinGW. */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#endif

enum fault_mode { FM_NORMAL, FM_NO_RESPONSE, FM_BAD_CRC, FM_DELAY, FM_PARTIAL, FM_EXCEPTION };
struct sim_cfg { const char *port,*tcp_host; unsigned tcp_port,baud,slave,delay_ms; enum fault_mode mode; int fixed; int32_t x,y,z; };
static uint16_t crc16(const uint8_t*p,size_t n){uint16_t c=0xffff;size_t i;unsigned j;for(i=0;i<n;i++){c^=p[i];for(j=0;j<8;j++)c=(c&1)?(uint16_t)((c>>1)^0xa001):(uint16_t)(c>>1);}return c;}
static void put_i32_regs(uint8_t *p,int32_t v){uint32_t u=(uint32_t)v;uint16_t hi=(uint16_t)(u>>16),lo=(uint16_t)u;p[0]=(uint8_t)(hi>>8);p[1]=(uint8_t)hi;p[2]=(uint8_t)(lo>>8);p[3]=(uint8_t)lo;}
static size_t make_response(const struct sim_cfg*c,const uint8_t req[8],uint8_t*out,int32_t x,int32_t y,int32_t z){uint16_t got,calc,cc;if(req[0]!=(uint8_t)c->slave||req[1]!=4)return 0;calc=crc16(req,6);got=(uint16_t)req[6]|((uint16_t)req[7]<<8);if(calc!=got)return 0;if(req[2]||req[3]||req[4]||req[5]!=6)return 0;if(c->mode==FM_EXCEPTION){out[0]=(uint8_t)c->slave;out[1]=0x84;out[2]=4;cc=crc16(out,3);out[3]=(uint8_t)cc;out[4]=(uint8_t)(cc>>8);return 5;}out[0]=(uint8_t)c->slave;out[1]=4;out[2]=12;put_i32_regs(out+3,x);put_i32_regs(out+7,y);put_i32_regs(out+11,z);cc=crc16(out,15);out[15]=(uint8_t)cc;out[16]=(uint8_t)(cc>>8);if(c->mode==FM_BAD_CRC)out[15]^=0x5a;return 17;}
static int parse_mode(const char*s,enum fault_mode*m){if(!strcmp(s,"normal"))*m=FM_NORMAL;else if(!strcmp(s,"no-response"))*m=FM_NO_RESPONSE;else if(!strcmp(s,"bad-crc"))*m=FM_BAD_CRC;else if(!strcmp(s,"delay"))*m=FM_DELAY;else if(!strcmp(s,"partial"))*m=FM_PARTIAL;else if(!strcmp(s,"exception"))*m=FM_EXCEPTION;else return 0;return 1;}
static void usage(void){puts("SONARSIM (--port COM2 | --tcp HOST PORT) [--baud 115200] [--slave 1] [--mode normal|no-response|bad-crc|delay|partial|exception] [--delay-ms 250] [--xyz X Y Z]");}
static int args(int ac,char**av,struct sim_cfg*c){int i;memset(c,0,sizeof(*c));c->port="COM2";c->baud=115200;c->slave=1;c->delay_ms=250;c->mode=FM_NORMAL;for(i=1;i<ac;i++){if(!strcmp(av[i],"--port")&&i+1<ac){c->port=av[++i];c->tcp_host=0;c->tcp_port=0;}else if(!strcmp(av[i],"--tcp")&&i+2<ac){c->tcp_host=av[++i];c->tcp_port=(unsigned)strtoul(av[++i],0,0);}else if(!strcmp(av[i],"--baud")&&i+1<ac)c->baud=(unsigned)strtoul(av[++i],0,0);else if(!strcmp(av[i],"--slave")&&i+1<ac)c->slave=(unsigned)strtoul(av[++i],0,0);else if(!strcmp(av[i],"--delay-ms")&&i+1<ac)c->delay_ms=(unsigned)strtoul(av[++i],0,0);else if(!strcmp(av[i],"--mode")&&i+1<ac){if(!parse_mode(av[++i],&c->mode))return 0;}else if(!strcmp(av[i],"--xyz")&&i+3<ac){c->fixed=1;c->x=(int32_t)strtol(av[++i],0,0);c->y=(int32_t)strtol(av[++i],0,0);c->z=(int32_t)strtol(av[++i],0,0);}else if(!strcmp(av[i],"--selftest")){}else return 0;}return c->slave>0&&c->slave<248&&c->baud>0&&(!c->tcp_host||(c->tcp_port>0&&c->tcp_port<65536u));}
static int selftest(void){struct sim_cfg c;uint8_t q[8]={1,4,0,0,0,6,0,0},r[32];uint16_t k;size_t n;c.port="";c.tcp_host=0;c.tcp_port=0;c.baud=115200;c.slave=1;c.delay_ms=1;c.mode=FM_NORMAL;c.fixed=1;c.x=123456;c.y=-654321;c.z=2000000000;k=crc16(q,6);q[6]=(uint8_t)k;q[7]=(uint8_t)(k>>8);if(crc16((const uint8_t*)"123456789",9)!=0x4b37)return 1;n=make_response(&c,q,r,c.x,c.y,c.z);if(n!=17||r[2]!=12||crc16(r,15)!=((uint16_t)r[15]|((uint16_t)r[16]<<8)))return 2;if(memcmp(r+3,(uint8_t[]){0,1,0xe2,0x40,0xff,0xf6,0x04,0x0f,0x77,0x35,0x94,0x00},12))return 3;c.mode=FM_BAD_CRC;n=make_response(&c,q,r,c.x,c.y,c.z);if(n!=17||crc16(r,15)==((uint16_t)r[15]|((uint16_t)r[16]<<8)))return 4;c.mode=FM_EXCEPTION;n=make_response(&c,q,r,c.x,c.y,c.z);if(n!=5||r[1]!=0x84||r[2]!=4)return 5;q[7]^=1;if(make_response(&c,q,r,c.x,c.y,c.z)!=0)return 6;puts("SONARSIM SELFTEST: PASS");return 0;}
#ifdef _WIN32
static HANDLE open_serial(const struct sim_cfg*c){
    char name[64];DCB d={0};COMMTIMEOUTS t={0};HANDLE h;
    snprintf(name,sizeof(name),"\\\\.\\%s",c->port);
    h=CreateFileA(name,GENERIC_READ|GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,0);
    if(h==INVALID_HANDLE_VALUE)return h;
    d.DCBlength=sizeof(d);if(!GetCommState(h,&d)){CloseHandle(h);return INVALID_HANDLE_VALUE;}
    d.BaudRate=c->baud;d.ByteSize=8;d.Parity=NOPARITY;d.StopBits=ONESTOPBIT;d.fBinary=TRUE;
    d.fOutxCtsFlow=FALSE;d.fOutxDsrFlow=FALSE;d.fDsrSensitivity=FALSE;d.fOutX=FALSE;d.fInX=FALSE;
    d.fDtrControl=DTR_CONTROL_ENABLE;d.fRtsControl=RTS_CONTROL_ENABLE;d.fAbortOnError=FALSE;
    if(!SetCommState(h,&d)){CloseHandle(h);return INVALID_HANDLE_VALUE;}
    /* Overlapped waits below are the authoritative bounds. Keep driver timeouts
       non-blocking so com0com cannot trap the simulator inside synchronous I/O. */
    t.ReadIntervalTimeout=MAXDWORD;t.ReadTotalTimeoutConstant=0;t.ReadTotalTimeoutMultiplier=0;
    t.WriteTotalTimeoutConstant=0;t.WriteTotalTimeoutMultiplier=0;SetCommTimeouts(h,&t);
    PurgeComm(h,PURGE_RXCLEAR|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_TXABORT);return h;
}
/* 1=completed, 0=bounded timeout, -1=I/O error. */
static int ov_read(HANDLE h,uint8_t*buf,DWORD cap,DWORD*got,DWORD timeout_ms){
    OVERLAPPED ov;DWORD e,w;memset(&ov,0,sizeof(ov));*got=0;ov.hEvent=CreateEventA(0,TRUE,FALSE,0);if(!ov.hEvent)return -1;
    if(ReadFile(h,buf,cap,got,&ov)){CloseHandle(ov.hEvent);return 1;}
    e=GetLastError();if(e!=ERROR_IO_PENDING){CloseHandle(ov.hEvent);return -1;}
    w=WaitForSingleObject(ov.hEvent,timeout_ms);
    if(w==WAIT_TIMEOUT){CancelIo(h);/* OVERLAPPED lives on this stack: wait until cancellation is complete before returning. */WaitForSingleObject(ov.hEvent,INFINITE);CloseHandle(ov.hEvent);*got=0;return 0;}
    if(w!=WAIT_OBJECT_0||!GetOverlappedResult(h,&ov,got,FALSE)){CloseHandle(ov.hEvent);return -1;}
    CloseHandle(ov.hEvent);return 1;
}
static int ov_write_all(HANDLE h,const uint8_t*buf,DWORD n,DWORD timeout_ms){
    DWORD off=0;
    while(off<n){OVERLAPPED ov;DWORD e,w,done=0;memset(&ov,0,sizeof(ov));ov.hEvent=CreateEventA(0,TRUE,FALSE,0);if(!ov.hEvent)return -1;
        if(!WriteFile(h,buf+off,n-off,&done,&ov)){e=GetLastError();if(e!=ERROR_IO_PENDING){CloseHandle(ov.hEvent);return -1;}w=WaitForSingleObject(ov.hEvent,timeout_ms);if(w==WAIT_TIMEOUT){CancelIo(h);/* Do not free a live OVERLAPPED. */WaitForSingleObject(ov.hEvent,INFINITE);CloseHandle(ov.hEvent);return 0;}if(w!=WAIT_OBJECT_0||!GetOverlappedResult(h,&ov,&done,FALSE)){CloseHandle(ov.hEvent);return -1;}}
        CloseHandle(ov.hEvent);if(!done)return 0;off+=done;
    }
    return 1;
}
static int run(struct sim_cfg*c){
    HANDLE h=open_serial(c);uint8_t q[8],r[32];DWORD got;unsigned qn=0;size_t have=0;int32_t x=c->x,y=c->y,z=c->z;
    if(h==INVALID_HANDLE_VALUE){fprintf(stderr,"SONARSIM: cannot open %s error=%lu\n",c->port,(unsigned long)GetLastError());return 2;}
    printf("SONARSIM: %s %u 8N1 slave=%u mode=%d READY bounded-overlapped-io\n",c->port,c->baud,c->slave,(int)c->mode);fflush(stdout);
    for(;;){size_t n;int io;
        while(have<8){io=ov_read(h,q+have,(DWORD)(8-have),&got,100);if(io<0){fprintf(stderr,"SONARSIM: RX error=%lu\n",(unsigned long)GetLastError());CloseHandle(h);return 3;}if(io==0)continue;if(got)have+=got;}
        if(!c->fixed){x=(int32_t)(100000+(int32_t)((qn+1u)%2000)*25);y=(int32_t)(-50000+(int32_t)((qn+1u)%1000)*10);z=(int32_t)(200000+(int32_t)((qn+1u)%500)*20);}
        n=make_response(c,q,r,x,y,z);if(!n){memmove(q,q+1,7);have=7;continue;}have=0;qn++;
        if(c->mode==FM_NO_RESPONSE){printf("request=%u xyz=%ld,%ld,%ld response=SUPPRESSED\n",qn,(long)x,(long)y,(long)z);fflush(stdout);continue;}
        if(c->mode==FM_DELAY)Sleep(c->delay_ms);
        if(c->mode==FM_PARTIAL&&n>3){DWORD a=(DWORD)(n/2);io=ov_write_all(h,r,a,500);if(io==1){Sleep(c->delay_ms);io=ov_write_all(h,r+a,(DWORD)(n-a),500);}}
        else io=ov_write_all(h,r,(DWORD)n,500);
        if(io==0){printf("request=%u xyz=%ld,%ld,%ld response=%u tx=TIMEOUT\n",qn,(long)x,(long)y,(long)z,(unsigned)n);fflush(stdout);continue;}
        if(io<0){fprintf(stderr,"SONARSIM: TX error request=%u error=%lu\n",qn,(unsigned long)GetLastError());CloseHandle(h);return 4;}
        printf("request=%u xyz=%ld,%ld,%ld response=%u tx=%u\n",qn,(long)x,(long)y,(long)z,(unsigned)n,(unsigned)n);fflush(stdout);
    }
}

/* FIX60Q: QEMU's native Windows COM chardev has upstream deadlock reports
   under inbound serial traffic.  TCP socket mode bypasses char-win.c while
   preserving the guest-visible 16550 byte stream.  All waits remain bounded. */
static int sock_wait(SOCKET s,int write_ready,DWORD timeout_ms){
    fd_set f;struct timeval tv;int r;FD_ZERO(&f);FD_SET(s,&f);tv.tv_sec=(long)(timeout_ms/1000u);tv.tv_usec=(long)((timeout_ms%1000u)*1000u);
    r=select(0,write_ready?0:&f,write_ready?&f:0,0,&tv);return r>0?1:(r==0?0:-1);
}
static SOCKET open_tcp(const struct sim_cfg*c){
    WSADATA wd;struct addrinfo hints,*ai=0,*it;char port[16];SOCKET s=INVALID_SOCKET;u_long nb=1;
    if(WSAStartup(MAKEWORD(2,2),&wd)!=0)return INVALID_SOCKET;
    memset(&hints,0,sizeof(hints));hints.ai_family=AF_UNSPEC;hints.ai_socktype=SOCK_STREAM;hints.ai_protocol=IPPROTO_TCP;snprintf(port,sizeof(port),"%u",c->tcp_port);
    if(getaddrinfo(c->tcp_host,port,&hints,&ai)!=0){WSACleanup();return INVALID_SOCKET;}
    for(it=ai;it;it=it->ai_next){int e=0;int elen=(int)sizeof(e);s=socket(it->ai_family,it->ai_socktype,it->ai_protocol);if(s==INVALID_SOCKET)continue;ioctlsocket(s,FIONBIO,&nb);
        if(connect(s,it->ai_addr,(int)it->ai_addrlen)==0)break;
        if(WSAGetLastError()==WSAEWOULDBLOCK||WSAGetLastError()==WSAEINPROGRESS){if(sock_wait(s,1,2000)==1&&getsockopt(s,SOL_SOCKET,SO_ERROR,(char*)&e,&elen)==0&&e==0)break;}
        closesocket(s);s=INVALID_SOCKET;
    }
    freeaddrinfo(ai);if(s==INVALID_SOCKET)WSACleanup();return s;
}
static int tcp_read_some(SOCKET s,uint8_t*buf,int cap,int*got,DWORD timeout_ms){int r,w;*got=0;w=sock_wait(s,0,timeout_ms);if(w<=0)return w;r=recv(s,(char*)buf,cap,0);if(r>0){*got=r;return 1;}if(r==0)return -1;if(WSAGetLastError()==WSAEWOULDBLOCK)return 0;return -1;}
static int tcp_write_all(SOCKET s,const uint8_t*buf,int n,DWORD timeout_ms){int off=0;while(off<n){int w=sock_wait(s,1,timeout_ms),r;if(w<=0)return w;r=send(s,(const char*)buf+off,n-off,0);if(r>0){off+=r;continue;}if(r<0&&WSAGetLastError()==WSAEWOULDBLOCK)continue;return -1;}return 1;}
static int run_tcp(struct sim_cfg*c){
    SOCKET s=open_tcp(c);uint8_t q[8],r[32];unsigned qn=0;size_t have=0;int32_t x=c->x,y=c->y,z=c->z;
    if(s==INVALID_SOCKET){fprintf(stderr,"SONARSIM: cannot connect TCP %s:%u error=%d\n",c->tcp_host,c->tcp_port,WSAGetLastError());return 2;}
    printf("SONARSIM: TCP %s:%u slave=%u mode=%d READY bounded-socket-io\n",c->tcp_host,c->tcp_port,c->slave,(int)c->mode);fflush(stdout);
    for(;;){size_t n;int io,got=0;
        while(have<8){io=tcp_read_some(s,q+have,(int)(8-have),&got,100);if(io<0){fprintf(stderr,"SONARSIM: TCP RX closed/error=%d\n",WSAGetLastError());closesocket(s);WSACleanup();return 3;}if(io==0)continue;if(got)have+=(size_t)got;}
        if(!c->fixed){x=(int32_t)(100000+(int32_t)((qn+1u)%2000)*25);y=(int32_t)(-50000+(int32_t)((qn+1u)%1000)*10);z=(int32_t)(200000+(int32_t)((qn+1u)%500)*20);}
        n=make_response(c,q,r,x,y,z);if(!n){memmove(q,q+1,7);have=7;continue;}have=0;qn++;
        if(c->mode==FM_NO_RESPONSE){printf("request=%u xyz=%ld,%ld,%ld response=SUPPRESSED\n",qn,(long)x,(long)y,(long)z);fflush(stdout);continue;}
        if(c->mode==FM_DELAY)Sleep(c->delay_ms);
        if(c->mode==FM_PARTIAL&&n>3){int a=(int)(n/2);io=tcp_write_all(s,r,a,500);if(io==1){Sleep(c->delay_ms);io=tcp_write_all(s,r+a,(int)(n-a),500);}}
        else io=tcp_write_all(s,r,(int)n,500);
        if(io==0){printf("request=%u xyz=%ld,%ld,%ld response=%u tx=TIMEOUT\n",qn,(long)x,(long)y,(long)z,(unsigned)n);fflush(stdout);continue;}
        if(io<0){fprintf(stderr,"SONARSIM: TCP TX error request=%u error=%d\n",qn,WSAGetLastError());closesocket(s);WSACleanup();return 4;}
        printf("request=%u xyz=%ld,%ld,%ld response=%u tx=%u\n",qn,(long)x,(long)y,(long)z,(unsigned)n,(unsigned)n);fflush(stdout);
    }
}
#endif
int main(int ac,char**av){struct sim_cfg c;int i;for(i=1;i<ac;i++)if(!strcmp(av[i],"--selftest"))return selftest();if(!args(ac,av,&c)){usage();return 1;}
#ifdef _WIN32
if(c.tcp_host)return run_tcp(&c);
return run(&c);
#else
fprintf(stderr,"SONARSIM runtime requires Windows; use --selftest on host.\n");return 2;
#endif
}
