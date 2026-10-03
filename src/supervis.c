typedef unsigned int uint32_t;
#define SYS_TIMER_GET 3u
#define SYS_EXIT 12u
#define SYS_MT_DATA 53u
#define SYS_PROCESS_SPAWN 57u
#define SYS_PROCESS_RESULT 59u
#define SYS_PROCESS_HEARTBEAT 60u
#define SYS_PROCESS_STOP_PID 61u
#define SYS_SAFE_MODE 63u
#define SYS_RECOVERY_EVENT 67u
#define SYS_RECOVERY_MANAGER 68u
#define EVENT_REC_RESTART 103u
#define EVENT_REC_WATCHDOG 104u
#define EVENT_REC_BUDGET 105u
#define SAFE_NORMAL 0u
#define SAFE_DEGRADED 1u
#define SAFE_SAFE 2u
#define SUP_REASON_FAULT_DEGRADED 4501u
#define SUP_REASON_WATCHDOG_DEGRADED 4502u
#define SUP_REASON_FAULT_SAFE 4591u
#define SUP_REASON_WATCHDOG_SAFE 4592u
#define SPAWN_ARGC_MAX 8u
#define SPAWN_ARG_SIZE 32u
#define NAME_SIZE 13u
#define WATCHDOG_TIMEOUT_TICKS 100u /* 2 s at legacy 50 Hz clock */
struct process_spawn_request {char name[NAME_SIZE];uint32_t argc;char argv[SPAWN_ARGC_MAX][SPAWN_ARG_SIZE];};
struct mt_data_request {uint32_t op,mt_id,buffer,length,seq,session;};
static uint32_t sc(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}
static uint32_t sl(const char*s){uint32_t n=0;while(s[n])n++;return n;}
static void recovery_event(uint32_t type,uint32_t pid,uint32_t reason,uint32_t status){uint32_t q[4];q[0]=type;q[1]=pid;q[2]=reason;q[3]=status;(void)sc(SYS_RECOVERY_EVENT,(uint32_t)q,0,0);}
static uint32_t recovery_reset(void){return sc(SYS_RECOVERY_MANAGER,0u,1u,0u);}
static uint32_t recovery_detect(uint32_t pid,uint32_t kind,uint32_t detail){uint32_t q[3];q[0]=pid;q[1]=kind;q[2]=detail;return sc(SYS_RECOVERY_MANAGER,(uint32_t)q,2u,0u);}
static uint32_t recovery_stopped(uint32_t pid){return sc(SYS_RECOVERY_MANAGER,pid,3u,0u);}
static uint32_t recovery_spawned(uint32_t oldpid,uint32_t newpid){uint32_t q[2];q[0]=oldpid;q[1]=newpid;return sc(SYS_RECOVERY_MANAGER,(uint32_t)q,4u,0u);}
static uint32_t recovery_verified(uint32_t pid){return sc(SYS_RECOVERY_MANAGER,pid,5u,0u);}
static uint32_t recovery_failed(uint32_t pid,uint32_t result,uint32_t detail){uint32_t q[3];q[0]=pid;q[1]=result;q[2]=detail;return sc(SYS_RECOVERY_MANAGER,(uint32_t)q,6u,0u);}
#define RECOVERY_RESULT_STOP_FAILED 2u
#define RECOVERY_RESULT_SPAWN_FAILED 3u
#define RECOVERY_RESULT_VERIFY_TIMEOUT 4u
#define RECOVERY_RESULT_PROCESS_FAILED 5u
static uint32_t add(char*d,uint32_t n,uint32_t cap,const char*s){uint32_t i=0;while(s[i]&&n+1u<cap)d[n++]=s[i++];d[n]=0;return n;}
static uint32_t addn(char*d,uint32_t n,uint32_t cap,uint32_t v){char t[12];uint32_t k=0,i;if(!v)t[k++]='0';else while(v&&k<sizeof(t)){t[k++]=(char)('0'+v%10u);v/=10u;}for(i=0;i<k&&n+1u<cap;i++)d[n++]=t[k-1u-i];d[n]=0;return n;}
static void publish(const char*state,uint32_t pid,uint32_t restarts,uint32_t detail){char b[180];uint32_t n=0;struct mt_data_request q;b[0]=0;n=add(b,n,sizeof(b),"SUP:STATE=");n=add(b,n,sizeof(b),state);n=add(b,n,sizeof(b),";PID=");n=addn(b,n,sizeof(b),pid);n=add(b,n,sizeof(b),";RESTARTS=");n=addn(b,n,sizeof(b),restarts);n=add(b,n,sizeof(b),";DETAIL=");n=addn(b,n,sizeof(b),detail);n=add(b,n,sizeof(b),";");q.op=0;q.mt_id=0;q.buffer=(uint32_t)b;q.length=sl(b);q.seq=0;q.session=0;(void)sc(SYS_MT_DATA,(uint32_t)&q,0,0);}
__attribute__((section(".usertext"))) void program_main(void){uint32_t argc;char**argv;struct process_spawn_request r;uint32_t out[8],hb[3],pid=0,restarts=0,i,j,x,start,last_seen,seen_seq,recovery_pending=0u,recovery_old_pid=0u;__asm__ volatile("movl %%ebx,%0;movl %%ecx,%1":"=r"(argc),"=r"(argv));if(argc<2u||!argv[1][0]){publish("BADARGS",0,0,0);sc(SYS_EXIT,2,0,0);for(;;){}}
(void)recovery_reset();
for(;;){for(i=0;i<sizeof(r);i++)((char*)&r)[i]=0;for(j=0;j<NAME_SIZE-1u&&argv[1][j];j++)r.name[j]=argv[1][j];if(argv[1][j]){publish("BADNAME",0,restarts,0);sc(SYS_EXIT,2,0,0);for(;;){}}pid=sc(SYS_PROCESS_SPAWN,(uint32_t)&r,0,0);if((int)pid<0){if(recovery_pending)(void)recovery_failed(recovery_old_pid,RECOVERY_RESULT_SPAWN_FAILED,pid);publish("SPAWNFAIL",0,restarts,0);sc(SYS_EXIT,4,0,0);for(;;){}}if(recovery_pending&&recovery_spawned(recovery_old_pid,pid)!=0u){publish("RMFAIL",pid,restarts,4u);sc(SYS_EXIT,7,0,0);for(;;){}}start=sc(SYS_TIMER_GET,0,0,0);last_seen=start;seen_seq=0;publish("RUNNING",pid,restarts,0);
for(;;){x=sc(SYS_PROCESS_RESULT,(uint32_t)out,pid,0);if(x==1u){if(recovery_pending)(void)recovery_failed(pid,RECOVERY_RESULT_PROCESS_FAILED,out[2]);break;}if(x==0xfffffffdu||x==0xffffffffu){if(recovery_pending)(void)recovery_failed(pid,RECOVERY_RESULT_PROCESS_FAILED,x);publish("LOST",pid,restarts,0);sc(SYS_EXIT,5,0,0);for(;;){}}x=sc(SYS_PROCESS_HEARTBEAT,(uint32_t)hb,pid,0);if(x==1u&&hb[2]!=seen_seq){seen_seq=hb[2];last_seen=hb[1];if(recovery_pending){if(recovery_verified(pid)!=0u){publish("RMFAIL",pid,restarts,5u);sc(SYS_EXIT,7,0,0);for(;;){}}recovery_pending=0u;publish("RECOVERED",pid,restarts,0);}}else{uint32_t now=sc(SYS_TIMER_GET,0,0,0);if((uint32_t)(now-last_seen)>=WATCHDOG_TIMEOUT_TICKS){publish("TIMEOUT",pid,restarts,now-last_seen);recovery_event(EVENT_REC_WATCHDOG,pid,4502u,now-last_seen);if(recovery_pending)(void)recovery_failed(pid,RECOVERY_RESULT_VERIFY_TIMEOUT,now-last_seen);if(sc(SYS_PROCESS_STOP_PID,pid,0,0)!=0u){(void)recovery_failed(pid,RECOVERY_RESULT_STOP_FAILED,0u);publish("STOPFAIL",pid,restarts,0);sc(SYS_EXIT,6,0,0);for(;;){}}out[2]=3u;out[3]=124u;out[4]=0u;break;}}for(i=0;i<20000u;i++)__asm__ volatile("pause");}
if(out[2]==2u||out[2]==3u){uint32_t detail=out[2]==3u?out[3]:out[4];uint32_t action=recovery_detect(pid,out[2]==3u?2u:1u,detail);if(action==1u){if(recovery_stopped(pid)!=0u){(void)recovery_failed(pid,RECOVERY_RESULT_STOP_FAILED,0u);publish("RMFAIL",pid,restarts,3u);sc(SYS_EXIT,7,0,0);for(;;){}}recovery_old_pid=pid;recovery_pending=1u;restarts++;publish(out[2]==3u?"WDRESTART":"RESTART",pid,restarts,detail);continue;}if(action==2u){publish(out[2]==3u?"WDFAILED":"FAILED",pid,restarts,detail);sc(SYS_EXIT,3,0,0);for(;;){}}publish("RMFAIL",pid,restarts,action);sc(SYS_EXIT,7,0,0);for(;;){}}publish("NORMAL",pid,restarts,0);sc(SYS_EXIT,0,0,0);for(;;){}}
}
__asm__(".section .start,\"ax\"\n.global _start\n_start:\njmp _program_main\n");
