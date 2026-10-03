/* Ring-3 interactive shell. The shell contains its own tiny INT 80h wrappers,
   so it never calls a kernel C function directly from CPL3. */
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef signed int int32_t;
#define SYS_CONSOLE_WRITE 1u
#define SYS_CONSOLE_READ  2u
#define SYS_TIMER_GET     3u
#define SYS_DISK_READ     4u
#define SYS_DISK_WRITE    5u
#define SYS_FILE_OPEN     6u
#define SYS_FILE_READ     7u
#define SYS_FILE_WRITE    8u
#define SYS_FILE_CLOSE    9u
#define SYS_FILE_DELETE  10u
#define SYS_EXEC          11u
#define SYS_EXIT          12u
#define SYS_PORT_OUT8    13u
#define SYS_PORT_IN8     14u
#define SYS_EXEC_QUEUE  15u
#define SYS_QUEUE_STOP  16u
#define SYS_FILE_SIZE   17u
#define SYS_MT_START    18u
#define SYS_MT_STOP     19u
#define SYS_SCHED_BLOCK 20u
#define SYS_SCHED_WAKE  21u
#define SYS_SCHED_EXIT  22u
#define SYS_EXECMT      23u
#define SYS_LS          24u
#define SYS_EXEC_ARGS   25u
#define SYS_EXEC_QUEUE_ARGS 26u
#define SYS_MKDIR          27u
#define SYS_RMDIR          28u
#define SYS_CHDIR          29u
#define SYS_GETCWD         30u
#define SYS_FILE_COPY      31u
#define SYS_VIDEO_MAP      32u
#define SYS_VIDEO_UNMAP    33u
#define SYS_EXEC_ARG       34u
#define SYS_RT_START       38u
#define SYS_RT_INFO        39u
#define SYS_RT_TRACE       41u
#define SYS_RT_TIME_GET    45u
#define SYS_RT_JITTER_INFO 46u
#define SYS_RT_STATS        48u
#define SYS_RT_YIELD        49u
#define SYS_RT_DATA         50u
#define SYS_MT_STATUS       51u
#define SYS_MT_STOP_ONE     52u
#define SYS_MT_DATA         53u
#define SYS_MT_STATS        54u
#define SYS_PROCESS_INFO     55u
#define SYS_PROCESS_WAIT     56u
#define SYS_PROCESS_SPAWN    57u
#define SYS_PROCESS_HEARTBEAT 60u
#define SYS_RESOURCE_INFO     58u
#define SYS_EVENT_LOG          62u
#define SYS_SAFE_MODE          63u
#define SYS_HW_WATCHDOG        64u
#define SYS_SYSTEM_HEALTH       65u
#define SYS_SAFE_POLICY          66u
#define SYS_RECOVERY_EVENT       67u
#define SYS_RECOVERY_MANAGER     68u
#define SYS_UART_TRANSPORT 70u
#define SYS_DATA_CHANNEL 71u
#define SYS_EXECMT_COMMIT 75u /* FIX60O */
#define SYS_FILE_PREAD 72u
#define SYS_FILE_PWRITE 73u
#define SYS_LAYOUT_INFO           69u
#define EVENT_REC_NORMAL 100u
#define EVENT_REC_DEGRADED 101u
#define EVENT_REC_SAFE 102u
#define EVENT_REC_RESTART 103u
#define EVENT_REC_WATCHDOG 104u
#define EVENT_REC_BUDGET 105u
#define SAFE_POLICY_DENIED 0xfffffffcu
#define SYS_RT_STATUS       43u
#define RT_STATUS_STOP      1u
#define RT_STOP_ALL         0xffffffffu
#define SYS_VIDEO_TEXT     35u
#define SYS_CONSOLE_AT     36u
#define SYS_CONSOLE_POLL   37u
#define EXEC_ARG_SIZE 16u
#define EXEC_ARG_BLOCK_SIZE 48u
#define EXEC_QUEUE_RECORD_SIZE 61u
#define EXECMT_MAX_TASKS 25u
#define SPAWN_ARGC_MAX 8u
#define SPAWN_ARG_SIZE 32u
struct process_spawn_request { char name[13]; uint32_t argc; char argv[SPAWN_ARGC_MAX][SPAWN_ARG_SIZE]; };
#define QUEUE_MAX_FILES 25u
#define QUEUE_MAX_REPS  3000u
#define FAT16_MODE_READ   0x01u
#define FAT16_MODE_WRITE  0x02u
#define FAT16_MODE_CREATE 0x04u
#define FAT16_MODE_TRUNC  0x08u
#define FAT16_MODE_APPEND 0x10u
#define RT_PENDING_MAX 8u
#define SHELL_READ_F10 4u
#define SHELL_READ_UP  5u
#define SHELL_HISTORY_CAP 400u
__attribute__((section(".userdata"),used)) static char shell_history[SHELL_HISTORY_CAP]={0};
__attribute__((section(".userdata"),used)) static uint32_t shell_history_valid=0u;
__attribute__((section(".userdata"),used)) static char pending_rt_args[RT_PENDING_MAX][EXEC_ARG_BLOCK_SIZE]={{0}};
__attribute__((section(".userdata"),used)) static uint32_t pending_rt_count=0u;
__attribute__((section(".userdata"),used)) static uint32_t shell_f10_event=0u;
/* FIX36D: test runner workspace is static user data, not the one-page Ring3 stack.
 * TEST can call ordinary shell commands whose own stack frames are already large. */
__attribute__((section(".userdata"),used)) static uint32_t test_process_snapshot[34u*10u]={0};
__attribute__((section(".userdata"),used)) static char test_line[400]={0};
__attribute__((section(".userdata"),used)) static char test_chunk[256]={0};
/* FIX40A: deterministic load-test command/staging buffers stay off the 4 KiB Ring3 stack. */
__attribute__((section(".userdata"),used)) static char test_stage_line[400]={0};
__attribute__((section(".userdata"),used)) static char test_generated_line[400]={0};
__attribute__((section(".userdata"),used)) static char test_mtdata_snapshot[256]={0}; /* FIX40B */
__attribute__((section(".userdata"),used)) static uint32_t test_stage_valid=0u;
__attribute__((section(".userdata"),used)) static uint32_t test_runner_active=0u;
/* FIX60X: compact end-of-run failure summary only. This is not a test log:
 * no command output is buffered and nothing is written to disk. */
#define TEST_FAIL_SUMMARY_MAX 128u
typedef struct { char name[20]; uint32_t line; } test_fail_summary_t;
__attribute__((section(".userdata"),used)) static test_fail_summary_t test_fail_summary[TEST_FAIL_SUMMARY_MAX];
__attribute__((section(".userdata"),used)) static uint32_t test_fail_summary_count=0u;
__attribute__((section(".userdata"),used)) static uint32_t test_fail_summary_overflow=0u;
__attribute__((section(".userdata"),used)) static uint32_t test_accept_active=0u;
__attribute__((section(".userdata"),used)) static uint32_t last_spawn_pid=0u;
__attribute__((section(".userdata"),used)) static uint32_t test_last_wait_valid=0u,test_last_wait_reason=0u,test_last_wait_status=0u;
/* FIX36D: large shell diagnostic snapshots are static as well, so TEST RUN
 * cannot stack a dispatcher frame on top of kilobyte-sized local arrays. */
__attribute__((section(".userdata"),used)) static uint32_t shell_process_snapshot[34u*10u]={0};
__attribute__((section(".userdata"),used)) static uint32_t shell_mtstat_snapshot[25u*10u]={0};
__attribute__((section(".userdata"),used)) static uint32_t shell_rtstat_snapshot[240u]={0};
__attribute__((section(".userdata"),used)) static uint32_t shell_rtdump_snapshot[65u]={0};
__attribute__((section(".userdata"),used)) static uint32_t shell_event_snapshot[1u+64u*8u]={0};
/* FIX42: persistent event-log workspace is static: Ring3 has only a 4 KiB stack. */
#define EVENT_DISK_NAME "EVENT.LOG"
#define EVENT_DISK_MAGIC 0x474c5654u /* "TVLG" little-endian */
#define EVENT_DISK_VERSION 1u
#define EVENT_DISK_HEADER_WORDS 5u
#define EVENT_DISK_MAX_WORDS (EVENT_DISK_HEADER_WORDS+64u*8u)
__attribute__((section(".userdata"),used)) static uint32_t shell_event_disk[EVENT_DISK_MAX_WORDS]={0};

/* FIX43: persistent reason-for-reboot marker.  This is deliberately a small
 * Ring3-managed FAT16 record: no filesystem I/O is added to IRQ/fault paths. */
#define BOOT_DIAG_NAME "BOOTSTAT.DAT"
#define BOOT_DIAG_MAGIC 0x544f4f42u /* "BOOT" little-endian */
#define BOOT_DIAG_VERSION 1u
#define BOOT_DIAG_WORDS 6u
__attribute__((section(".userdata"),used)) static uint32_t shell_boot_diag[BOOT_DIAG_WORDS]={0};

/* Syscall ABI: EAX=number, EBX=arg1, ECX=arg2, EDX=arg3, EAX=return. */
__attribute__((section(".usertext"))) static uint32_t syscall3(uint32_t n,uint32_t a,uint32_t b,uint32_t c){
    uint32_t r;
    __asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");
    return r;
}
__attribute__((section(".usertext"))) static uint32_t sys_console_write(const char*s,uint32_t n){return syscall3(SYS_CONSOLE_WRITE,(uint32_t)s,n,0);}
__attribute__((section(".usertext"))) static uint32_t sys_console_read(char*b,uint32_t n){return syscall3(SYS_CONSOLE_READ,(uint32_t)b,n,0);}
__attribute__((section(".usertext"))) static uint32_t sys_timer_get(void){return syscall3(SYS_TIMER_GET,0,0,0);}
__attribute__((section(".usertext"))) static uint32_t sys_rt_time_get(void){return syscall3(SYS_RT_TIME_GET,0,0,0);}
__attribute__((section(".usertext"))) static uint32_t sys_rt_yield(void){return syscall3(SYS_RT_YIELD,0,0,0);}
__attribute__((section(".usertext"))) static uint32_t sys_rt_jitter_info(uint32_t*out){return syscall3(SYS_RT_JITTER_INFO,(uint32_t)out,0,0);}
__attribute__((section(".usertext"))) static uint32_t sys_rt_stats(void*out,uint32_t op){return syscall3(SYS_RT_STATS,(uint32_t)out,op,0);}
__attribute__((section(".usertext"))) static uint32_t sys_rt_stop(uint32_t target){return syscall3(SYS_RT_STATUS,0,RT_STATUS_STOP,target);}
__attribute__((section(".usertext"))) static uint32_t sys_rt_data_read(void*out,uint32_t slot){return syscall3(SYS_RT_DATA,(uint32_t)out,0u,slot);}
__attribute__((section(".usertext"))) static uint32_t sys_disk_read(uint32_t lba,void*b,uint32_t n){return syscall3(SYS_DISK_READ,lba,(uint32_t)b,n);}
__attribute__((section(".usertext"))) static uint32_t sys_disk_write(uint32_t lba,const void*b,uint32_t n){return syscall3(SYS_DISK_WRITE,lba,(uint32_t)b,n);}
__attribute__((section(".usertext"))) static uint32_t sys_file_open(const char*name,uint32_t mode){return syscall3(SYS_FILE_OPEN,(uint32_t)name,mode,0);}
__attribute__((section(".usertext"))) static uint32_t sys_file_read(uint32_t fd,void*b,uint32_t n){return syscall3(SYS_FILE_READ,fd,(uint32_t)b,n);}
__attribute__((section(".usertext"))) static uint32_t sys_file_write(uint32_t fd,const void*b,uint32_t n){return syscall3(SYS_FILE_WRITE,fd,(uint32_t)b,n);}
__attribute__((section(".usertext"))) static uint32_t sys_file_close(uint32_t fd){return syscall3(SYS_FILE_CLOSE,fd,0,0);}
__attribute__((section(".usertext"))) static uint32_t sys_file_size(const char*name){return syscall3(SYS_FILE_SIZE,(uint32_t)name,0,0);}
__attribute__((section(".usertext"))) static uint32_t sys_file_delete(const char*name){return syscall3(SYS_FILE_DELETE,(uint32_t)name,0,0);}
__attribute__((section(".usertext"))) static uint32_t sys_file_copy(const char*src,const char*dst){return syscall3(SYS_FILE_COPY,(uint32_t)src,(uint32_t)dst,0);}
__attribute__((section(".usertext"))) static uint32_t sys_port_out8(uint32_t port,uint32_t value){return syscall3(SYS_PORT_OUT8,port,value,0);}
__attribute__((section(".usertext"))) static uint32_t sys_port_in8(uint32_t port){return syscall3(SYS_PORT_IN8,port,0,0);}
__attribute__((section(".usertext"))) static uint32_t sys_console_at(uint32_t row,uint32_t col,const char*s,uint32_t n){return syscall3(SYS_CONSOLE_AT,(row<<16)|col,(uint32_t)s,n);}
__attribute__((section(".usertext"))) static uint32_t sys_console_poll(void*b,uint32_t n){return syscall3(SYS_CONSOLE_POLL,(uint32_t)b,n,0);}
__attribute__((section(".usertext"))) static uint32_t sys_exec_queue(const char*names,uint32_t count,uint32_t repetitions){return syscall3(SYS_EXEC_QUEUE,(uint32_t)names,count,repetitions);}
__attribute__((section(".usertext"))) static uint32_t sys_queue_stop(void){return syscall3(SYS_QUEUE_STOP,0,0,0);}
__attribute__((section(".usertext"))) static uint32_t sys_exec(const char*name){return syscall3(SYS_EXEC,(uint32_t)name,0,0);}
__attribute__((section(".usertext"))) static uint32_t sys_process_spawn(const void*r){return syscall3(SYS_PROCESS_SPAWN,(uint32_t)r,0,0);}
__attribute__((section(".usertext"))) static uint32_t sys_resource_info(uint32_t op,uint32_t pid){return syscall3(SYS_RESOURCE_INFO,op,pid,0u);}
__attribute__((section(".usertext"))) static uint32_t sys_event_log(uint32_t*out,uint32_t op){return syscall3(SYS_EVENT_LOG,(uint32_t)out,op,0u);}
__attribute__((section(".usertext"))) static uint32_t sys_safe_mode(uint32_t*out,uint32_t op,uint32_t value,uint32_t reason){return syscall3(SYS_SAFE_MODE,op?value:(uint32_t)out,op,reason);}
__attribute__((section(".usertext"))) static uint32_t sys_hwwd(uint32_t*out,uint32_t op,uint32_t value){return syscall3(SYS_HW_WATCHDOG,op==0u?(uint32_t)out:value,op,0u);}
__attribute__((section(".usertext"))) static uint32_t sys_system_health(uint32_t*out){return syscall3(SYS_SYSTEM_HEALTH,(uint32_t)out,0u,0u);}
__attribute__((section(".usertext"))) static uint32_t sys_safe_policy(uint32_t*out,uint32_t op){return syscall3(SYS_SAFE_POLICY,op?(uint32_t)0:(uint32_t)out,op,0u);}
__attribute__((section(".usertext"))) static uint32_t sys_recovery_manager(uint32_t*out,uint32_t op){return syscall3(SYS_RECOVERY_MANAGER,op?(uint32_t)0:(uint32_t)out,op,0u);}
__attribute__((section(".usertext"))) static uint32_t sys_recovery_manager_ext(uint32_t*out){return syscall3(SYS_RECOVERY_MANAGER,(uint32_t)out,7u,0u);}
__attribute__((section(".usertext"))) static uint32_t sys_layout_info(uint32_t*q){return syscall3(SYS_LAYOUT_INFO,(uint32_t)q,0u,0u);}
__attribute__((section(".usertext"))) static uint32_t sys_exec_args(const char*name,const void*args){return syscall3(SYS_EXEC_ARGS,(uint32_t)name,(uint32_t)args,0);}
__attribute__((section(".usertext"))) static uint32_t sys_exec_queue_args(const void*jobs,uint32_t count,uint32_t reps){return syscall3(SYS_EXEC_QUEUE_ARGS,(uint32_t)jobs,count,reps);}
__attribute__((section(".usertext"))) static uint32_t sys_mt_start(void){return syscall3(SYS_MT_START,0,0,0);}
__attribute__((section(".usertext"))) static uint32_t sys_mt_stop(void){return syscall3(SYS_MT_STOP,0,0,0);}
__attribute__((section(".usertext"))) static uint32_t sys_sched_block(void){return syscall3(SYS_SCHED_BLOCK,0,0,0);}
__attribute__((section(".usertext"))) static uint32_t sys_sched_wake(uint32_t id){return syscall3(SYS_SCHED_WAKE,id,0,0);}
__attribute__((section(".usertext"))) static uint32_t sys_sched_exit(void){return syscall3(SYS_SCHED_EXIT,0,0,0);}
__attribute__((section(".usertext"))) static uint32_t sys_execmt(const char*names,uint32_t count){return syscall3(SYS_EXECMT,(uint32_t)names,count,0);}
__attribute__((section(".usertext"))) static uint32_t sys_mt_status(void*out){return syscall3(SYS_MT_STATUS,(uint32_t)out,0,0);}
__attribute__((section(".usertext"))) static uint32_t sys_mt_stats(void*out){return syscall3(SYS_MT_STATS,(uint32_t)out,0,0);}
__attribute__((section(".usertext"))) static uint32_t sys_mt_stop_one(uint32_t id){return syscall3(SYS_MT_STOP_ONE,id,0,0);}
struct mt_data_request {uint32_t op,mt_id,buffer,length,seq,session;};
__attribute__((section(".usertext"))) static uint32_t sys_mt_data_read(void*out,uint32_t id,uint32_t*seq,uint32_t*session){
    struct mt_data_request r;r.op=1u;r.mt_id=id;r.buffer=(uint32_t)out;r.length=256u;r.seq=0u;r.session=0u;
    {uint32_t v=syscall3(SYS_MT_DATA,(uint32_t)&r,0u,0u);if(seq)*seq=r.seq;if(session)*session=r.session;return v;}
}
__attribute__((section(".usertext"))) static uint32_t sys_ls(void){return syscall3(SYS_LS,0,0,0);}
__attribute__((section(".usertext"))) static uint32_t sys_ls_path(const char*path){return syscall3(SYS_LS,(uint32_t)path,0,0);}
__attribute__((section(".usertext"))) static uint32_t sys_mkdir(const char*path){return syscall3(SYS_MKDIR,(uint32_t)path,0,0);}
__attribute__((section(".usertext"))) static uint32_t sys_rmdir(const char*path){return syscall3(SYS_RMDIR,(uint32_t)path,0,0);}
__attribute__((section(".usertext"))) static uint32_t sys_chdir(const char*path){return syscall3(SYS_CHDIR,(uint32_t)path,0,0);}
__attribute__((section(".usertext"))) static uint32_t sys_getcwd(char*out,uint32_t cap){return syscall3(SYS_GETCWD,(uint32_t)out,cap,0);}


__attribute__((section(".usertext"))) static uint32_t sl(const char*s){uint32_t n=0;while(s[n])n++;return n;}
__attribute__((section(".usertext"))) static void put(const char*s){sys_console_write(s,sl(s));}
__attribute__((section(".usertext"))) static void putn(const char*s,uint32_t n){sys_console_write(s,n);}
__attribute__((section(".usertext"))) static void dec(uint32_t v,char*b){static const uint32_t p[10]={1000000000u,100000000u,10000000u,1000000u,100000u,10000u,1000u,100u,10u,1u};uint32_t d,st=0,i;for(i=0;i<10;i++){d=0;while(v>=p[i]){v-=p[i];d++;}if(d||st||i==9){*b++=(char)('0'+d);st=1;}}*b=0;}
__attribute__((section(".usertext"))) static void hex8(uint32_t v,char*b){static const char h[]="0123456789abcdef";b[0]=h[(v>>4)&15u];b[1]=h[v&15u];b[2]=0;}
/* v67.1: сравнение имён команд выполняется без учёта регистра ASCII.
 * Это намеренно ограничено ASCII A-Z/a-z: ToyOS использует имена команд и
 * FAT 8.3, поэтому локализованное Unicode-преобразование здесь не требуется.
 * Исходная строка пользователя не изменяется: регистр аргументов и текстовых
 * данных передаётся дальше в исходном виде. */
__attribute__((section(".usertext"))) static char ascii_fold(char c){if(c>='A'&&c<='Z')return (char)(c-'A'+'a');return c;}
__attribute__((section(".usertext"))) static uint32_t eq(const char*a,const char*b){uint32_t i=0;while(a[i]&&b[i]&&ascii_fold(a[i])==ascii_fold(b[i]))i++;return a[i]==0&&b[i]==0;}
/* v67.1: prefix() используется только для распознавания командных ключевых
 * слов и их разделителя. Поэтому регистр ключевого слова игнорируется, а
 * всё, что следует после пробела, остаётся нетронутым и является аргументом. */
__attribute__((section(".usertext"))) static uint32_t prefix(const char*a,const char*b){uint32_t i=0;while(b[i]){if(ascii_fold(a[i])!=ascii_fold(b[i]))return 0;i++;}return 1;}
__attribute__((section(".usertext"))) static void skip(char**p){while(**p==' '||**p=='\t')(*p)++;}
__attribute__((section(".usertext"))) static uint32_t number(char**p){uint32_t v=0,d;skip(p);if((*p)[0]=='0'&&((*p)[1]=='x'||(*p)[1]=='X')){*p+=2;while(1){if(**p>='0'&&**p<='9')d=(uint32_t)(**p-'0');else if(**p>='a'&&**p<='f')d=(uint32_t)(**p-'a'+10);else if(**p>='A'&&**p<='F')d=(uint32_t)(**p-'A'+10);else break;v=(v<<4)|d;(*p)++;}}else{while(**p>='0'&&**p<='9'){v=v*10u+(uint32_t)(*(*p)-'0');(*p)++;}}return v;}
__attribute__((section(".usertext"))) static void trim(char*s){uint32_t n=sl(s);while(n&& (s[n-1]==' '||s[n-1]=='\t'||s[n-1]=='\n'||s[n-1]=='\r'))s[--n]=0;}
__attribute__((section(".usertext"))) static void readline(char*b,uint32_t cap){
    uint32_t n=0;
    while(n+1<cap){
        char c;
        uint32_t r=sys_console_read(&c,1);
        if(r==0xffffffffu){put("read: FAIL\n");b[0]=0;return;}
        if(r==SHELL_READ_F10){
            if(n)put("\n");
            b[n]=0;
            /* FIX60ZED: do not silently discard a fully typed RTD command
               when F10 is used as the launch key instead of ENTER+F10. */
            shell_f10_event=n?2u:1u;
            return;
        }
        if(r==SHELL_READ_UP){
            uint32_t i;
            if(!shell_history_valid)continue;
            while(n){
                put("\b \b");
                n--;
            }
            for(i=0;i+1<cap&&shell_history[i];i++)b[i]=shell_history[i];
            n=i;
            b[n]=0;
            if(n)putn(b,n);
            continue;
        }
        if(r==2u){
            /* FIX60N: while detached MT work exists, keep a bounded Ring-3
               window. IRQ0 alone captures the live foreground frame and
               performs MT dispatch/preemption; console syscall never swaps
               directly to an MT frame. */
            uint32_t spin;
            for(spin=0u;spin<4096u;spin++)__asm__ volatile("pause");
            continue;
        }
        if(r==3u){put("toy0> ");continue;}
        if(r!=1)continue;
        if(c=='\n'){
            uint32_t i;
            b[n]=0;
            put("\n");
            if(n){
                for(i=0;i+1<SHELL_HISTORY_CAP&&i<=n;i++)shell_history[i]=b[i];
                shell_history[SHELL_HISTORY_CAP-1]=0;
                shell_history_valid=1u;
            }
            return;
        }
        if(c=='\b'){
            if(n){n--;put("\b");}
            continue;
        }
        b[n++]=c;
        putn(&c,1);
    }
    b[n]=0;
    put("\n");
}
/*
 * v64: справка разбита на страницы, чтобы добавление новых команд больше не
 * приводило к тому, что нижняя часть help исчезает за пределами экрана.
 * Нумерация начинается с 1. `help` показывает первую страницу, а `help N`
 * — указанную страницу. Каждая страница заканчивается подсказкой о переходе.
 */
__attribute__((section(".usertext"))) static void help_page(uint32_t page){
    put("ToyOS help ");{char b[12];dec(page,b);put(b);put("/3\n");}
    if(page==1u){
        put("FILES: ls [P] | cd P | pwd | mkdir/rmdir P | cp S D | rm F\n");
        put("       cat/read F | write F TEXT | append F TEXT | crlf F | filesize F\n");
        put("PROC : exec F | spawn F [ARG..] | ps [PID] | wait PID | resstat [PID]\n       supstart F | supstat | layout | test [F.TST]\n");
        put("SHELL: help [1..3] | clear | echo TEXT | ticks | exit\n");
    }else if(page==2u){
        put("MT   : execmt F.. (new session; one F appends) | mtlist | mtstat [watch]\n");
        put("       mtdata MTn | mtstop MTn|ALL ; live SONARVWR: execmt SONARVWR.EXE\n");
        put("RT   : exec RTD.EXE F PERIOD DEADLINE PRIORITY [DELAY] ; F10=start\n");
        put("       rtstat [stop|compare|watch|dump|events|reset] | rtdata SLOTn\n");
        put("QUEUE: execq N F.. | execq forever F.. | execq-stop\n");
        put("DRIVER: exec LOADER/COMDRV/NETDRV/VGADRV.EXE ...\n");
    }else{
        put("DIAG : cwd-test | pit-test | udp-test | syscall-test | syscall N [ARG]\n");
        put("I/O  : inb PORT | outb PORT VALUE | uartstat | uartreset\n");
        put("TIME : SYS_TIMER_GET=50Hz; SYS_RT_TIME_GET=100Hz (10ms)\n");
        put("BOOT : AUTOSTART.SH runs automatically when present\n");
        put("SAFE : safemode [normal|degraded|safe [REASON]] | safepolicy [reset] | health\n");
        put("RECOVERY: recoverylog [disk] (FAULT/WATCHDOG/RESTART/DEGRADED/SAFE history)\n");
        put("HWWD : hwwd [status|arm TICKS|feed|disarm]\n");
    }
    if(page<3u){put("Next: help ");{char b[12];dec(page+1u,b);put(b);put("\n");}}
}
__attribute__((section(".usertext"))) static void help_command(char*p){uint32_t page=1u;skip(&p);if(*p){page=number(&p);skip(&p);if(*p||page<1u||page>3u){put("usage: help [1..3]\n");return;}}help_page(page);}
__attribute__((section(".usertext"))) static void help(void){help_page(1u);}
__attribute__((section(".usertext"))) static void clear(void){uint32_t i;char s[81];for(i=0;i<80;i++)s[i]=' ';s[80]=0;for(i=0;i<25;i++){put(s);put("\n");}}
__attribute__((section(".usertext"))) static void cat(const char*name){char b[512];uint32_t fd,n;fd=sys_file_open(name,FAT16_MODE_READ);if(fd==0xffffffffu){put("open: FAIL\n");return;}n=sys_file_read(fd,b,511);sys_file_close(fd);if(n==0xffffffffu){put("read: FAIL\n");return;}b[n]=0;put(b);if(!n||b[n-1]!='\n')put("\n");}
__attribute__((section(".usertext"))) static void write_file(const char*name,const char*text,uint32_t append){uint32_t fd,n;uint32_t mode=FAT16_MODE_WRITE|FAT16_MODE_CREATE|(append?FAT16_MODE_APPEND:FAT16_MODE_TRUNC);fd=sys_file_open(name,mode);if(fd==0xffffffffu){put("open: FAIL\n");return;}n=sys_file_write(fd,text,sl(text));sys_file_close(fd);if(n==sl(text))put("write: OK\n");else put("write: FAIL\n");}
__attribute__((section(".usertext"))) static void crlf_file(const char*name){static const char eol[2]={'\r','\n'};uint32_t fd,n;fd=sys_file_open(name,FAT16_MODE_WRITE|FAT16_MODE_APPEND);if(fd==0xffffffffu){put("crlf: file not found/open failed\n");return;}n=sys_file_write(fd,eol,2u);sys_file_close(fd);put(n==2u?"crlf: OK\n":"crlf: FAIL\n");}
__attribute__((section(".usertext"))) static void filesize(const char*name){uint32_t r=sys_file_size(name);if(r==0xffffffffu){put("filesize: FAIL\n");return;}char b[12];dec(r,b);put(b);put(" bytes\n");}
/* v61: интерактивная проверка cwd без изменения файловой системы.
 * Используется уже существующий каталог BIN, который mkfat16 создаёт при сборке.
 * Тестирует cd, pwd, относительные пути и нормализацию '.', '..'. */
__attribute__((section(".usertext"))) static void cwd_test(void){
    char before[128],after[128];uint32_t r;
    r=sys_getcwd(before,sizeof(before));
    if(r==0xffffffffu){put("cwd-test: GETCWD FAIL\n");return;}
    if(sys_chdir("/BIN")!=0){put("cwd-test: CD /BIN FAIL\n");return;}
    r=sys_getcwd(after,sizeof(after));
    if(r==0xffffffffu||!eq(after,"/BIN")){put("cwd-test: /BIN verification FAIL\n");sys_chdir(before);return;}
    if(sys_chdir("./../BIN/./")!=0){put("cwd-test: normalization FAIL\n");sys_chdir(before);return;}
    r=sys_getcwd(after,sizeof(after));
    if(r==0xffffffffu||!eq(after,"/BIN")){put("cwd-test: normalized path FAIL\n");sys_chdir(before);return;}
    if(sys_ls_path(".")!=0){put("cwd-test: relative ls FAIL\n");sys_chdir(before);return;}
    if(sys_chdir("..")!=0){put("cwd-test: CD .. FAIL\n");sys_chdir(before);return;}
    r=sys_getcwd(after,sizeof(after));
    if(r==0xffffffffu||!eq(after,"/")){put("cwd-test: root normalization FAIL\n");sys_chdir(before);return;}
    if(sys_chdir(before)!=0){put("cwd-test: restore FAIL\n");return;}
    put("cwd-test: PASS\n");
}
__attribute__((section(".usertext"))) static uint32_t token(char**p,char*out,uint32_t cap){uint32_t n=0;skip(p);while(**p&&**p!=' '&&**p!='\t'){if(n+1<cap)out[n++]=**p;(*p)++;}out[n]=0;return n;}
/* v58: отдельная команда LOADER.EXE получает тот же расширенный ABI, что и COMDRV. */
/* Ранее общий обработчик exec передавал только имя файла, поэтому LOADER запускался без аргументов. */
__attribute__((section(".usertext"))) static void exec_loader_command(char*p){
    const char name[]="LOADER.EXE";
    char port[16],dir[16],file[16],args[48];uint32_t r,i;
    /* После префикса "exec LOADER.EXE " указатель p уже стоит перед PORT. */
    token(&p,port,sizeof(port));token(&p,dir,sizeof(dir));token(&p,file,sizeof(file));skip(&p);
    if(!port[0]||!dir[0]||!file[0]||*p){put("usage: exec LOADER.EXE PORT RECV FILE.EXE\n");return;}
    /* Упаковываем PORT, RECV и имя файла в существующий блок из трёх полей по 16 байт. */
    for(i=0;i<48;i++)args[i]=0;
    for(i=0;i<16&&port[i];i++)args[i]=port[i];
    for(i=0;i<16&&dir[i];i++)args[16+i]=dir[i];
    for(i=0;i<16&&file[i];i++)args[32+i]=file[i];
    /* Передаём аргументы LOADER через уже существующий SYS_EXEC_ARGS, не меняя ABI ядра. */
    r=sys_exec_args(name,args);
    if(r!=0)put("exec: FAIL\n");
}

/* v58 NETDRV: shell передаёт IP, направление и имя файла через существующий
   SYS_EXEC_ARGS. Никаких новых syscall и изменений ядра для NETDRV не требуется. */
__attribute__((section(".usertext"))) static void exec_netdrv_command(char*p){
    /* v66.7: NETDRV.EXE гарантированно присутствует как в корне, так и в BIN/.
       Важное исправление: пути стали абсолютными. Это исключает ошибку EXE_ERR_OPEN
       (-3), если текущий каталог shell отличен от корня, например /BIN.
       Пользовательская команда остаётся прежней. */
    const char name[]="/NETDRV.EXE";
    const char bin_name[]="/BIN/NETDRV.EXE";
    char ip[16],dir[16],file[16],args[48];
    uint32_t r,i;
    token(&p,ip,sizeof(ip));
    token(&p,dir,sizeof(dir));
    token(&p,file,sizeof(file));
    skip(&p);
    if(!ip[0]||!dir[0]||!file[0]||*p){
        put("usage: exec NETDRV.EXE IP SEND|RECV FILE.TXT\n");
        return;
    }
    for(i=0;i<48;i++)args[i]=0;
    for(i=0;i<16&&ip[i];i++)args[i]=ip[i];
    for(i=0;i<16&&dir[i];i++)args[16+i]=dir[i];
    for(i=0;i<16&&file[i];i++)args[32+i]=file[i];
    r=sys_exec_args(bin_name,args);
    if(r!=0){
        /* Резервный путь нужен для старых образов, где BIN/NETDRV.EXE ещё
           отсутствует, но корневой NETDRV.EXE уже записан. */
        r=sys_exec_args(name,args);
    }
    if(r!=0){
        put("exec NETDRV.EXE: FAIL ");
        {char b[12];dec(r,b);put(b);}
        put("\n");
    }
}

__attribute__((section(".usertext"))) static void exec_com_command(char*p){
    const char name[]="COMDRV.EXE";
    char port[16],dir[16],file[16],args[48];uint32_t r;
    /* shell_run() вызывает helper уже после префикса "exec COMDRV.EXE ".
       Поэтому первым аргументом здесь является PORT, а не имя EXE. */
    token(&p,port,sizeof(port));token(&p,dir,sizeof(dir));token(&p,file,sizeof(file));skip(&p);
    if(!port[0]||!dir[0]||!file[0]||*p){put("usage: exec COMDRV.EXE PORT SEND|RECV FILE\n");return;}
    {uint32_t i;for(i=0;i<48;i++)args[i]=0;for(i=0;i<16&&port[i];i++)args[i]=port[i];for(i=0;i<16&&dir[i];i++)args[16+i]=dir[i];for(i=0;i<16&&file[i];i++)args[32+i]=file[i];}
    r=sys_exec_args(name,args);if(r!=0){put("exec: FAIL\n");}}
__attribute__((section(".usertext"))) static void exec_queue_com_command(char*p,uint32_t forever){
    char exe[16],port[16],dir[16],file[16],jobs[61];uint32_t reps,r,i;
    skip(&p);if(forever)reps=0;else{reps=number(&p);if(reps<1u||reps>QUEUE_MAX_REPS){put("execq: repetitions must be 1..3000\n");return;}}
    token(&p,exe,sizeof(exe));token(&p,port,sizeof(port));token(&p,dir,sizeof(dir));token(&p,file,sizeof(file));skip(&p);
    if(!exe[0]||!port[0]||!dir[0]||!file[0]||*p){put(forever?"usage: execq forever COMDRV.EXE PORT SEND|RECV FILE\n":"usage: execq REPEAT COMDRV.EXE PORT SEND|RECV FILE\n");return;}
    for(i=0;i<61;i++)jobs[i]=0;
    for(i=0;i<13&&exe[i];i++)jobs[i]=exe[i];
    for(i=0;i<16&&port[i];i++)jobs[13+i]=port[i];
    for(i=0;i<16&&dir[i];i++)jobs[29+i]=dir[i];
    for(i=0;i<16&&file[i];i++)jobs[45+i]=file[i];
    put(forever?"execq: COMDRV queue started; receive ends on idle timeout\n":"execq: COMDRV queue started\n");
    r=sys_exec_queue_args(jobs,1,reps);if(r==0)put("execq: finished\n");else put("execq: stopped or failed\n");
}

__attribute__((section(".usertext"))) static void exec_vga_command(char*p){
    const char name[]="VGADRV.EXE";
    char file[16],args[48];uint32_t r,i;
    token(&p,file,sizeof(file));
    skip(&p);
    if(!file[0]||*p){put("usage: exec VGADRV.EXE FILE\n");return;}
    for(i=0;i<48;i++)args[i]=0;
    for(i=0;i<16&&file[i];i++)args[i]=file[i];
    r=sys_exec_args(name,args);
    if(r!=0)put("exec: FAIL\n");
}
__attribute__((section(".usertext"))) static void exec_rtd_command(char*p){
    char target[EXEC_ARG_SIZE],period[EXEC_ARG_SIZE],deadline[EXEC_ARG_SIZE],priority[EXEC_ARG_SIZE],delay[EXEC_ARG_SIZE];
    char packed[EXEC_ARG_SIZE];uint32_t i,n=0,slot;
    token(&p,target,sizeof(target)); token(&p,period,sizeof(period)); token(&p,deadline,sizeof(deadline)); token(&p,priority,sizeof(priority)); token(&p,delay,sizeof(delay)); skip(&p);
    if(!target[0]||!period[0]||!deadline[0]||!priority[0]||*p){put("usage: exec RTD.EXE SENSOR.EXE PERIOD_MS DEADLINE_MS PRIORITY [DELAY_MS]\n");return;}
    if(pending_rt_count>=RT_PENDING_MAX){put("RTD: pending queue is full (maximum 8 tasks); press F10 to start queued task(s)\n");return;}
    for(i=0;i<sizeof(packed);i++)packed[i]=0;
    for(i=0;i<sizeof(period)-1u&&period[i]&&n<sizeof(packed)-1u;i++)packed[n++]=period[i];
    if(n+1u>=sizeof(packed)){put("exec RTD: parameter string too long\n");return;}packed[n++]=' ';
    for(i=0;i<sizeof(deadline)-1u&&deadline[i]&&n<sizeof(packed)-1u;i++)packed[n++]=deadline[i];
    if(n+1u>=sizeof(packed)){put("exec RTD: parameter string too long\n");return;}packed[n++]=' ';
    for(i=0;i<sizeof(priority)-1u&&priority[i]&&n<sizeof(packed)-1u;i++)packed[n++]=priority[i];
    if(delay[0]){if(n+1u>=sizeof(packed)){put("exec RTD: parameter string too long\n");return;}packed[n++]=' ';for(i=0;i<sizeof(delay)-1u&&delay[i]&&n<sizeof(packed)-1u;i++)packed[n++]=delay[i];}
    packed[n]=0;slot=pending_rt_count;
    for(i=0;i<EXEC_ARG_BLOCK_SIZE;i++)pending_rt_args[slot][i]=0;
    for(i=0;i<sizeof(target)-1u&&target[i];i++)pending_rt_args[slot][i]=target[i];
    for(i=0;i<sizeof(packed)-1u&&packed[i];i++)pending_rt_args[slot][EXEC_ARG_SIZE+i]=packed[i];
    pending_rt_count++;
    put("RTD: task prepared: ");put(target);put(" ");put(period);put(" ");put(deadline);put(" ");put(priority);if(delay[0]){put(" ");put(delay);}put("\n");
    put("RTD: enter next RT command or press F10 to start queued task(s)\n");
}

__attribute__((section(".usertext"))) static void rt_launch_pending(void){
    uint32_t i,j,count=pending_rt_count,r;uint32_t launched=0u;
    /* F10 launches detached RT tasks.  SENSOR itself is always silent;
       RTD has no verbose/quiet task mode. */
    if(count==0u){put("RTD: no queued tasks\n");return;}
    put("RTD: F10 pressed, starting queued task(s): ");{char b[12];dec(count,b);put(b);}put("\n");
    /* FIX24: make the multi-RTD F10 transaction foreground-atomic with respect
       to RT/MT dispatch. IRQ0 and release accounting continue in the kernel;
       only task dispatch is deferred until every RTD helper has returned. */
    sys_rt_stats((void*)1,7u);
    for(i=0;i<count;i++){
        r=sys_exec_args("/RTD.EXE",pending_rt_args[i]);
        if(r==0u){
            char name[EXEC_ARG_SIZE],spec[EXEC_ARG_SIZE];uint32_t k=0u;
            launched++;
            for(k=0u;k<sizeof(name)-1u&&pending_rt_args[i][k];k++)name[k]=pending_rt_args[i][k];
            name[k]=0;
            for(k=0u;k<sizeof(spec)-1u&&pending_rt_args[i][EXEC_ARG_SIZE+k];k++)spec[k]=pending_rt_args[i][EXEC_ARG_SIZE+k];
            spec[k]=0;
            put("RTD: started ");put(name);put(" ");put(spec);put("\n");
        }
        else{put("RTD: task ");{char b[12];dec(i+1u,b);put(b);}if(r==SAFE_POLICY_DENIED){put(" DENIED by SAFE policy\n");}else{put(" start FAIL code=");{char b[12];dec(r,b);put(b);}put("\n");}break;}
    }
    sys_rt_stats((void*)0,7u);
    if(launched==count){pending_rt_count=0u;return;}
    for(j=launched;j<count;j++)for(r=0;r<EXEC_ARG_BLOCK_SIZE;r++)pending_rt_args[j-launched][r]=pending_rt_args[j][r];
    pending_rt_count=count-launched;
}
__attribute__((section(".usertext"))) static void spawn_command(char*p){
    struct process_spawn_request r;char tok[SPAWN_ARG_SIZE];uint32_t i,j,pid;
    for(i=0;i<sizeof(r);i++)((char*)&r)[i]=0;token(&p,tok,sizeof(tok));if(!tok[0]){put("usage: spawn FILE.EXE [ARG1..ARG8]\n");return;}
    for(i=0;i<12u&&tok[i];i++)r.name[i]=tok[i];if(tok[i]){put("spawn: file name too long\n");return;}
    while(1){skip(&p);if(!*p)break;if(r.argc>=SPAWN_ARGC_MAX){put("spawn: maximum 8 arguments\n");return;}token(&p,tok,sizeof(tok));for(j=0;j<SPAWN_ARG_SIZE-1u&&tok[j];j++)r.argv[r.argc][j]=tok[j];if(tok[j]){put("spawn: argument too long\n");return;}r.argc++;}
    pid=sys_process_spawn(&r);if(pid==SAFE_POLICY_DENIED){put("spawn: DENIED by SAFE policy\n");return;}if((int32_t)pid<0){
        /* FIX37A: never hide the loader reason.  This is important on the
           target because OPEN/HEADER/MEMORY/BUSY failures require different
           corrective action and a generic FAIL made FIX37 impossible to diagnose. */
        put("spawn: FAIL ");
        switch((int32_t)pid){case -1:put("BUSY");break;case -2:put("NAME");break;case -3:put("OPEN");break;case -4:put("HEADER");break;case -5:put("MAGIC");break;case -6:put("IMAGE_SIZE");break;case -7:put("ENTRY");break;case -8:put("BSS_SIZE");break;case -9:put("IMAGE_READ");break;case -10:put("TRAILING");break;case -13:put("MEMORY");break;default:put("CODE=");{char b[12];dec((uint32_t)(-(int32_t)pid),b);put("-");put(b);}break;}put("\n");return;
    }last_spawn_pid=pid;put("spawn: PID=");{char b[12];dec(pid,b);put(b);}put("\n");
}
/* FIX60ZEE: command classes are explicit again.  `exec` is foreground EXE1;
   live observers that must coexist with an already-running MT service are
   started with `execmt` (active-session single-task append) or `spawn`. */
__attribute__((section(".usertext"))) static void exec_file(const char*name){uint32_t r;if(eq(name,"SONARVWR.EXE")){put("exec: SONARVWR live view uses MT; use execmt SONARVWR.EXE\n");return;}r=sys_exec(name);if(r==0)return;put("exec: FAIL ");if(r==0xffffffffu)put("-1\n");else{char b[12];dec(r,b);put(b);put("\n");}}
__attribute__((section(".usertext"))) static void jitter_info_command(void);
__attribute__((section(".usertext"))) static void do_syscall(char*p){uint32_t n,a,b,r;char out[12];skip(&p);n=number(&p);skip(&p);if(n==SYS_CONSOLE_WRITE){r=sys_console_write(p,sl(p));put("\nsyscall 1 -> ");dec(r,out);put(out);put(" bytes\n");return;}if(n==SYS_TIMER_GET){r=sys_timer_get();put("\nsyscall 3 -> ");dec(r,out);put(out);put(" ticks (50 Hz system)\n");return;}if(n==SYS_RT_TIME_GET){r=sys_rt_time_get();put("\nsyscall 45 -> ");dec(r,out);put(out);put(" ticks (100 Hz RT, 10 ms)\n");return;}if(n==SYS_RT_YIELD){r=sys_rt_yield();put("\nsyscall 49 -> ");put(r==0?"OK\n":"FAIL\n");return;}if(n==SYS_RT_JITTER_INFO){jitter_info_command();return;}if(n==SYS_MT_STOP){r=sys_mt_stop();put("\nsyscall 19 -> ");put(r==0?"OK\n":"FAIL\n");return;}if(n==SYS_MT_START){r=sys_mt_start();put("\nsyscall 18 -> ");put(r==0?"OK\n":"FAIL\n");return;}if(n==SYS_SCHED_BLOCK){r=sys_sched_block();put("\nsyscall 20 -> ");put(r==0?"OK\n":"FAIL\n");return;}if(n==SYS_SCHED_WAKE){if(!*p){put("\nsyscall 21: usage syscall 21 TASK_ID\n");return;}a=number(&p);r=sys_sched_wake(a);put("\nsyscall 21 -> ");put(r==0?"OK\n":"FAIL\n");return;}if(n==SYS_SCHED_EXIT){r=sys_sched_exit();put("\nsyscall 22 -> ");put(r==0?"OK\n":"FAIL\n");return;}if(n==SYS_CONSOLE_READ){put("syscall 2: type one character> ");{char x;put("\n");r=sys_console_read(&x,1);if(r==0xffffffffu){put("read=FAIL\n");return;}if(r==1){put("\nread=\"");putn(&x,1);put("\"\n");}else put("\nread=\"\n");}return;}if(n==SYS_PORT_OUT8){if(!*p){put("\nsyscall 13: usage syscall 13 PORT VALUE\n");return;}a=number(&p);skip(&p);if(!*p){put("\nsyscall 13: usage syscall 13 PORT VALUE\n");return;}b=number(&p);if(a>0xffffu||b>0xffu){put("\nsyscall 13: invalid port/value\n");return;}r=sys_port_out8(a,b);put("\nsyscall 13 -> ");put(r==0?"OK\n":"FAIL\n");return;}if(n==SYS_PORT_IN8){if(!*p){put("\nsyscall 14: usage syscall 14 PORT\n");return;}a=number(&p);if(a>0xffffu){put("\nsyscall 14: invalid port\n");return;}r=sys_port_in8(a);if(r==0xffffffffu){put("\nsyscall 14 -> FAIL\n");return;}hex8(r,out);put("\nsyscall 14 -> 0x");put(out);put(" (");dec(r,out);put(out);put(")\n");return;}if(n==SYS_QUEUE_STOP){r=sys_queue_stop();put("\nsyscall 16 -> ");put(r==0?"requested\n":"no active queue\n");return;}if(n==SYS_FILE_SIZE){skip(&p);if(!*p){put("\nsyscall 17: usage syscall 17 FILE\n");return;}r=sys_file_size(p);put("\nsyscall 17 -> ");if(r==0xffffffffu)put("FAIL\n");else{dec(r,out);put(out);put(" bytes\n");}return;}if(n==SYS_EXECMT){put("syscall 23 is intended for execmt; use: execmt FILE1 ... FILE25\n");return;}if(n==SYS_EXEC_QUEUE){put("syscall 15 is intended for execq; use: execq REPEAT FILE1 ... FILE25\n");return;}if(n==SYS_CHDIR){if(!*p){put("\nsyscall 29: usage syscall 29 PATH\n");return;}r=sys_chdir(p);put("\nsyscall 29 -> ");put(r==0?"OK\n":"FAIL\n");return;}if(n==SYS_GETCWD){char cwd[128];r=sys_getcwd(cwd,sizeof(cwd));put("\nsyscall 30 -> ");if(r==0xffffffffu)put("FAIL\n");else{put(cwd);put("\n");}return;}put("\nsyscall: unsupported number\n");}

/* v38: four independent Ring-3 tasks.  The scheduler demonstrates the full
   lifecycle CREATE -> READY -> RUNNING -> BLOCKED/STOPPED -> EXIT. */
__attribute__((section(".usertext"),noinline)) void mt_task_a(void){
    uint32_t r;
    volatile uint32_t isolation=0xa5a55a5au;
    put("\n[SCHED-A] RUNNING: wrote private marker; BLOCKED\n");
    r=sys_sched_block();
    (void)r;
    put("[SCHED-A] READY -> RUNNING after wake\n");
    if(isolation==0xa5a55a5au)put("[SCHED-A] ISOLATION PASS: A marker survived B switches\n");
    else put("[SCHED-A] ISOLATION FAIL: private state changed\n");
    put("[SCHED-A] EXIT\n");
    sys_sched_exit();
    for(;;){}
}
__attribute__((section(".usertext"),noinline)) void mt_task_b(void){
    volatile uint32_t isolation=0;
    put("[SCHED-B] RUNNING: check private stack, wake A, then BLOCKED\n");
    if(isolation==0xa5a55a5au)put("[SCHED-B] ISOLATION FAIL: saw A marker\n");
    else{isolation=0x5aa5a55au;put("[SCHED-B] ISOLATION PASS: A marker is invisible in B\n");}
    if(sys_sched_wake(0)==0)put("[SCHED-B] A -> READY\n");
    else put("[SCHED-B] wake A FAIL\n");
    sys_sched_block();
    put("[SCHED-B] resumed by C, EXIT\n");
    sys_sched_exit();
    for(;;){}
}
__attribute__((section(".usertext"),noinline)) void mt_task_c(void){
    put("[SCHED-C] RUNNING: wake B\n");
    if(sys_sched_wake(1)==0)put("[SCHED-C] B -> READY\n");
    else put("[SCHED-C] wake B FAIL\n");
    put("[SCHED-C] EXIT\n");
    sys_sched_exit();
    for(;;){}
}
__attribute__((section(".usertext"),noinline)) void mt_task_d(void){
    put("[SCHED-D] RUNNING: final independent task\n");
    put("[SCHED-D] EXIT\n");
    sys_sched_exit();
    for(;;){}
}
__attribute__((section(".usertext"))) static void mt_test(void){
    uint32_t r;
    put("\n=== v38 SCHEDULER LIFECYCLE TEST ===\n");
    put("4 Ring-3 tasks, separate CR3, round-robin IRQ0 scheduler.\n");
    put("Lifecycle: CREATE -> READY -> RUNNING -> BLOCKED -> READY -> RUNNING -> EXIT.\n");
    r=sys_mt_start();
    if(r==0)return;
    put("SCHED test: FAIL (scheduler busy)\n");
}

/* Full end-to-end syscall test. FIX53 chooses its scratch sector from the
   descriptor-defined protected gap, so the test never relies on a fixed FAT LBA or writes into the kernel growth reserve. */
__attribute__((section(".usertext"))) static uint32_t syscall_test(void){
    uint32_t pass=1,r,i,fd,n,disk_saved=0,file_opened=0,scratch=0u,layoutq[16];
    uint32_t t0,t1;
    char input[64];
    char file_data[64];
    char file_name[]="SCT17.TMP";
    uint8_t original[512];
    uint8_t work[512];

    put("\n=== SYSCALL TEST ===\n");
    if(sys_layout_info(layoutq)==0u)scratch=layoutq[4]+layoutq[6]+layoutq[7];

    put("[01] SYS_CONSOLE_WRITE: ");
    r=sys_console_write("console-write OK\n",17);
    if(r==17){put("PASS\n");}else{put("FAIL\n");pass=0;}

    put("[02] SYS_CONSOLE_READ: type OK then Enter: ");
    /* SYS_CONSOLE_READ is a character-read syscall: one successful call
       returns exactly one byte. Read O, K, and Enter separately. */
    /* FIX35A: EAX=2 is an internal RT/MT hand-off from SYS_CONSOLE_READ,
       not input.  Keep waiting for the real byte, exactly as FIX32 VGADRV
       already does. */
    do{r=sys_console_read(&input[0],1);}while(r==2u);
    if(r!=1){put("FAIL (read O)\n");pass=0;}
    do{r=sys_console_read(&input[1],1);}while(r==2u);
    if(r!=1){put("FAIL (read K)\n");pass=0;}
    do{r=sys_console_read(&input[2],1);}while(r==2u);
    if(r!=1){put("FAIL (read Enter)\n");pass=0;}
    input[3]=0;
    if(input[0]=='O'&&input[1]=='K'&&input[2]=='\n'){put("PASS\n");}
    else{put("FAIL (expected OK + Enter)\n");pass=0;}

    put("[03] SYS_TIMER_GET: ");
    t0=sys_timer_get();
    for(i=0;i<10000u;i++)__asm__ volatile("nop");
    t1=sys_timer_get();
    if(t1>=t0){char b[12];dec(t1,b);put(b);put(" PASS\n");}
    else{put("FAIL\n");pass=0;}

    put("[04] SYS_DISK_READ: ");
    r=sys_disk_read(scratch,original,1);
    if(r==0){put("PASS\n");disk_saved=1;}else{put("FAIL\n");pass=0;}

    put("[05] SYS_DISK_WRITE: ");
    if(!disk_saved){put("SKIP (no safe backup)\n");pass=0;}
    else{
        for(i=0;i<512u;i++)work[i]=(uint8_t)(0xa5u^(uint8_t)i);
        r=sys_disk_write(scratch,work,1);
        if(r!=0){put("FAIL (write)\n");pass=0;}
        else if(sys_disk_read(scratch,work,1)!=0){put("FAIL (readback)\n");pass=0;}
        else{for(i=0;i<512u;i++)if(work[i]!=(uint8_t)(0xa5u^(uint8_t)i))break;if(i!=512u){put("FAIL (compare)\n");pass=0;}else put("PASS\n");}
        if(sys_disk_write(scratch,original,1)!=0){put("[05] restore: FAIL\n");pass=0;}
        else if(sys_disk_read(scratch,work,1)!=0){put("[05] restore-check: FAIL\n");pass=0;}
        else{for(i=0;i<512u;i++)if(work[i]!=original[i])break;if(i!=512u){put("[05] restore-check: FAIL\n");pass=0;}else put("[05] restore: PASS\n");}
    }

    put("[06] SYS_FILE_OPEN: ");
    fd=sys_file_open(file_name,FAT16_MODE_WRITE|FAT16_MODE_CREATE|FAT16_MODE_TRUNC);
    if(fd==0xffffffffu){put("FAIL\n");pass=0;}
    else{file_opened=1;put("PASS\n");
        put("[08] SYS_FILE_WRITE: ");
        file_data[0]='s';file_data[1]='y';file_data[2]='s';file_data[3]='c';file_data[4]='a';file_data[5]='l';file_data[6]='l';file_data[7]='-';file_data[8]='t';file_data[9]='e';file_data[10]='s';file_data[11]='t';file_data[12]=0;
        n=sys_file_write(fd,file_data,12);
        if(n==12){put("PASS\n");}else{put("FAIL\n");pass=0;}
        put("[09] SYS_FILE_CLOSE: ");
        r=sys_file_close(fd);if(r==0){put("PASS\n");}else{put("FAIL\n");pass=0;}
        put("[06] SYS_FILE_OPEN(read): ");
        fd=sys_file_open(file_name,FAT16_MODE_READ);
        if(fd==0xffffffffu){put("FAIL\n");pass=0;}
        else{put("PASS\n");
            put("[07] SYS_FILE_READ: ");
            n=sys_file_read(fd,work,12);
            if(n==12){for(i=0;i<12u;i++)if(work[i]!=(uint8_t)file_data[i])break;if(i==12u)put("PASS\n");else{put("FAIL (compare)\n");pass=0;}}else{put("FAIL\n");pass=0;}
            put("[09] SYS_FILE_CLOSE(read): ");
            r=sys_file_close(fd);if(r==0){put("PASS\n");}else{put("FAIL\n");pass=0;}
        }
    }
    put("[17] SYS_FILE_SIZE: ");
    r=sys_file_size(file_name);
    if(r==12u){put("PASS (12 bytes)\n");}else{put("FAIL\n");pass=0;}
    put("[17] SYS_FILE_SIZE missing-file guard: ");
    r=sys_file_size("NOFILE.TXT");
    if(r==0xffffffffu){put("PASS\n");}else{put("FAIL\n");pass=0;}
    put("[10] SYS_FILE_DELETE: ");
    if(file_opened){r=sys_file_delete(file_name);if(r==0){put("PASS\n");}else{put("FAIL\n");pass=0;}}
    else{put("SKIP\n");}

    put("[13] SYS_PORT_OUT8 validation: ");
    r=sys_port_out8(0x10000u,0u);
    if(r==0xffffffffu){put("PASS\n");}else{put("FAIL\n");pass=0;}
    put("[14] SYS_PORT_IN8 validation: ");
    r=sys_port_in8(0x10000u);
    if(r==0xffffffffu){put("PASS\n");}else{put("FAIL\n");pass=0;}

    put("[15] SYS_EXEC_QUEUE x2: ");
    {
        /* SYS_EXEC_QUEUE accepts a packed 13-byte slot, not a C string of
           arbitrary length.  Keep the test buffer writable so it also tests
           the same ABI used by the execq shell command. */
        char queue_test_names[13];
        queue_test_names[0]='H';queue_test_names[1]='E';queue_test_names[2]='L';queue_test_names[3]='L';
        queue_test_names[4]='O';queue_test_names[5]='.';queue_test_names[6]='E';queue_test_names[7]='X';
        queue_test_names[8]='E';queue_test_names[9]=0;queue_test_names[10]=0;queue_test_names[11]=0;queue_test_names[12]=0;
        r=sys_exec_queue(queue_test_names,1,2);
    }
    if(r==0){put("PASS\n");}else{put("FAIL\n");pass=0;}
    put("[16] SYS_QUEUE_STOP shell guard: ");
    r=sys_queue_stop();
    if(r==0xffffffffu){put("PASS (no active queue)\n");}else{put("FAIL\n");pass=0;}

    put("[12] SYS_EXIT shell guard: ");
    r=syscall3(SYS_EXIT,0,0,0);
    if(r==0xffffffffu){put("PASS (rejected outside EXE)\n");}else{put("FAIL\n");pass=0;}

    put("[11] SYS_EXEC + [12] SYS_EXIT: ");
    r=sys_exec("/HELLO.EXE");
    if(r==0){put("PASS\n");}else{put("FAIL\n");pass=0;}

    put(pass?"=== ALL TESTS PASS ===\n":"=== TESTS FAILED ===\n");
    return pass;
}
__attribute__((section(".usertext"))) static void exec_queue_command(char*p,uint32_t forever){
    char names[25*13];uint32_t count=0,reps=0,n;char*b;char*start=p;
    skip(&p);
    if(forever)reps=0;else{reps=number(&p);if(reps<1u||reps>QUEUE_MAX_REPS){put("execq: repetitions must be 1..3000\n");return;}}
    skip(&p);
    /* Специальный COMDRV получает три аргумента через расширенный ABI.
       Передаём ему исходную позицию после "execq ", чтобы helper сам
       разобрал REPEAT COMDRV.EXE PORT DIRECTION FILE. */
    if(!forever && (prefix(p,"COMDRV.EXE ") || eq(p,"COMDRV.EXE"))){
        exec_queue_com_command(start,0);return;
    }
    while(*p&&count<QUEUE_MAX_FILES){
        b=&names[count*13u];n=0;
        while(*p&&*p!=' '&&*p!='\t'&&n<12u)b[n++]=*p++;
        if(n==0){skip(&p);continue;}
        if(n>=13u){put("execq: filename too long\n");return;}
        b[n]=0;count++;skip(&p);
    }
    if(*p){put("execq: maximum is 25 EXE1 files\n");return;}
    if(count==0){put(forever?"usage: execq forever FILE1 [FILE2 ... FILE25]\n":"usage: execq REPEAT FILE1 [FILE2 ... FILE25]\n");return;}
    put(forever?"execq: infinite circular queue started; press Esc or use SYS_QUEUE_STOP from an EXE\n":"execq: circular queue started\n");
    n=sys_exec_queue(names,count,reps);
    if(n==0)put("execq: finished\n");
    else if(n==0xffffffffu)put("execq: stopped or failed\n");
    else{char out[12];dec(n,out);put("execq: FAIL ");put(out);put("\n");}
}

__attribute__((section(".usertext"))) static void execmt_command(char*p){
    char names[EXECMT_MAX_TASKS*13u];uint32_t count=0,n,r;char*b;
    skip(&p);
    while(*p&&count<EXECMT_MAX_TASKS){
        b=&names[count*13u];n=0;
        while(*p&&*p!=' '&&*p!='\t'&&n<12u)b[n++]=*p++;
        if(n==0){skip(&p);continue;}
        if(n>=13u){put("execmt: filename too long\n");return;}
        b[n]=0;count++;skip(&p);
    }
    if(*p){put("execmt: maximum is 25 EXE1 tasks\n");return;}
    if(count==0){put("usage: execmt FILE1.EXE [FILE2.EXE ... FILE25.EXE]\n");return;}
    put("execmt: loading ");{char out[12];dec(count,out);put(out);}put(" independent tasks...\n");
    r=sys_execmt(names,count);
    if(r==0){put("execmt: started in background; quantum=20 ms\n");return;}
    if(r==SAFE_POLICY_DENIED){put("execmt: DENIED by SAFE policy\n");return;}
    /* FIX60ZEE: SYS_EXECMT intentionally creates a new MT session and returns
       BUSY when one is already active.  For one executable, make the shell
       command useful for the natural live-observer case by appending it via
       the established PROCESS_SPAWN path.  Multiple-file session creation
       keeps the old transactional semantics. */
    if(r==0xffffffffu&&count==1u){
        struct process_spawn_request q;uint32_t i,pid;
        for(i=0u;i<sizeof(q);i++)((char*)&q)[i]=0;
        for(i=0u;i<12u&&names[i];i++)q.name[i]=names[i];
        pid=sys_process_spawn(&q);
        if(pid==SAFE_POLICY_DENIED){put("execmt: DENIED by SAFE policy\n");return;}
        if((int32_t)pid<0){put("execmt: active-session append FAIL\n");return;}
        last_spawn_pid=pid;put("execmt: appended MT task PID=");{char out[12];dec(pid,out);put(out);}put("\n");return;
    }
    if(r==0xffffffffu){put("execmt: session active; append one task at a time or use mtstop ALL\n");return;}
    {char out[12];dec(r,out);put("execmt: FAIL ");put(out);put("\n");}
}

__attribute__((section(".usertext"))) static void pit_test(void){uint32_t t0,t1,loops=0;char b[12];t0=sys_timer_get();put("PIT50 test: waiting for 3 timer ticks...\n");while((t1=sys_timer_get())-t0<3u){if(++loops==0xffffffffu){put("PIT50 test: FAIL (counter stalled)\n");return;}}put("PIT50 test: PASS (ticks ");dec(t0,b);put(b);put(" -> ");dec(t1,b);put(b);put(")\n");}

/* v64: разбор команды cp. Пути передаются в ядро без локальной подмены cwd,
 * поэтому абсолютные и относительные пути используют единый path-модуль. */
__attribute__((section(".usertext"))) static void copy_file_command(char*p){
    char src[128],dst[128];
    token(&p,src,sizeof(src));token(&p,dst,sizeof(dst));skip(&p);
    if(!src[0]||!dst[0]||*p){put("usage: cp SOURCE DEST\n");return;}
    if(sys_file_copy(src,dst)==0)put("cp: OK\n");else put("cp: FAIL\n");
}
/* Stage 6.2 executable runtime test helper.  It is intentionally an ordinary
   shell command wrapper around the same RT diagnostic syscall used by
   JITTER.EXE, so command spelling remains covered by the global ASCII-fold
   parser. */
__attribute__((section(".usertext"))) static void jitter_info_command(void){
    uint32_t v[13],r,i;
    r=sys_rt_jitter_info(v);
    if(r!=0u){put("rt-jitter: FAIL\n");return;}
    put("rt-jitter: job=");{char b[12];dec(v[0],b);put(b);}put(" period_ticks=");{char b[12];dec(v[1],b);put(b);}
    put(" release=");{char b[12];dec(v[2],b);put(b);}put(" observed=");{char b[12];dec(v[3],b);put(b);}
    put(" interval=");{char b[12];dec(v[4],b);put(b);}put(" min=");{char b[12];dec(v[5],b);put(b);}
    put(" max=");{char b[12];dec(v[6],b);put(b);}put(" late=");{char b[12];dec(v[7],b);put(b);}
    put(" maxlate=");{char b[12];dec(v[8],b);put(b);}put(" dispatch=");{char b[12];dec(v[9],b);put(b);}
    put(" dispatch_latency=");{char b[12];dec(v[10],b);put(b);}put(" dispatched=");{char b[12];dec(v[11],b);put(b);}
    put(" samples=");{char b[12];dec(v[12],b);put(b);}put("\n");
    (void)i;
}


__attribute__((section(".usertext"))) static void mtlist_command(void){
    uint32_t v[25u*8u],i,j,shown=0u;char b[16],name[17];
    /* SYS_MT_STATUS remains the kernel ABI snapshot provider.  MTLIST is
       deliberately a simple user view: only READY/RUNNING/BLOCKED tasks are
       alive.  STOPPED/EXIT/CREATE entries are never printed. */
    if(sys_mt_status(v)!=0u){put("mtlist: FAIL\n");return;}
    for(i=0;i<25u;i++){
        uint32_t*q=v+i*8u;
        if(q[1]!=1u&&q[1]!=2u&&q[1]!=3u)continue;
        shown=1u;
        put("MT");dec(i+1u,b);put(b);put(" ");
        for(j=0;j<16u;j++)name[j]=(char)((q[4u+j/4u]>>((j%4u)*8u))&255u);
        name[16]=0;put(name);put("\n");
    }
    if(!shown)put("MT: no tasks\n");
}
__attribute__((section(".usertext"))) static void mtstat_name(uint32_t*q,char*out){uint32_t i;for(i=0u;i<16u;i++)out[i]=(char)((q[6u+i/4u]>>((i%4u)*8u))&255u);out[16]=0;}
__attribute__((section(".usertext"))) static const char*mtstat_state(uint32_t s){if(s==1u)return "READY";if(s==2u)return "RUN";if(s==3u)return "BLOCK";if(s==4u)return "STOP";if(s==5u)return "EXIT";return "CREATE";}
__attribute__((section(".usertext"))) static void mtstat_col(const char*s,uint32_t w){uint32_t n=sl(s);put(s);while(n++<w)put(" ");}
__attribute__((section(".usertext"))) static void mtstat_num(uint32_t v,uint32_t w){char b[16];dec(v,b);mtstat_col(b,w);}
__attribute__((section(".usertext"))) static uint32_t mtstat_snapshot(uint32_t*v){return sys_mt_stats(v);}
/* FIX30: MTSTAT and MTSTAT WATCH intentionally use the same compact view.
   With 25 active tasks the old one-shot table also exceeded the 25-row VGA
   console.  Keep all statistics and the SYS_MT_STATS ABI unchanged; only the
   shell presentation is compacted to two MT records per line. */

__attribute__((section(".usertext"))) static uint32_t sys_process_info(uint32_t*out){return syscall3(SYS_PROCESS_INFO,(uint32_t)out,0u,0u);}
__attribute__((section(".usertext"))) static uint32_t sys_process_fault(uint32_t*out,uint32_t pid){return syscall3(SYS_PROCESS_INFO,(uint32_t)out,2u,pid);}
__attribute__((section(".usertext"))) static const char*ps_state(uint32_t s){if(s==1u)return "READY";if(s==2u)return "RUNNING";if(s==3u)return "BLOCKED";if(s==4u||s==5u)return "EXITED";return "FREE";}

__attribute__((section(".usertext"))) static uint32_t sys_process_wait(uint32_t*out,uint32_t pid){return syscall3(SYS_PROCESS_WAIT,(uint32_t)out,pid,0u);}
__attribute__((section(".usertext"))) static void wait_command(char*p){uint32_t q[8],pid,r;char b[16];skip(&p);pid=number(&p);skip(&p);if(*p||pid==0u){put("usage: wait PID\n");return;}put("WAIT: ESC to cancel\n");for(;;){uint8_t key=0u;if(sys_console_poll((void*)&key,1u)==0x1bu&&key==0x1bu){(void)sys_process_wait((uint32_t*)0,0u);put("wait: cancelled\n");return;}r=sys_process_wait(q,pid);if(r==1u)break;if(r==0xfffffffdu){put("wait: PID not found or result expired\n");return;}if(r==0xffffffffu){put("wait: FAIL\n");return;}if(r!=0u){put("wait: FAIL\n");return;}}test_last_wait_valid=1u;test_last_wait_reason=q[2];test_last_wait_status=q[3];put("PID: ");dec(q[0],b);put(b);put(" TYPE: ");put(q[1]==1u?"FG":(q[1]==2u?"MT":"RT"));put(" EXIT: ");put(q[2]==2u?"FAULT":(q[2]==1u?"NORMAL":(q[2]==3u?"WATCHDOG":(q[2]==4u?"STOPPED":"NONE"))));put(" STATUS: ");if(q[3]==0xffffffffu)put("-");else{dec(q[3],b);put(b);}if(q[2]==2u){put(" VECTOR: ");dec(q[4],b);put(b);put(" ERROR: ");dec(q[5],b);put(b);put(" EIP: ");dec(q[6],b);put(b);put(" CR2: ");dec(q[7],b);put(b);}put("\n");}
__attribute__((section(".usertext"))) static void ps_command(void){uint32_t *v=shell_process_snapshot,n,i,j;char b[16],name[17];n=sys_process_info(v);if(n==0xffffffffu){put("ps: FAIL\n");return;}put("PID TYPE STATE SLOT NAME\n");for(i=0u;i<n&&i<34u;i++){uint32_t*q=v+i*10u;if(!q[0])continue;dec(q[0],b);put(b);put(" ");put(q[1]==1u?"FG ":(q[1]==2u?"MT ":"RT "));put(ps_state(q[2]));put(" ");if(q[3]==0xffffffffu)put("- ");else{put(q[1]==2u?"MT":"SLOT");dec(q[3]+1u,b);put(b);put(" ");}for(j=0u;j<16u;j++)name[j]=(char)((q[6u+j/4u]>>((j%4u)*8u))&0xffu);name[16]=0;put(name);if(q[2]==4u||q[2]==5u){uint32_t d[9];if(sys_process_fault(d,q[0])==1u&&d[4]==2u)put(" FAULT");}put("\n");}}
__attribute__((section(".usertext"))) static void ps_pid_command(char*p){uint32_t q[9],pid,r;char b[16];skip(&p);pid=number(&p);skip(&p);if(*p||pid==0u){put("usage: ps PID\n");return;}r=sys_process_fault(q,pid);if(r==0xffffffffu){put("ps: FAIL\n");return;}if(r==0u){put("ps: PID not found\n");return;}put("PID: ");dec(q[0],b);put(b);put("\nTYPE: ");put(q[1]==1u?"FG":(q[1]==2u?"MT":"RT"));put("\nSTATE: ");put(ps_state(q[2]));put("\nSLOT: ");if(q[3]==0xffffffffu)put("-");else{put(q[1]==2u?"MT":"SLOT");dec(q[3]+1u,b);put(b);}put("\nEXIT: ");put(q[4]==2u?"FAULT":(q[4]==1u?"NORMAL":"NONE"));put("\nVECTOR: ");dec(q[5],b);put(b);put("\nERROR: ");dec(q[6],b);put(b);put("\nEIP: ");dec(q[7],b);put(b);put("\nCR2: ");dec(q[8],b);put(b);put("\n");}
__attribute__((section(".usertext"))) static void mtstat_watch_one(uint32_t*q,uint32_t slot){char b[16];put("MT");dec(slot+1u,b);mtstat_col(b,3u);put(" D=");dec(q[2],b);mtstat_col(b,7u);put(" Q=");dec(q[3],b);mtstat_col(b,7u);put(" C=");dec(q[4],b);mtstat_col(b,7u);}
__attribute__((section(".usertext"))) static void mtstat_watch_print(uint32_t*v){uint32_t i,n=0u;put("MTSTAT WATCH: D=dispatch Q=quanta C=cpu_ticks\n");for(i=0u;i<25u;i++){uint32_t*q=v+i*10u;if(q[1]!=1u&&q[1]!=2u&&q[1]!=3u&&q[2]==0u&&q[4]==0u)continue;mtstat_watch_one(q,i);n++;if((n&1u)==0u)put("\n");else put("  |  ");}if(n&1u)put("\n");if(!n)put("MT: no tasks/statistics\n");}
__attribute__((section(".usertext"))) static void mtstat_command(char*p){uint32_t *v=shell_mtstat_snapshot;skip(&p);if(*p==0){if(mtstat_snapshot(v)!=0u)put("mtstat: FAIL\n");else{put("MTSTAT: D=dispatch Q=quanta C=cpu_ticks\n");mtstat_watch_print(v);}return;}if(eq(p,"watch")){uint32_t next_rt;put("MTSTAT WATCH: ESC to stop watch; refresh=1s\n");/* Reuse the FIX21 foreground-watch ESC routing while RT tasks are active. */sys_rt_stats((void*)1,6u);next_rt=sys_rt_time_get()+100u;for(;;){uint8_t key=0u;uint32_t cur=sys_rt_time_get();if(sys_console_poll((void*)&key,1u)==0x1bu&&key==0x1bu)break;cur=sys_rt_time_get();if((uint32_t)(cur-(next_rt-100u))>=100u){next_rt=cur+100u;clear();put("MTSTAT WATCH tick=");{char b[16];dec(cur,b);put(b);}put("\n");if(mtstat_snapshot(v)!=0u){put("mtstat: FAIL\n");break;}mtstat_watch_print(v);}/* Yield only to the established RT mechanism. Normal PIT scheduling remains responsible for MT. */if(sys_rt_yield()==0xffffffffu)break;}sys_rt_stats((void*)0,6u);return;}put("usage: mtstat [watch]\n");}
__attribute__((section(".usertext"))) static void mtdata_command(char*p){
    uint32_t id,r,seq,session;char data[256];skip(&p);
    if(!prefix(p,"MT")){put("usage: mtdata MT1..MT25\n");return;}p+=2;id=number(&p);skip(&p);
    if(*p||id<1u||id>25u){put("usage: mtdata MT1..MT25\n");return;}
    r=sys_mt_data_read(data,id-1u,&seq,&session);(void)seq;(void)session;
    if(r==0xfffffffeu){put("MTDATA: no data\n");return;}
    if(r==0xfffffffdu){put("MTDATA: no data\n");return;}
    if(r==0xffffffffu){put("mtdata: FAIL\n");return;}
    put(data);put("\n");
}
__attribute__((section(".usertext"))) static void mtstop_command(char*p){uint32_t id,r;char b[12];skip(&p);if(eq(p,"ALL")){r=sys_mt_stop();put(r==0u?"mtstop: all stopped\n":"mtstop: no active queue\n");return;}if(!prefix(p,"MT")){put("usage: mtstop MT1..MT25|ALL\n");return;}p+=2;id=number(&p);skip(&p);if(*p||id<1u||id>25u){put("usage: mtstop MT1..MT25|ALL\n");return;}r=sys_mt_stop_one(id-1u);if(r!=0u){put("mtstop: task not active\n");return;}put("mtstop: MT");dec(id,b);put(b);put(" stopped\n");}

__attribute__((section(".usertext"))) static void sdec(int32_t v,char*b){if(v<0){*b++='-';dec((uint32_t)(-(v+1))+1u,b);}else dec((uint32_t)v,b);}
__attribute__((section(".usertext"))) static void rtstat_name(uint32_t*q,char*out){uint32_t i,w;for(i=0u;i<13u;i++){w=q[4u+i/4u];out[i]=(char)((w>>((i%4u)*8u))&255u);}out[12]=0;}
__attribute__((section(".usertext"))) static void rtstat_avg(uint32_t sum,uint32_t n,char*out){if(!n){out[0]='-';out[1]=0;}else dec(sum/n,out);}
__attribute__((section(".usertext"))) static void rtstat_savg(int32_t sum,uint32_t n,char*out){if(!n){out[0]='-';out[1]=0;}else sdec(sum/(int32_t)n,out);}
__attribute__((section(".usertext"))) static void rtstat_col(const char*s,uint32_t w){uint32_t n=sl(s);put(s);while(n++<w)put(" ");}
__attribute__((section(".usertext"))) static void rtstat_num(uint32_t v,uint32_t w){char b[16];dec(v,b);rtstat_col(b,w);}
__attribute__((section(".usertext"))) static void rtstat_snum(int32_t v,uint32_t w){char b[16];sdec(v,b);rtstat_col(b,w);}
__attribute__((section(".usertext"))) static void rtstat_head(void){put("SLOT TASK         JOBS   INT  JIT  DSP  CPU  WALL MISS SKIP\n");put("---- ------------ ------ ---- ---- ---- ---- ---- ---- ----\n");}
__attribute__((section(".usertext"))) static void rtstat_row(uint32_t*q,uint32_t slot){char name[13],b[16];uint32_t n=q[8];put("S");dec(slot+1u,b);rtstat_col(b,3u);rtstat_name(q,name);rtstat_col(name,13u);rtstat_num(n,7u);rtstat_avg(q[9],n,b);rtstat_col(b,5u);rtstat_savg((int32_t)q[12],n,b);rtstat_col(b,5u);rtstat_avg(q[15],n,b);rtstat_col(b,5u);rtstat_avg(q[18],n,b);rtstat_col(b,5u);rtstat_avg(q[21],n,b);rtstat_col(b,5u);rtstat_num(q[24],5u);rtstat_num(q[25],4u);put("\n");}
__attribute__((section(".usertext"))) static uint32_t rtstat_snapshot(uint32_t*v){return sys_rt_stats(v,0u);}
__attribute__((section(".usertext"))) static void rtstat_print(uint32_t*v,uint32_t mode){uint32_t i;if(mode==2u)put("RTSTAT WATCH  (6h window; time: 10-ms ticks)\n");else if(mode==1u)put("RTSTAT COMPARE  (6h window; time: 10-ms ticks)\n");else put("RTSTAT  (6h window; time: 10-ms ticks)\n");rtstat_head();for(i=0u;i<8u;i++){uint32_t*q=v+i*30u;if(q[8]||q[1]!=0u)rtstat_row(q,i);}}
__attribute__((section(".usertext"))) static void rtstat_dump(void){uint32_t *v=shell_rtstat_snapshot,*r=shell_rtdump_snapshot,i,j,c;char name[13],b[16];if(rtstat_snapshot(v)!=0u){put("rtstat: FAIL\n");return;}put("RTSTAT DUMP  (last completed jobs; time: 10-ms ticks)\n");for(i=0u;i<8u;i++){uint32_t*q=v+i*30u;if(!q[8])continue;rtstat_name(q,name);put("S");dec(i+1u,b);put(b);put(" ");put(name);put("\n");put("SEQ    INT  JIT  DSP  CPU  WALL MISS SKIP\n");put("------ ---- ---- ---- ---- ---- ---- ----\n");if(syscall3(SYS_RT_STATS,(uint32_t)r,2u,i)!=0u){put("dump: FAIL\n");continue;}c=r[0];for(j=0u;j<c;j++){uint32_t*x=r+1u+j*8u;rtstat_num(x[0],7u);rtstat_num(x[1],5u);rtstat_snum((int32_t)x[2],5u);rtstat_num(x[3],5u);rtstat_num(x[4],5u);rtstat_num(x[5],5u);rtstat_num(x[6],5u);rtstat_num(x[7],4u);put("\n");}}}
__attribute__((section(".usertext"))) static uint32_t rtstat_sensor_slot(const char*s){
    /* v67.11-B-FIX: STOP addresses the RT slot, not the executable name.
       SLOT1..SLOT8 are the eight RT manager slots and are deliberately parsed
       with eq(), so command/parameter spelling remains case-insensitive. */
    if(eq(s,"slot1"))return 0u;
    if(eq(s,"slot2"))return 1u;
    if(eq(s,"slot3"))return 2u;
    if(eq(s,"slot4"))return 3u;
    if(eq(s,"slot5"))return 4u;
    if(eq(s,"slot6"))return 5u;
    if(eq(s,"slot7"))return 6u;
    if(eq(s,"slot8"))return 7u;
    return RT_STOP_ALL;
}
__attribute__((section(".usertext"))) static void rtstat_stop_command(char*p){
    uint32_t target,n;
    skip(&p);
    if(*p==0){target=RT_STOP_ALL;}
    else{
        char name[16];uint32_t i=0u;
        while(*p&&*p!=' '&&*p!='\t'&&i+1u<sizeof(name))name[i++]=*p++;
        name[i]=0;skip(&p);
        if(*p||i==0u){put("usage: rtstat stop [SLOT1..SLOT8]\n");return;}
        target=rtstat_sensor_slot(name);
        if(target==RT_STOP_ALL){put("rtstat stop: unknown task\n");return;}
    }
    n=sys_rt_stop(target);
    if(n==0xffffffffu){put("rtstat stop: FAIL\n");return;}
    if(target==RT_STOP_ALL){put("rtstat stop: stopped ");}
    else{put("rtstat stop: ");{char b[12];dec(target+1u,b);put("SLOT");put(b);}put(" stopped\n");return;}
    {char b[12];dec(n,b);put(b);}put(" task(s)\n");
}
__attribute__((section(".usertext"))) static void rtdata_command(char*p){
    char name[16],data[256];uint32_t i=0u,slot,r;
    skip(&p);while(*p&&*p!=' '&&*p!='\t'&&i+1u<sizeof(name))name[i++]=*p++;name[i]=0;skip(&p);
    if(*p||i==0u){put("usage: rtdata SLOT1..SLOT8\n");return;}
    slot=rtstat_sensor_slot(name);if(slot==RT_STOP_ALL){put("rtdata: unknown slot\n");return;}
    r=sys_rt_data_read(data,slot);
    if(r==0xfffffffeu){put("RTDATA: slot not active\n");return;}
    if(r==0xfffffffdu){put("RTDATA: no data\n");return;}
    if(r==0xffffffffu){put("rtdata: FAIL\n");return;}
    data[255]=0;put(data);put("\n");
}
__attribute__((section(".usertext"))) static void rtstat_events_slot(uint32_t slot){uint32_t r[83],j,c;char b[16];if(syscall3(SYS_RT_STATS,(uint32_t)r,5u,slot)!=0u){put("events: FAIL\n");return;}put("SLOT");dec(slot+1u,b);put(b);put(" LIFETIME MISS=");dec(r[1],b);put(b);put(" SKIP=");dec(r[2],b);put(b);put("\n");c=r[0];if(!c){put("  no anomalies\n");return;}put("  TICK       SEQ    MISS SKIP LATE\n");for(j=0u;j<c;j++){uint32_t*x=r+3u+j*5u;put("  ");rtstat_num(x[0],11u);rtstat_num(x[1],7u);rtstat_num(x[2],5u);rtstat_num(x[3],5u);rtstat_num(x[4],4u);put("\n");}}
__attribute__((section(".usertext"))) static void rtstat_events_command(char*p){uint32_t i,slot;char name[16];skip(&p);if(!*p){for(i=0u;i<8u;i++)rtstat_events_slot(i);return;}i=0u;while(*p&&*p!=' '&&*p!='\t'&&i+1u<sizeof(name))name[i++]=*p++;name[i]=0;skip(&p);if(*p){put("usage: rtstat events [SLOT1..SLOT8]\n");return;}slot=rtstat_sensor_slot(name);if(slot==RT_STOP_ALL){put("rtstat events: unknown slot\n");return;}rtstat_events_slot(slot);}
__attribute__((section(".usertext"))) static void rtstat_command(char*p){uint32_t *v=shell_rtstat_snapshot;skip(&p);if(eq(p,"reset")){if(sys_rt_stats((void*)0,1u)==0u)put("rtstat: reset OK\n");else put("rtstat: reset FAIL\n");return;}if(prefix(p,"stop")){char*q=p+4;if(*q==0||*q==' '||*q=='\t'){rtstat_stop_command(q);return;}}if(eq(p,"compare")){if(rtstat_snapshot(v)!=0u)put("rtstat: FAIL\n");else rtstat_print(v,1u);return;}if(eq(p,"dump")){rtstat_dump();return;}if(prefix(p,"events")){char*q=p+6;if(*q==0||*q==' '||*q=='\t'){rtstat_events_command(q);return;}}if(eq(p,"watch")){uint32_t next_rt;put("RTSTAT WATCH: ESC to stop watch; refresh=1s; RT tasks continue\n");sys_rt_stats((void*)1,6u);next_rt=sys_rt_time_get()+100u;for(;;){uint8_t key=0u;uint32_t cur=sys_rt_time_get();if(sys_console_poll((void*)&key,1u)==0x1bu&&key==0x1bu)break;cur=sys_rt_time_get();if((uint32_t)(cur-(next_rt-100u))>=100u){next_rt=cur+100u;clear();put("RTSTAT WATCH tick=");{char b[16];dec(cur,b);put(b);}put("\n");if(rtstat_snapshot(v)!=0u){put("rtstat: FAIL\n");break;}rtstat_print(v,2u);}/* WATCH is a foreground shell loop.  A pure RT clock read/poll loop would keep the shell runnable and starve detached RT jobs.  Yield once per pass so the kernel can dispatch a released RT job; if none is ready the kernel sleeps for one RT tick. */sys_rt_yield();}sys_rt_stats((void*)0,6u);return;}if(*p==0){if(rtstat_snapshot(v)!=0u)put("rtstat: FAIL\n");else rtstat_print(v,0u);return;}put("usage: rtstat [stop [SLOT1..SLOT8]|compare|watch|dump|events [SLOT1..SLOT8]|reset]\n");}

__attribute__((section(".usertext"))) static void supstart_command(char*p){char cmd[96];uint32_t n=0;skip(&p);if(!*p){put("usage: supstart FILE.EXE\n");return;}while("SUPERVIS.EXE "[n]){cmd[n]="SUPERVIS.EXE "[n];n++;}while(*p&&n+1u<sizeof(cmd))cmd[n++]=*p++;cmd[n]=0;spawn_command(cmd);}
__attribute__((section(".usertext"))) static void supstat_command(void){uint32_t*v=shell_process_snapshot,n,i;char data[256];n=sys_process_info(v);if(n==0xffffffffu){put("supstat: FAIL\n");return;}for(i=0;i<n&&i<34u;i++){uint32_t*q=v+i*10u;char name[17];uint32_t j,k;for(j=0;j<4u;j++)for(k=0;k<4u;k++)name[j*4u+k]=(char)((q[6u+j]>>(k*8u))&0xffu);name[16]=0;if(q[1]==2u&&eq(name,"SUPERVIS.EXE")){uint32_t seq=0,session=0,r=sys_mt_data_read(data,q[3],&seq,&session);put("SUPERVIS MT");{char b[12];dec(q[3]+1u,b);put(b);}put(" PID=");{char b[12];dec(q[0],b);put(b);}put(" ");if(r==0xfffffffeu||r==0xfffffffdu||r==0xffffffffu)put("NO DATA");else{data[255]=0;put(data);}put("\n");return;}}put("SUPERVIS: not found\n");}
__attribute__((section(".usertext"))) static uint32_t event_disk_checksum(const uint32_t*w,uint32_t words){uint32_t i,h=2166136261u;for(i=0u;i<words;i++){h^=w[i];h*=16777619u;}return h;}
__attribute__((section(".usertext"))) static uint32_t event_disk_load(uint32_t**entries,uint32_t*count){uint32_t fd,bytes,want,got,c;fd=sys_file_open(EVENT_DISK_NAME,FAT16_MODE_READ);if(fd==0xffffffffu)return 0u;bytes=sys_file_size(EVENT_DISK_NAME);if(bytes<EVENT_DISK_HEADER_WORDS*4u||bytes>EVENT_DISK_MAX_WORDS*4u||(bytes&3u)){sys_file_close(fd);return 0u;}got=sys_file_read(fd,shell_event_disk,bytes);sys_file_close(fd);if(got!=bytes)return 0u;if(shell_event_disk[0]!=EVENT_DISK_MAGIC||shell_event_disk[1]!=EVENT_DISK_VERSION)return 0u;c=shell_event_disk[2];if(c>64u)return 0u;want=(EVENT_DISK_HEADER_WORDS+c*8u)*4u;if(bytes!=want)return 0u;if(shell_event_disk[4]!=event_disk_checksum(shell_event_disk+EVENT_DISK_HEADER_WORDS,c*8u))return 0u;*entries=shell_event_disk+EVENT_DISK_HEADER_WORDS;*count=c;return 1u;}
__attribute__((section(".usertext"))) static uint32_t event_disk_save(void){uint32_t n,fd,bytes,written,c,*v=shell_event_snapshot,*d=shell_event_disk;n=sys_event_log(v,0u);if(n==0xffffffffu)return 0u;c=v[0];if(c>64u)return 0u;d[0]=EVENT_DISK_MAGIC;d[1]=EVENT_DISK_VERSION;d[2]=c;d[3]=c?v[1u+(c-1u)*8u]:0u;{uint32_t i;for(i=0u;i<c*8u;i++)d[EVENT_DISK_HEADER_WORDS+i]=v[1u+i];}d[4]=event_disk_checksum(d+EVENT_DISK_HEADER_WORDS,c*8u);bytes=(EVENT_DISK_HEADER_WORDS+c*8u)*4u;fd=sys_file_open(EVENT_DISK_NAME,FAT16_MODE_WRITE|FAT16_MODE_CREATE|FAT16_MODE_TRUNC);if(fd==0xffffffffu)return 0u;written=sys_file_write(fd,d,bytes);sys_file_close(fd);return written==bytes;}
__attribute__((section(".usertext"))) static void event_print_rows(uint32_t*v,uint32_t n){uint32_t i;char b[16];put("EVENTLOG SEQ TICK PID TYPE REASON STATUS VECTOR ERROR\n");for(i=0u;i<n&&i<64u;i++){uint32_t*q=v+i*8u;dec(q[0],b);put(b);put(" ");dec(q[1],b);put(b);put(" ");dec(q[2],b);put(b);put(" ");dec(q[3],b);put(b);put(" ");dec(q[4],b);put(b);put(" ");dec(q[5],b);put(b);put(" ");dec(q[6],b);put(b);put(" ");dec(q[7],b);put(b);put("\n");}}
__attribute__((section(".usertext"))) static void eventlog_command(char*p){
    uint32_t*v=shell_event_snapshot,n,*e,c;skip(&p);
    if(eq(p,"clear")){put(sys_event_log((uint32_t*)0,1u)==0u?"eventlog: clear OK\n":"eventlog: clear FAIL\n");return;}
    if(eq(p,"save")){put(event_disk_save()?"eventlog: save OK\n":"eventlog: save FAIL\n");return;}
    if(eq(p,"disk")){if(!event_disk_load(&e,&c)){put("eventlog: disk missing/invalid\n");return;}event_print_rows(e,c);return;}
    if(eq(p,"clear disk")){uint32_t r=sys_file_delete(EVENT_DISK_NAME);put(r==0u?"eventlog: disk clear OK\n":"eventlog: disk clear FAIL\n");return;}
    if(*p){put("usage: eventlog [clear|save|disk|clear disk]\n");return;}
    n=sys_event_log(v,0u);if(n==0xffffffffu){put("eventlog: FAIL\n");return;}event_print_rows(v+1u,v[0]);
}
__attribute__((section(".usertext"))) static const char* recovery_event_name(uint32_t type,uint32_t reason){if(type==EVENT_REC_NORMAL)return "NORMAL";if(type==EVENT_REC_DEGRADED)return "DEGRADED";if(type==EVENT_REC_SAFE)return "SAFE";if(type==EVENT_REC_RESTART)return "RESTART";if(type==EVENT_REC_WATCHDOG)return "WATCHDOG";if(type==EVENT_REC_BUDGET)return "BUDGET";if(reason==2u)return "FAULT";if(reason==3u)return "WATCHDOG_STOP";return 0;}
__attribute__((section(".usertext"))) static void recovery_print_rows(uint32_t*v,uint32_t n){uint32_t i;char b[16];put("RECOVERY SEQ TICK PID EVENT REASON STATUS\n");for(i=0u;i<n&&i<64u;i++){uint32_t*q=v+i*8u;const char*name=recovery_event_name(q[3],q[4]);if(!name)continue;dec(q[0],b);put(b);put(" ");dec(q[1],b);put(b);put(" ");dec(q[2],b);put(b);put(" ");put(name);put(" ");dec(q[4],b);put(b);put(" ");dec(q[5],b);put(b);put("\n");}}
__attribute__((section(".usertext"))) static void recoverylog_command(char*p){uint32_t*v=shell_event_snapshot,n,*e,c;skip(&p);if(eq(p,"disk")){if(!event_disk_load(&e,&c)){put("recoverylog: disk missing/invalid\n");return;}recovery_print_rows(e,c);return;}if(*p){put("usage: recoverylog [disk]\n");return;}n=sys_event_log(v,0u);if(n==0xffffffffu){put("recoverylog: FAIL\n");return;}recovery_print_rows(v+1u,v[0]);}
__attribute__((section(".usertext"))) static uint32_t boot_diag_load(uint32_t*out){
    uint32_t fd,got,bytes,i,h=2166136261u;
    fd=sys_file_open(BOOT_DIAG_NAME,FAT16_MODE_READ);if(fd==0xffffffffu)return 0u;
    bytes=sys_file_size(BOOT_DIAG_NAME);if(bytes!=BOOT_DIAG_WORDS*4u){sys_file_close(fd);return 0u;}
    got=sys_file_read(fd,shell_boot_diag,bytes);sys_file_close(fd);if(got!=bytes)return 0u;
    if(shell_boot_diag[0]!=BOOT_DIAG_MAGIC||shell_boot_diag[1]!=BOOT_DIAG_VERSION)return 0u;
    for(i=0u;i<5u;i++){h^=shell_boot_diag[i];h*=16777619u;}if(h!=shell_boot_diag[5])return 0u;
    if(out)for(i=0u;i<BOOT_DIAG_WORDS;i++)out[i]=shell_boot_diag[i];return 1u;
}
__attribute__((section(".usertext"))) static uint32_t boot_diag_mark(uint32_t reason){
    uint32_t old[BOOT_DIAG_WORDS],seq=1u,fd,w,i,h=2166136261u;
    if(reason<1u||reason>4u)return 0u;if(boot_diag_load(old))seq=old[3]+1u;
    shell_boot_diag[0]=BOOT_DIAG_MAGIC;shell_boot_diag[1]=BOOT_DIAG_VERSION;shell_boot_diag[2]=reason;shell_boot_diag[3]=seq;shell_boot_diag[4]=sys_timer_get();
    for(i=0u;i<5u;i++){h^=shell_boot_diag[i];h*=16777619u;}shell_boot_diag[5]=h;
    fd=sys_file_open(BOOT_DIAG_NAME,FAT16_MODE_WRITE|FAT16_MODE_CREATE|FAT16_MODE_TRUNC);if(fd==0xffffffffu)return 0u;w=sys_file_write(fd,shell_boot_diag,sizeof(shell_boot_diag));sys_file_close(fd);return w==sizeof(shell_boot_diag);
}
__attribute__((section(".usertext"))) static void boot_diag_show(void){uint32_t v[BOOT_DIAG_WORDS];char b[16];if(!boot_diag_load(v)){put("BOOTDIAG: no valid previous reboot marker\n");return;}put("BOOTDIAG REASON=");dec(v[2],b);put(b);put(" SEQ=");dec(v[3],b);put(b);put(" MARK_TICK=");dec(v[4],b);put(b);put("\n");}
__attribute__((section(".usertext"))) static const char* safe_mode_name(uint32_t m){return m==0u?"NORMAL":(m==1u?"DEGRADED":(m==2u?"SAFE":"INVALID"));}
__attribute__((section(".usertext"))) static void safepolicy_command(char*p){uint32_t q[6];char b[16];skip(&p);if(eq(p,"reset")){if(sys_safe_policy((uint32_t*)0,1u)==0u)put("safepolicy: counters reset OK\n");else put("safepolicy: reset FAIL\n");return;}if(*p){put("usage: safepolicy [reset]\n");return;}if(sys_safe_policy(q,0u)!=0u){put("safepolicy: FAIL\n");return;}put("SAFEPOLICY MODE=");put(safe_mode_name(q[0]));put(" FLAGS=");dec(q[1],b);put(b);put(" DENY_SPAWN=");dec(q[2],b);put(b);put(" DENY_MT=");dec(q[3],b);put(b);put(" DENY_RT=");dec(q[4],b);put(b);put(" TOTAL=");dec(q[5],b);put(b);put("\n");}

__attribute__((section(".usertext"))) static void safemode_command(char*p){uint32_t v[4],mode,reason=0u;char b[16];skip(&p);if(!*p){if(sys_safe_mode(v,0u,0u,0u)!=0u){put("safemode: FAIL\n");return;}put("SAFEMODE STATE=");put(safe_mode_name(v[0]));put(" REASON=");dec(v[1],b);put(b);put(" SEQ=");dec(v[2],b);put(b);put(" TICK=");dec(v[3],b);put(b);put("\n");return;}if(prefix(p,"normal")){mode=0u;p+=6;}else if(prefix(p,"degraded")){mode=1u;p+=8;}else if(prefix(p,"safe")){mode=2u;p+=4;}else{put("usage: safemode [normal|degraded|safe [REASON]]\n");return;}skip(&p);if(*p){reason=number(&p);skip(&p);if(*p){put("usage: safemode [normal|degraded|safe [REASON]]\n");return;}}if(sys_safe_mode((uint32_t*)0,1u,mode,reason)!=0u){put("safemode: FAIL\n");return;}put("safemode: ");put(safe_mode_name(mode));put(" OK\n");}

__attribute__((section(".usertext"))) static const char*health_name(uint32_t v){return v==0u?"NORMAL":(v==1u?"DEGRADED":"SAFE");}
__attribute__((section(".usertext"))) static void health_command(void){uint32_t q[10];char b[16];if(sys_system_health(q)!=0u){put("health: FAIL\n");return;}put("HEALTH STATE=");put(health_name(q[0]));put(" FLAGS=");dec(q[1],b);put(b);put(" SAFE=");dec(q[2],b);put(b);put(" REASON=");dec(q[3],b);put(b);put(" RT=");dec(q[4],b);put(b);put(" MT=");dec(q[5],b);put(b);put(" HANDLES=");dec(q[6],b);put(b);put(" HWWD_ARMED=");dec(q[7],b);put(b);put(" BACKEND=");dec(q[8],b);put(b);put(" TICK=");dec(q[9],b);put(b);put("\n");}

__attribute__((section(".usertext"))) static void hwwd_command(char*p){uint32_t v[6],r,n;char b[16];skip(&p);if(!*p||eq(p,"status")){if(sys_hwwd(v,0u,0u)!=0u){put("hwwd: FAIL\n");return;}put("HWWD BACKEND=");dec(v[0],b);put(b);put(" ARMED=");dec(v[1],b);put(b);put(" TIMEOUT=");dec(v[2],b);put(b);put(" LAST_FEED=");dec(v[3],b);put(b);put(" FEEDS=");dec(v[4],b);put(b);put(" NOW=");dec(v[5],b);put(b);put("\n");return;}if(prefix(p,"arm ")){p+=4;skip(&p);n=number(&p);skip(&p);if(*p||!n){put("usage: hwwd arm TICKS\n");return;}r=sys_hwwd((uint32_t*)0,1u,n);put(r==0u?"hwwd: arm OK (emulated backend)\n":"hwwd: arm FAIL\n");return;}if(eq(p,"feed")){r=sys_hwwd((uint32_t*)0,2u,0u);put(r!=0xffffffffu?"hwwd: feed OK\n":"hwwd: feed FAIL\n");return;}if(eq(p,"disarm")){r=sys_hwwd((uint32_t*)0,3u,0u);put(r==0u?"hwwd: disarm OK\n":"hwwd: disarm FAIL\n");return;}put("usage: hwwd [status|arm TICKS|feed|disarm]\n");}
__attribute__((section(".usertext"))) static void bootdiag_command(char*p){uint32_t reason,r;skip(&p);if(!*p){boot_diag_show();return;}if(eq(p,"clear")){if(sys_file_size(BOOT_DIAG_NAME)==0xffffffffu){put("bootdiag: clear OK (missing)\n");return;}r=sys_file_delete(BOOT_DIAG_NAME);put(r==0u?"bootdiag: clear OK\n":"bootdiag: clear FAIL\n");return;}if(prefix(p,"mark ")){p+=5;skip(&p);reason=number(&p);skip(&p);if(*p||reason<1u||reason>4u){put("usage: bootdiag mark REASON(1..4)\n");return;}put(boot_diag_mark(reason)?"bootdiag: mark OK\n":"bootdiag: mark FAIL\n");return;}put("usage: bootdiag [mark REASON(1..4)|clear]\n");}
__attribute__((section(".usertext"))) static void test_command(char*p);
__attribute__((section(".usertext"))) static void layout_command(void){uint32_t q[16];char b[16];if(sys_layout_info(q)!=0u){put("LAYOUT: FAIL\n");return;}put("LAYOUT: kernel_lba=");dec(q[4],b);put(b);put(" kernel_bytes=");dec(q[5],b);put(b);put(" kernel_sectors=");dec(q[6],b);put(b);put(" reserve=");dec(q[7],b);put(b);put(" gap=");dec(q[8],b);put(b);put(" fat_lba=");dec(q[9],b);put(b);put(" fat_sectors=");dec(q[10],b);put(b);put(" image_sectors=");dec(q[11],b);put(b);put(" align=");dec(q[12],b);put(b);put("\n");}

__attribute__((section(".usertext"))) static void resstat_command(char*p);

__attribute__((section(".usertext"))) static void uartstat_command(void){
    uint32_t q[16];char b[16];
    if(syscall3(SYS_UART_TRANSPORT,(uint32_t)q,11u,0u)!=0u){put("UARTSTAT: FAIL\n");return;}
    put("UARTSTAT: EN=");dec(q[0],b);put(b);put(" RXQ=");dec(q[1],b);put(b);put(" TXQ=");dec(q[2],b);put(b);put(" HWRX=");dec(q[3],b);put(b);put(" HWTX=");dec(q[4],b);put(b);put("\n");
    put("  POLL=");dec(q[5],b);put(b);put(" NONEMPTY=");dec(q[6],b);put(b);put(" MAXRXQ=");dec(q[12],b);put(b);put(" FLUSH=");dec(q[13],b);put(b);put(" IRQ4=");dec(q[14],b);put(b);put("\n");
    put("  ERR overrun=");dec(q[7],b);put(b);put(" OE=");dec(q[8],b);put(b);put(" PE=");dec(q[9],b);put(b);put(" FE=");dec(q[10],b);put(b);put(" BI=");dec(q[11],b);put(b);put(" LSR=0x");hex8(q[15],b);put(b);put("\n");
}

__attribute__((section(".usertext"),noinline,optimize("no-inline"))) static uint32_t execute_line(char*line){
    uint32_t r;trim(line);if(eq(line,""))return 0;if(eq(line,"help")){help();return 0;}if(prefix(line,"help ")){help_command(line+5);return 0;}if(eq(line,"clear")){clear();return 0;}if(eq(line,"ls")){if(sys_ls_path(".")!=0)put("ls: FAIL\n");return 0;}if(prefix(line,"ls ")){if(sys_ls_path(line+3)!=0)put("ls: FAIL\n");return 0;}if(prefix(line,"mkdir ")){if(sys_mkdir(line+6)==0)put("mkdir: OK\n");else put("mkdir: FAIL\n");return 0;}if(prefix(line,"rmdir ")){if(sys_rmdir(line+6)==0)put("rmdir: OK\n");else put("rmdir: FAIL\n");return 0;}if(prefix(line,"cd ")){if(sys_chdir(line+3)==0)put("cd: OK\n");else put("cd: FAIL\n");return 0;}if(eq(line,"cd")){put("usage: cd PATH\n");return 0;}if(eq(line,"pwd")){char cwd[128];r=sys_getcwd(cwd,sizeof(cwd));if(r==0xffffffffu)put("pwd: FAIL\n");else{put(cwd);put("\n");}return 0;}if(eq(line,"cwd-test")){cwd_test();return 0;}if(eq(line,"ticks")){char b[12];dec(sys_timer_get(),b);put(b);put("\n");return 0;}if(eq(line,"rtstat")){rtstat_command(line+6);return 0;}if(prefix(line,"rtstat ")){rtstat_command(line+7);return 0;}if(eq(line,"rtdata")){rtdata_command(line+6);return 0;}if(prefix(line,"rtdata ")){rtdata_command(line+7);return 0;}if(eq(line,"udp-test")){
        char t[]="UDP SLOT TEST: this line is overwritten in place";
        r=sys_console_at(20u,2u,t,sl(t));
        put(r==sl(t)?"udp-test: PASS\n":"udp-test: FAIL\n");
        return 0;
    }
if(eq(line,"pit-test")){pit_test();return 0;}if(eq(line,"exit")){put("shell halted.\n");for(;;){} }if(prefix(line,"echo ")){put(line+5);put("\n");return 0;}if(prefix(line,"cp ")){copy_file_command(line+3);return 0;}if(prefix(line,"cat ")){cat(line+4);return 0;}if(prefix(line,"write ")){char*p=line+6,*name,*text;skip(&p);name=p;while(*p&&*p!=' ')p++;if(!*p){put("usage: write FILE TEXT\n");return 0;}*p++=0;text=p;write_file(name,text,0);return 0;}if(prefix(line,"append ")){char*p=line+7,*name,*text;skip(&p);name=p;while(*p&&*p!=' ')p++;if(!*p){put("usage: append FILE TEXT\n");return 0;}*p++=0;text=p;write_file(name,text,1);return 0;}if(prefix(line,"crlf ")){char*p=line+5;skip(&p);if(!*p){put("usage: crlf FILE\n");return 0;}crlf_file(p);return 0;}if(eq(line,"crlf")){put("usage: crlf FILE\n");return 0;}if(prefix(line,"read ")){cat(line+5);return 0;}if(prefix(line,"rm ")){uint32_t r=sys_file_delete(line+3);put(r==0?"rm: OK\n":"rm: FAIL\n");return 0;}if(prefix(line,"delete ")){uint32_t r=sys_file_delete(line+7);put(r==0?"delete: OK\n":"delete: FAIL\n");return 0;}if(prefix(line,"filesize ")){filesize(line+9);return 0;}if(eq(line,"uartstat")){uartstat_command();return 0;}if(eq(line,"uartreset")){uint32_t a=syscall3(SYS_UART_TRANSPORT,0u,7u,0u),b=syscall3(SYS_UART_TRANSPORT,0u,3u,0u);put((a==0u&&b==0u)?"uartreset: OK\n":"uartreset: FAIL\n");return 0;}if(eq(line,"ps")){ps_command();return 0;}if(prefix(line,"ps ")){ps_pid_command(line+3);return 0;}if(eq(line,"layout")){layout_command();return 0;}if(eq(line,"resstat")){resstat_command(line+7);return 0;}if(prefix(line,"resstat ")){resstat_command(line+8);return 0;}if(eq(line,"eventlog")){eventlog_command(line+8);return 0;}if(prefix(line,"eventlog ")){eventlog_command(line+9);return 0;}if(eq(line,"recoverylog")){recoverylog_command(line+11);return 0;}if(prefix(line,"recoverylog ")){recoverylog_command(line+12);return 0;}if(eq(line,"safemode")){safemode_command(line+8);return 0;}if(prefix(line,"safemode ")){safemode_command(line+9);return 0;}if(eq(line,"safepolicy")){safepolicy_command(line+10);return 0;}if(prefix(line,"safepolicy ")){safepolicy_command(line+11);return 0;}if(eq(line,"health")){health_command();return 0;}if(eq(line,"hwwd")){hwwd_command(line+4);return 0;}if(prefix(line,"hwwd ")){hwwd_command(line+5);return 0;}if(eq(line,"bootdiag")){bootdiag_command(line+8);return 0;}if(prefix(line,"bootdiag ")){bootdiag_command(line+9);return 0;}if(eq(line,"supstat")){supstat_command();return 0;}if(prefix(line,"supstart ")){supstart_command(line+9);return 0;}if(eq(line,"supstart")){put("usage: supstart FILE.EXE\n");return 0;}if(prefix(line,"wait ")){wait_command(line+5);return 0;}if(eq(line,"wait")){put("usage: wait PID\n");return 0;}if(prefix(line,"test ")){put("test: interactive command only\n");return 0;}if(eq(line,"test")){put("test: interactive command only\n");return 0;}if(eq(line,"mtstat")){mtstat_command(line+6);return 0;}if(prefix(line,"mtstat ")){mtstat_command(line+7);return 0;}if(eq(line,"mtlist")){mtlist_command();return 0;}if(prefix(line,"mtdata ")){mtdata_command(line+7);return 0;}if(eq(line,"mtdata")){put("usage: mtdata MT1..MT25\n");return 0;}if(prefix(line,"mtstop ")){mtstop_command(line+7);return 0;}if(eq(line,"mtstop")){put("usage: mtstop MT1..MT25|ALL\n");return 0;}if(prefix(line,"spawn ")){spawn_command(line+6);return 0;}if(eq(line,"spawn")){put("usage: spawn FILE.EXE [ARG1..ARG8]\n");return 0;}if(prefix(line,"execmt ")){execmt_command(line+7);return 0;}if(prefix(line,"exec RTD.EXE ")){exec_rtd_command(line+13);return 0;}if(prefix(line,"exec LOADER.EXE ")){exec_loader_command(line+16);return 0;}if(prefix(line,"exec COMDRV.EXE ")){exec_com_command(line+16);return 0;}if(prefix(line,"exec NETDRV.EXE ")){exec_netdrv_command(line+16);return 0;}if(prefix(line,"exec VGADRV.EXE ")){exec_vga_command(line+16);return 0;}if(prefix(line,"exec ")){exec_file(line+5);return 0;}if(prefix(line,"outb ")){char*p=line+5;uint32_t port,value;skip(&p);port=number(&p);skip(&p);value=number(&p);if(port>0xffffu||value>0xffu){put("outb: usage outb PORT VALUE (PORT=0..65535, VALUE=0..255)\n");return 0;}if(sys_port_out8(port,value)==0){put("outb: OK\n");}else put("outb: FAIL\n");return 0;}if(prefix(line,"inb ")){char*p=line+4;uint32_t port,r;char b[12];skip(&p);port=number(&p);if(port>0xffffu){put("inb: usage inb PORT\n");return 0;}r=sys_port_in8(port);if(r==0xffffffffu){put("inb: FAIL\n");}else{hex8(r,b);put("inb: 0x");put(b);put(" (");dec(r,b);put(b);put(")\n");}return 0;}if(eq(line,"execq-stop")){r=sys_queue_stop();put(r==0?"execq-stop: requested\n":"execq-stop: no active queue\n");return 0;}if(prefix(line,"execq forever COMDRV.EXE ")){exec_queue_com_command(line+14,1);return 0;}if(prefix(line,"execq forever ")){exec_queue_command(line+14,1);return 0;}if(prefix(line,"execq ")){exec_queue_command(line+6,0);return 0;}if(eq(line,"syscall-test")){syscall_test();return 0;}if(prefix(line,"syscall ")){do_syscall(line+8);return 0;}put("unknown command; type help\n");return 0;
}

__attribute__((section(".usertext"))) static void resstat_command(char*p){uint32_t pid=0u,n;char b[16];skip(&p);if(*p){pid=number(&p);skip(&p);if(*p||!pid){put("usage: resstat [PID]\n");return;}n=sys_resource_info(1u,pid);put("PID ");dec(pid,b);put(b);put(" HANDLES=");}else{n=sys_resource_info(0u,0u);put("PROCESS HANDLES=");}if(n==0xffffffffu){put("FAIL\n");return;}dec(n,b);put(b);put("\n");}

/* FIX36D: deterministic, deliberately small test-script runner.
 * This is NOT a general shell language.  RUN reuses the normal case-insensitive
 * command dispatcher; ASSERT reads structured process metadata instead of
 * parsing console text.  No scheduler policy or syscall ABI is changed. */
__attribute__((section(".usertext"),noinline)) static uint32_t test_sonar_log_valid(uint32_t want);
__attribute__((section(".usertext"),noinline)) static uint32_t test_sonar_log_min(uint32_t want);
__attribute__((section(".usertext"),noinline)) static uint32_t test_assert(char*p){
    uint32_t *v=test_process_snapshot,n,i,rt=0u,mt=0u,pid,state;
    skip(&p);
    if(prefix(p,"FILE_EXISTS ")){uint32_t z;p+=12;skip(&p);if(!*p)return 0u;z=sys_file_size(p);return z!=0xffffffffu;}
    if(prefix(p,"FILE_SIZE ")){uint32_t want,z;char*name;p+=10;skip(&p);name=p;while(*p&&*p!=' '&&*p!='\t')p++;if(!*p)return 0u;*p++=0;skip(&p);want=number(&p);skip(&p);if(*p)return 0u;z=sys_file_size(name);return z!=0xffffffffu&&z==want;}
    if(prefix(p,"RT_ACTIVE ")){p+=10;pid=number(&p);skip(&p);if(*p)return 0u;n=sys_process_info(v);if(n==0xffffffffu)return 0u;for(i=0u;i<n&&i<34u;i++){uint32_t*q=v+i*10u;if(q[0]&&q[1]==3u&&(q[2]==1u||q[2]==2u||q[2]==3u))rt++;}return rt==pid;}
    if(prefix(p,"MT_ACTIVE ")){p+=10;pid=number(&p);skip(&p);if(*p)return 0u;n=sys_process_info(v);if(n==0xffffffffu)return 0u;for(i=0u;i<n&&i<34u;i++){uint32_t*q=v+i*10u;if(q[0]&&q[1]==2u&&(q[2]==1u||q[2]==2u||q[2]==3u))mt++;}return mt==pid;}
    if(prefix(p,"MT_PROGRESS ")){uint32_t slot,want,start,k;uint32_t*st=shell_mtstat_snapshot;p+=12;slot=number(&p);skip(&p);want=number(&p);skip(&p);if(*p||slot<1u||slot>25u)return 0u;slot--;if(syscall3(SYS_EXECMT_COMMIT,0u,0u,0u)!=0u)return 0u;start=sys_timer_get();while((uint32_t)(sys_timer_get()-start)<10u){for(k=0u;k<2048u;k++)__asm__ volatile("pause");}if(syscall3(SYS_MT_STATS,(uint32_t)st,0u,0u)!=0u)return 0u;return st[slot*10u+2u]>=want&&st[slot*10u+4u]>=want;}
    if(prefix(p,"MT_HEARTBEAT_WAIT ")){uint32_t slot,want,limit,start,k,hb[3],got,j,*q,target=0u;p+=18;slot=number(&p);skip(&p);want=number(&p);skip(&p);limit=number(&p);skip(&p);if(*p||slot<1u||slot>25u||!limit)return 0u;slot--;if(syscall3(SYS_EXECMT_COMMIT,0u,0u,0u)!=0u)return 0u;got=sys_process_info(v);if(got==0xffffffffu)return 0u;for(j=0u;j<got&&j<34u;j++){q=v+j*10u;if(q[0]&&q[1]==2u&&(q[2]==1u||q[2]==2u||q[2]==3u)&&q[3]==slot){target=q[0];break;}}if(!target)return 0u;start=sys_timer_get();for(;;){if(syscall3(SYS_PROCESS_HEARTBEAT,(uint32_t)hb,target,0u)==1u&&hb[2]>=want)return 1u;if((uint32_t)(sys_timer_get()-start)>=limit)return 0u;for(k=0u;k<2048u;k++)__asm__ volatile("pause");}}
    if(prefix(p,"MT_HEARTBEAT_IDLE_WAIT ")){uint32_t slot,want,limit,start,spin,r,hb[3],got,j,*q,target=0u;char c=0;p+=23;slot=number(&p);skip(&p);want=number(&p);skip(&p);limit=number(&p);skip(&p);if(*p||slot<1u||slot>25u||!limit)return 0u;slot--;if(syscall3(SYS_EXECMT_COMMIT,0u,0u,0u)!=0u)return 0u;got=sys_process_info(v);if(got==0xffffffffu)return 0u;for(j=0u;j<got&&j<34u;j++){q=v+j*10u;if(q[0]&&q[1]==2u&&(q[2]==1u||q[2]==2u||q[2]==3u)&&q[3]==slot){target=q[0];break;}}if(!target)return 0u;start=sys_timer_get();for(;;){if(syscall3(SYS_PROCESS_HEARTBEAT,(uint32_t)hb,target,0u)==1u&&hb[2]>=want)return 1u;if((uint32_t)(sys_timer_get()-start)>=limit)return 0u;r=sys_console_read(&c,1u);/* FIX60ZEI: heartbeat wait must exercise the same idle-shell contract used by the stable runtime. Token 2 is a real idle opportunity; token 3 is the one-shot prompt redraw event and is consumed without weakening the heartbeat threshold. */if(r==3u)continue;if(r!=2u)return 0u;for(spin=0u;spin<4096u;spin++)__asm__ volatile("pause");}}
    if(prefix(p,"MT_HEARTBEAT ")){uint32_t slot,want,hb[3],got,j,*q;p+=13;slot=number(&p);skip(&p);want=number(&p);skip(&p);if(*p||slot<1u||slot>25u)return 0u;slot--;got=sys_process_info(v);if(got==0xffffffffu)return 0u;for(j=0u;j<got&&j<34u;j++){q=v+j*10u;if(q[0]&&q[1]==2u&&(q[2]==1u||q[2]==2u||q[2]==3u)&&q[3]==slot){if(syscall3(SYS_PROCESS_HEARTBEAT,(uint32_t)hb,q[0],0u)!=1u)return 0u;return hb[2]>=want;}}return 0u;}
    if(prefix(p,"SONAR_LOG_VALID ")){uint32_t want;p+=16;want=number(&p);skip(&p);return !*p&&test_sonar_log_valid(want);}
    if(prefix(p,"SONAR_LOG_MIN ")){uint32_t want;p+=14;want=number(&p);skip(&p);return !*p&&test_sonar_log_min(want);}
    if(prefix(p,"DATA_PUBLISHED ")){uint32_t ch,want,info[7];p+=15;ch=number(&p);skip(&p);want=number(&p);skip(&p);if(*p||ch>=4u)return 0u;if(syscall3(SYS_DATA_CHANNEL,(uint32_t)info,0u,ch)!=0u)return 0u;return info[3]>=want;}
    if(prefix(p,"DATA_PUBLISHED_WAIT ")){uint32_t ch,want,limit,start,k,info[7];p+=20;ch=number(&p);skip(&p);want=number(&p);skip(&p);limit=number(&p);skip(&p);if(*p||ch>=4u||!limit)return 0u;if(syscall3(SYS_EXECMT_COMMIT,0u,0u,0u)!=0u)return 0u;start=sys_timer_get();for(;;){if(syscall3(SYS_DATA_CHANNEL,(uint32_t)info,0u,ch)!=0u)return 0u;if(info[3]>=want)return 1u;if((uint32_t)(sys_timer_get()-start)>=limit)return 0u;for(k=0u;k<2048u;k++)__asm__ volatile("pause");}}
    if(prefix(p,"UART_ENABLED ")){uint32_t want,q[16];p+=13;want=number(&p);skip(&p);return !*p&&syscall3(SYS_UART_TRANSPORT,(uint32_t)q,11u,0u)==0u&&q[0]==want;}
    if(prefix(p,"UART_IRQ4 ")){uint32_t want,q[16];p+=10;want=number(&p);skip(&p);return !*p&&syscall3(SYS_UART_TRANSPORT,(uint32_t)q,11u,0u)==0u&&q[14]==want;}
    if(prefix(p,"LAST_STATUS ")){p+=12;pid=number(&p);skip(&p);return !*p&&test_last_wait_valid&&test_last_wait_status==pid;}
    if(prefix(p,"LAST_REASON ")){p+=12;pid=number(&p);skip(&p);return !*p&&test_last_wait_valid&&test_last_wait_reason==pid;}
    if(prefix(p,"PID_ACTIVE ")){p+=11;pid=number(&p);skip(&p);if(*p||!pid)return 0u;n=sys_process_info(v);if(n==0xffffffffu)return 0u;for(i=0u;i<n&&i<34u;i++){uint32_t*q=v+i*10u;if(q[0]==pid)return q[2]==1u||q[2]==2u||q[2]==3u;}return 0u;}
    if(prefix(p,"PID_EXITED ")){p+=11;pid=number(&p);skip(&p);if(*p||!pid)return 0u;n=sys_process_info(v);if(n==0xffffffffu)return 0u;for(i=0u;i<n&&i<34u;i++){uint32_t*q=v+i*10u;if(q[0]==pid)return q[2]==4u||q[2]==5u;}return 0u;}
    if(prefix(p,"PID_STATE ")){p+=10;pid=number(&p);skip(&p);state=number(&p);skip(&p);if(*p||!pid)return 0u;n=sys_process_info(v);if(n==0xffffffffu)return 0u;for(i=0u;i<n&&i<34u;i++){uint32_t*q=v+i*10u;if(q[0]==pid)return q[2]==state;}return 0u;}
    if(prefix(p,"PROC_HANDLES ")){p+=13;pid=number(&p);skip(&p);if(*p)return 0u;return sys_resource_info(0u,0u)==pid;}
    if(prefix(p,"EVENT_TYPE ")){uint32_t*e=shell_event_snapshot,c,j,want;p+=11;want=number(&p);skip(&p);if(*p)return 0u;c=sys_event_log(e,0u);if(c==0xffffffffu)return 0u;for(j=0u;j<e[0]&&j<64u;j++)if(e[1u+j*8u+3u]==want)return 1u;return 0u;}
    if(eq(p,"RECOVERY_FAULT_CHAIN")){uint32_t*e=shell_event_snapshot,c,j,stage=0u;c=sys_event_log(e,0u);if(c==0xffffffffu)return 0u;for(j=0u;j<e[0]&&j<64u;j++){uint32_t*q=e+1u+j*8u;if(stage==0u&&q[4]==2u)stage=1u;else if(stage==1u&&q[3]==EVENT_REC_DEGRADED)stage=2u;else if(stage==2u&&q[3]==EVENT_REC_RESTART)stage=3u;else if(stage>=3u&&q[3]==EVENT_REC_BUDGET)stage=4u;else if(stage==4u&&q[3]==EVENT_REC_SAFE)stage=5u;}return stage==5u;}
    if(prefix(p,"EVENT_REASON ")){uint32_t*e=shell_event_snapshot,c,j,want;p+=13;want=number(&p);skip(&p);if(*p)return 0u;c=sys_event_log(e,0u);if(c==0xffffffffu)return 0u;for(j=0u;j<e[0]&&j<64u;j++)if(e[1u+j*8u+4u]==want)return 1u;return 0u;}
    if(eq(p,"EVENT_SEQ_MONOTONIC")){uint32_t*e=shell_event_snapshot,c,j;c=sys_event_log(e,0u);if(c==0xffffffffu)return 0u;for(j=1u;j<e[0]&&j<64u;j++)if(e[1u+j*8u]<=e[1u+(j-1u)*8u])return 0u;return 1u;}
    if(eq(p,"EVENT_TICK_MONOTONIC")){uint32_t*e=shell_event_snapshot,c,j;c=sys_event_log(e,0u);if(c==0xffffffffu)return 0u;for(j=1u;j<e[0]&&j<64u;j++)if(e[1u+j*8u+1u]<e[1u+(j-1u)*8u+1u])return 0u;return 1u;}
    if(eq(p,"EVENT_DISK_SEQ_MONOTONIC")){uint32_t*e,c,j;if(!event_disk_load(&e,&c))return 0u;for(j=1u;j<c;j++)if(e[j*8u]<=e[(j-1u)*8u])return 0u;return 1u;}
    if(eq(p,"EVENT_DISK_RECOVERY_CHAIN")){uint32_t*e,c,j,stage=0u;if(!event_disk_load(&e,&c))return 0u;for(j=0u;j<c;j++){uint32_t*q=e+j*8u;if(stage==0u&&q[4]==2u)stage=1u;else if(stage==1u&&q[3]==EVENT_REC_DEGRADED)stage=2u;else if(stage==2u&&q[3]==EVENT_REC_RESTART)stage=3u;else if(stage>=3u&&q[3]==EVENT_REC_BUDGET)stage=4u;else if(stage==4u&&q[3]==EVENT_REC_SAFE)stage=5u;}return stage==5u;}
    if(prefix(p,"RT_SLOT_PROGRESS ")){uint32_t slot,before,after,k;p+=17;slot=number(&p);skip(&p);if(*p||slot<1u||slot>8u)return 0u;slot--;if(rtstat_snapshot(shell_rtstat_snapshot)!=0u)return 0u;before=shell_rtstat_snapshot[slot*30u+26u];/* FIX60ZA: q[8] is a rolling-window job count and may remain constant while a healthy periodic task advances. q[26] is the monotonic completed-job sequence (last_seq), so it is the correct progress invariant. */for(k=0u;k<128u;k++){if(sys_rt_yield()==0xffffffffu)return 0u;if(rtstat_snapshot(shell_rtstat_snapshot)!=0u)return 0u;after=shell_rtstat_snapshot[slot*30u+26u];if(after!=before)return 1u;}return 0u;}
    if(prefix(p,"RT_SWEEP_FAIR ")){uint32_t count,want,startseq[8],d[8],i,k,min=0xffffffffu,max=0u;p+=14;count=number(&p);skip(&p);want=number(&p);skip(&p);if(*p||count<2u||count>8u||want==0u)return 0u;if(rtstat_snapshot(shell_rtstat_snapshot)!=0u)return 0u;for(i=0u;i<count;i++)startseq[i]=shell_rtstat_snapshot[i*30u+26u];for(k=0u;k<512u;k++){if(sys_rt_yield()==0xffffffffu)return 0u;}if(rtstat_snapshot(shell_rtstat_snapshot)!=0u)return 0u;for(i=0u;i<count;i++){uint32_t now=shell_rtstat_snapshot[i*30u+26u];d[i]=now-startseq[i];if(d[i]<want)return 0u;if(d[i]<min)min=d[i];if(d[i]>max)max=d[i];}/* Distinct-slot sweeps should keep equal-period jobs close even with different priorities. */return max-min<=2u;}
    if(eq(p,"CONSOLE_REDRAW")){char c=0;return sys_console_read(&c,1u)==3u;}
    if(prefix(p,"CONSOLE_IDLE_TICKS ")){uint32_t want,start,spin,r;char c=0;p+=19;want=number(&p);skip(&p);if(*p||want==0u||want>200u)return 0u;start=sys_rt_time_get();while((uint32_t)(sys_rt_time_get()-start)<want){r=sys_console_read(&c,1u);/* FIX60ZEH: redraw token 3 is a legitimate one-shot UI event, not an idle failure. Consume it and continue until the requested number of RT ticks has elapsed. Only token 2 represents the actual idle-shell scheduler window; any key/error token is still a hard test failure. */if(r==3u)continue;if(r!=2u)return 0u;for(spin=0u;spin<4096u;spin++)__asm__ volatile("pause");}return 1u;}
    if(prefix(p,"RT_DUMP_NONEMPTY ")){uint32_t slot;p+=17;slot=number(&p);skip(&p);if(*p||slot<1u||slot>8u)return 0u;return syscall3(SYS_RT_STATS,(uint32_t)shell_rtdump_snapshot,2u,slot-1u)==0u&&shell_rtdump_snapshot[0]>0u;}
    if(prefix(p,"EVENT_DISK_REASON ")){uint32_t*e,c,j,want;p+=18;want=number(&p);skip(&p);if(*p)return 0u;if(!event_disk_load(&e,&c))return 0u;for(j=0u;j<c;j++)if(e[j*8u+4u]==want)return 1u;return 0u;}
    if(prefix(p,"EVENT_DISK_COUNT ")){uint32_t*e,c,want;p+=17;want=number(&p);skip(&p);if(*p)return 0u;if(!event_disk_load(&e,&c))return 0u;(void)e;return c==want;}
    if(eq(p,"EVENT_DISK_MISSING")){uint32_t*e,c;return !event_disk_load(&e,&c);}
    if(prefix(p,"BOOT_REASON ")){uint32_t v[BOOT_DIAG_WORDS],want;p+=12;want=number(&p);skip(&p);return !*p&&boot_diag_load(v)&&v[2]==want;}
    if(eq(p,"BOOT_MARKER_MISSING")){return !boot_diag_load((uint32_t*)0);}
    if(eq(p,"LAYOUT_VALID")){uint32_t q[16],i,sum=0u;if(sys_layout_info(q)!=0u)return 0u;for(i=0u;i<16u;i++)sum+=q[i];return q[0]==0x3159414cu&&q[1]==1u&&q[2]==64u&&sum==0u;}
    if(prefix(p,"LAYOUT_KERNEL_LBA ")){uint32_t q[16],want;p+=18;want=number(&p);skip(&p);return !*p&&sys_layout_info(q)==0u&&q[4]==want;}
    if(prefix(p,"LAYOUT_FAT_ALIGNED ")){uint32_t q[16],want;p+=19;want=number(&p);skip(&p);return !*p&&want&&sys_layout_info(q)==0u&&(q[9]%want)==0u;}
    if(prefix(p,"LAYOUT_GAP_MIN ")){uint32_t q[16],want;p+=15;want=number(&p);skip(&p);return !*p&&sys_layout_info(q)==0u&&q[8]>=want&&q[9]>=q[4]+q[6]+q[7]+q[8];}
    if(eq(p,"LAYOUT_FAT_ALIGNED_SELF")){uint32_t q[16];return sys_layout_info(q)==0u&&q[12]!=0u&&(q[9]%q[12])==0u;}
    if(eq(p,"LAYOUT_RESERVE_GAP_SEPARATE")){uint32_t q[16],kend,rend; if(sys_layout_info(q)!=0u||!q[7]||!q[8])return 0u;kend=q[4]+q[6];rend=kend+q[7];return kend>=q[4]&&rend>=kend&&q[9]>=rend+q[8];}
    if(eq(p,"LAYOUT_MEMORY_VALID")){uint32_t q[16],end;if(sys_layout_info(q)!=0u||q[14]!=0x00007e00u||q[15]<=q[14])return 0u;end=q[14]+q[6]*512u;return end>=q[14]&&end<=q[15];}
    if(eq(p,"LAYOUT_IMAGE_MATCH")){uint32_t q[16];return sys_layout_info(q)==0u&&q[9]+q[10]==q[11];}
    if(eq(p,"FS_RW")){static const char n[]="L53RW.TMP",d[]="FIX53";char z[8];uint32_t fd,w,r,i;fd=sys_file_open(n,FAT16_MODE_WRITE|FAT16_MODE_CREATE|FAT16_MODE_TRUNC);if(fd==0xffffffffu)return 0u;w=sys_file_write(fd,d,5u);sys_file_close(fd);if(w!=5u){sys_file_delete(n);return 0u;}fd=sys_file_open(n,FAT16_MODE_READ);if(fd==0xffffffffu){sys_file_delete(n);return 0u;}r=sys_file_read(fd,z,5u);sys_file_close(fd);if(r!=5u){sys_file_delete(n);return 0u;}for(i=0u;i<5u;i++)if(z[i]!=d[i]){sys_file_delete(n);return 0u;}return sys_file_delete(n)==0u;}
    if(eq(p,"TST_APPEND_STRESS")){static const char n[]="/TST/TSTRW.TMP";char w[128],r[128];uint32_t i,j,fd,z;(void)sys_file_delete(n);for(i=0u;i<128u;i++)w[i]=(char)('A'+(i%23u));fd=sys_file_open(n,FAT16_MODE_WRITE|FAT16_MODE_CREATE|FAT16_MODE_TRUNC);if(fd==0xffffffffu)return 0u;if(sys_file_close(fd)!=0u)return 0u;for(j=0u;j<48u;j++){uint32_t before=sys_file_size(n);if(before!=j*128u){(void)sys_file_delete(n);return 0u;}fd=sys_file_open(n,FAT16_MODE_WRITE|FAT16_MODE_APPEND);if(fd==0xffffffffu){(void)sys_file_delete(n);return 0u;}z=sys_file_write(fd,w,128u);if(sys_file_close(fd)!=0u||z!=128u||sys_file_size(n)!=(j+1u)*128u){(void)sys_file_delete(n);return 0u;}}fd=sys_file_open(n,FAT16_MODE_READ);if(fd==0xffffffffu){(void)sys_file_delete(n);return 0u;}for(j=0u;j<48u;j++){z=sys_file_read(fd,r,128u);if(z!=128u){(void)sys_file_close(fd);(void)sys_file_delete(n);return 0u;}for(i=0u;i<128u;i++)if(r[i]!=w[i]){(void)sys_file_close(fd);(void)sys_file_delete(n);return 0u;}}if(sys_file_close(fd)!=0u){(void)sys_file_delete(n);return 0u;}return sys_file_delete(n)==0u;}
    if(eq(p,"TST_DIR_RW")){static const char n[]="/TST/TCHECK.TMP",d[]="DIRCHAIN";char z[8];uint32_t fd,w,r,i;(void)sys_file_delete(n);fd=sys_file_open(n,FAT16_MODE_WRITE|FAT16_MODE_CREATE|FAT16_MODE_TRUNC);if(fd==0xffffffffu)return 0u;w=sys_file_write(fd,d,8u);sys_file_close(fd);if(w!=8u||sys_file_size(n)!=8u){sys_file_delete(n);return 0u;}fd=sys_file_open(n,FAT16_MODE_READ);if(fd==0xffffffffu){sys_file_delete(n);return 0u;}r=sys_file_read(fd,z,8u);sys_file_close(fd);if(r!=8u){sys_file_delete(n);return 0u;}for(i=0u;i<8u;i++)if(z[i]!=d[i]){sys_file_delete(n);return 0u;}return sys_file_delete(n)==0u;}if(eq(p,"TST_APPEND_RW")){static const char n[]="/TST/TCHECK.TMP",a[]="AAAA",b[]="BBBB",e[]="AAAABBBB";char z[8];uint32_t fd,r,i;(void)sys_file_delete(n);fd=sys_file_open(n,FAT16_MODE_WRITE|FAT16_MODE_CREATE|FAT16_MODE_TRUNC);if(fd==0xffffffffu)return 0u;if(sys_file_write(fd,a,4u)!=4u||sys_file_close(fd)!=0u){(void)sys_file_delete(n);return 0u;}fd=sys_file_open(n,FAT16_MODE_WRITE|FAT16_MODE_APPEND);if(fd==0xffffffffu){(void)sys_file_delete(n);return 0u;}if(sys_file_write(fd,b,4u)!=4u||sys_file_close(fd)!=0u){(void)sys_file_delete(n);return 0u;}if(sys_file_size(n)!=8u){(void)sys_file_delete(n);return 0u;}fd=sys_file_open(n,FAT16_MODE_READ);if(fd==0xffffffffu){(void)sys_file_delete(n);return 0u;}r=sys_file_read(fd,z,8u);(void)sys_file_close(fd);if(r!=8u){(void)sys_file_delete(n);return 0u;}for(i=0u;i<8u;i++)if(z[i]!=e[i]){(void)sys_file_delete(n);return 0u;}return sys_file_delete(n)==0u;}
    if(prefix(p,"RM_ATTEMPTS ")){uint32_t q[8],want;p+=12;want=number(&p);skip(&p);return !*p&&sys_recovery_manager(q,0u)==0u&&q[0]==want;}
    if(prefix(p,"RM_LIMIT ")){uint32_t q[8],want;p+=9;want=number(&p);skip(&p);return !*p&&sys_recovery_manager(q,0u)==0u&&q[1]==want;}
    if(prefix(p,"RM_KIND ")){uint32_t q[8],want;p+=8;want=number(&p);skip(&p);return !*p&&sys_recovery_manager(q,0u)==0u&&q[3]==want;}
    if(prefix(p,"RM_ACTION ")){uint32_t q[8],want;p+=10;want=number(&p);skip(&p);return !*p&&sys_recovery_manager(q,0u)==0u&&q[4]==want;}
    if(prefix(p,"RM_STATE ")){uint32_t q[12],want;p+=9;want=number(&p);skip(&p);return !*p&&sys_recovery_manager_ext(q)==0u&&q[1]==want;}
    if(prefix(p,"RM_RESULT ")){uint32_t q[12],want;p+=10;want=number(&p);skip(&p);return !*p&&sys_recovery_manager_ext(q)==0u&&q[8]==want;}
    if(eq(p,"RM_PID_CHANGED")){uint32_t q[12];return sys_recovery_manager_ext(q)==0u&&q[4]!=0u&&q[5]!=0u&&q[4]!=q[5];}
    if(prefix(p,"RM_OLD_PID ")){uint32_t q[12],want;p+=11;want=number(&p);skip(&p);return !*p&&sys_recovery_manager_ext(q)==0u&&q[4]==want;}
    if(prefix(p,"RM_NEW_PID ")){uint32_t q[12],want;p+=11;want=number(&p);skip(&p);return !*p&&sys_recovery_manager_ext(q)==0u&&q[5]==want;}
    if(prefix(p,"SAFE_MODE ")){uint32_t q[4],want;p+=10;want=number(&p);skip(&p);return !*p&&sys_safe_mode(q,0u,0u,0u)==0u&&q[0]==want;}
    if(prefix(p,"SAFE_REASON ")){uint32_t q[4],want;p+=12;want=number(&p);skip(&p);return !*p&&sys_safe_mode(q,0u,0u,0u)==0u&&q[1]==want;}
    if(prefix(p,"HEALTH_STATE ")){uint32_t q[10],want;p+=13;want=number(&p);skip(&p);return !*p&&sys_system_health(q)==0u&&q[0]==want;}
    if(prefix(p,"HEALTH_FLAGS ")){uint32_t q[10],want;p+=13;want=number(&p);skip(&p);return !*p&&sys_system_health(q)==0u&&q[1]==want;}
    if(prefix(p,"HEALTH_RT ")){uint32_t q[10],want;p+=10;want=number(&p);skip(&p);return !*p&&sys_system_health(q)==0u&&q[4]==want;}
    if(prefix(p,"HEALTH_MT ")){uint32_t q[10],want;p+=10;want=number(&p);skip(&p);return !*p&&sys_system_health(q)==0u&&q[5]==want;}
    if(prefix(p,"HEALTH_HANDLES ")){uint32_t q[10],want;p+=15;want=number(&p);skip(&p);return !*p&&sys_system_health(q)==0u&&q[6]==want;}
    if(prefix(p,"SAFE_POLICY_FLAGS ")){uint32_t q[6],want;p+=18;want=number(&p);skip(&p);return !*p&&sys_safe_policy(q,0u)==0u&&q[1]==want;}
    if(prefix(p,"SAFE_DENY_SPAWN ")){uint32_t q[6],want;p+=16;want=number(&p);skip(&p);return !*p&&sys_safe_policy(q,0u)==0u&&q[2]==want;}
    if(prefix(p,"SAFE_DENY_MT ")){uint32_t q[6],want;p+=13;want=number(&p);skip(&p);return !*p&&sys_safe_policy(q,0u)==0u&&q[3]==want;}
    if(prefix(p,"SAFE_DENY_RT ")){uint32_t q[6],want;p+=13;want=number(&p);skip(&p);return !*p&&sys_safe_policy(q,0u)==0u&&q[4]==want;}
    if(prefix(p,"SAFE_DENY_TOTAL ")){uint32_t q[6],want;p+=16;want=number(&p);skip(&p);return !*p&&sys_safe_policy(q,0u)==0u&&q[5]==want;}
    if(prefix(p,"HWWD_BACKEND ")){uint32_t q[6],want;p+=13;want=number(&p);skip(&p);return !*p&&sys_hwwd(q,0u,0u)==0u&&q[0]==want;}
    if(prefix(p,"HWWD_ARMED ")){uint32_t q[6],want;p+=11;want=number(&p);skip(&p);return !*p&&sys_hwwd(q,0u,0u)==0u&&q[1]==want;}
    if(prefix(p,"HWWD_TIMEOUT ")){uint32_t q[6],want;p+=13;want=number(&p);skip(&p);return !*p&&sys_hwwd(q,0u,0u)==0u&&q[2]==want;}
    if(prefix(p,"HWWD_FEEDS ")){uint32_t q[6],want;p+=11;want=number(&p);skip(&p);return !*p&&sys_hwwd(q,0u,0u)==0u&&q[4]==want;}
    if(prefix(p,"LAST_MTDATA ")){
        uint32_t slot=0xffffffffu,seq=0u,session=0u,r,j;const char*want;
        p+=12;skip(&p);want=p;if(!*want||!last_spawn_pid)return 0u;
        n=sys_process_info(v);if(n==0xffffffffu)return 0u;
        for(i=0u;i<n&&i<34u;i++){uint32_t*q=v+i*10u;if(q[0]==last_spawn_pid&&q[1]==2u){slot=q[3];break;}}
        if(slot>=25u)return 0u;r=sys_mt_data_read(test_mtdata_snapshot,slot,&seq,&session);(void)seq;(void)session;if(r==0xffffffffu||r==0xfffffffeu||r==0xfffffffdu)return 0u;
        for(j=0u;want[j]||test_mtdata_snapshot[j];j++)if(want[j]!=test_mtdata_snapshot[j])return 0u;return 1u;
    }
    put("TEST: unknown ASSERT: ");put(p);put("\n");return 0u;
}

__attribute__((section(".usertext"),noinline)) static uint32_t test_run_command(char*line);

/* FIX40A: load-test helpers.  START changes only test orchestration; the
 * normal EXECMT and RTD/F10 paths remain the only mechanisms that create tasks. */
__attribute__((section(".usertext"))) static uint32_t test_append(char*b,uint32_t*n,const char*s){uint32_t i=0u;while(s[i]){if(*n+1u>=400u)return 0u;b[(*n)++]=s[i++];}b[*n]=0;return 1u;}
__attribute__((section(".usertext"),noinline)) static uint32_t test_start_mt(uint32_t count){
    char *b=test_generated_line,tmp[16];uint32_t i,n=0u;
    if(count<1u||count>25u){put("TEST START MT: N must be 1..25\n");return 0u;}
    b[0]=0;
    for(i=1u;i<=count;i++){if(i>1u&&!test_append(b,&n," "))return 0u;if(!test_append(b,&n,"MT"))return 0u;if(i<10u&&!test_append(b,&n,"0"))return 0u;dec(i,tmp);if(!test_append(b,&n,tmp)||!test_append(b,&n,".EXE"))return 0u;}
    execmt_command(b);
    /* FIX54A: START MT is an assertion-bearing test directive.  Do not report
       success merely because execmt_command() returned to the shell: verify
       that the requested number of MT processes was actually admitted. */
    {uint32_t info[34u*10u],got=sys_process_info(info),j,active=0u;if(got==0xffffffffu)return 0u;for(j=0u;j<got&&j<34u;j++){uint32_t*q=info+j*10u;if(q[0]&&q[1]==2u&&(q[2]==1u||q[2]==2u||q[2]==3u))active++;}if(active!=count){put("TEST START MT: requested tasks were not started\n");return 0u;}}
    return 1u;
}
__attribute__((section(".usertext"),noinline)) static uint32_t test_start_rt_period(uint32_t count,uint32_t period){
    char *b=test_generated_line,tmp[16],pt[16];uint32_t i,n;
    if(count<1u||count>8u){put("TEST START RT: N must be 1..8\n");return 0u;}
    if(pending_rt_count){put("TEST START RT: pending RT queue is not empty\n");return 0u;}
    dec(period,pt);
    for(i=1u;i<=count;i++){n=0u;b[0]=0;if(!test_append(b,&n,"SENSOR.EXE "))return 0u;if(!test_append(b,&n,pt)||!test_append(b,&n," ")||!test_append(b,&n,pt)||!test_append(b,&n," "))return 0u;if(i==1u)dec(1u,tmp);else if(i==2u)dec(3u,tmp);else if(i==3u)dec(5u,tmp);else if(i==4u)dec(7u,tmp);else dec(i+3u,tmp);if(!test_append(b,&n,tmp))return 0u;exec_rtd_command(b);}
    rt_launch_pending();return pending_rt_count==0u;
}
__attribute__((section(".usertext"),noinline)) static uint32_t test_start_rt(uint32_t count){return test_start_rt_period(count,100u);}
__attribute__((section(".usertext"),noinline)) static uint32_t test_start_rt10(uint32_t count){return test_start_rt_period(count,10u);}
__attribute__((section(".usertext"),noinline)) static uint32_t test_type_command(char*p){uint32_t i=0u;skip(&p);if(!*p){put("TEST TYPE: empty command\n");return 0u;}while(p[i]){if(i+1u>=sizeof(test_stage_line)){put("TEST TYPE: command too long\n");test_stage_valid=0u;return 0u;}test_stage_line[i]=p[i];i++;}test_stage_line[i]=0;test_stage_valid=1u;put("TEST TYPE> ");put(test_stage_line);put("\n");return 1u;}
__attribute__((section(".usertext"),noinline)) static uint32_t test_key_command(char*p){skip(&p);if(eq(p,"F10")){put("TEST KEY F10\n");if(test_stage_valid){if(prefix(test_stage_line,"exec RTD.EXE ")){test_stage_valid=0u;if(!test_run_command(test_stage_line))return 0u;}else{put("TEST KEY F10: staged non-RT command requires ENTER\n");return 0u;}}rt_launch_pending();return 1u;}if(eq(p,"ENTER")){put("TEST KEY ENTER\n");if(!test_stage_valid){put("TEST KEY ENTER: no staged command\n");return 0u;}test_stage_valid=0u;return test_run_command(test_stage_line);}put("TEST KEY: use F10 or ENTER\n");return 0u;}
__attribute__((section(".usertext"),noinline)) static uint32_t test_start_command(char*p){uint32_t n;skip(&p);if(prefix(p,"MT ")){p+=3;n=number(&p);skip(&p);if(*p)return 0u;return test_start_mt(n);}if(prefix(p,"RT10 ")){p+=5;n=number(&p);skip(&p);if(*p)return 0u;return test_start_rt10(n);}if(prefix(p,"RT ")){p+=3;n=number(&p);skip(&p);if(*p)return 0u;return test_start_rt(n);}put("TEST START: use START MT N, START RT N or START RT10 N\n");return 0u;}
/* FIX48B: an expected SAFE denial is a successful regression condition, not a
 * failed START directive.  Exercise the normal RTD/F10 path, verify the kernel
 * denial counter changed exactly once and no RT task was admitted, then discard
 * the intentionally retained pending request so it cannot contaminate later
 * assertions. */
__attribute__((section(".usertext"),noinline)) static uint32_t test_expect_safe_deny_rt(void){
    uint32_t before[6],after[6],info[34u*10u],n,i,rt=0u;
    char cmd[]="SENSOR1.EXE 10 10 1";
    if(pending_rt_count){put("TEST EXPECT SAFE_DENY_RT: pending RT queue is not empty\n");return 0u;}
    if(sys_safe_policy(before,0u)!=0u||before[0]!=2u){put("TEST EXPECT SAFE_DENY_RT: SAFE mode required\n");return 0u;}
    exec_rtd_command(cmd);
    if(pending_rt_count!=1u){put("TEST EXPECT SAFE_DENY_RT: queue prepare FAIL\n");return 0u;}
    rt_launch_pending();
    if(sys_safe_policy(after,0u)!=0u){pending_rt_count=0u;return 0u;}
    n=sys_process_info(info);if(n==0xffffffffu){pending_rt_count=0u;return 0u;}
    for(i=0u;i<n&&i<34u;i++){uint32_t*q=info+i*10u;if(q[0]&&q[1]==3u&&(q[2]==1u||q[2]==2u||q[2]==3u))rt++;}
    pending_rt_count=0u;
    if(after[4]!=before[4]+1u||after[5]!=before[5]+1u||rt!=0u){put("TEST EXPECT SAFE_DENY_RT: denial state mismatch\n");return 0u;}
    put("TEST EXPECT SAFE_DENY_RT: OK\n");return 1u;
}
__attribute__((section(".usertext"),noinline)) static uint32_t test_expect_safe_deny_mt_start(void){uint32_t before[8],after[8],r;if(sys_safe_policy(before,0u)!=0u)return 0u;r=sys_mt_start();if(sys_safe_policy(after,0u)!=0u)return 0u;if(r!=SAFE_POLICY_DENIED||after[3]!=before[3]+1u){put("TEST EXPECT SAFE_DENY_MT_START: mismatch\n");return 0u;}put("TEST EXPECT SAFE_DENY_MT_START: OK\n");return 1u;}
__attribute__((section(".usertext"),noinline)) static uint32_t test_expect_rm_guards(void){uint32_t q[3],r;(void)sys_recovery_manager((uint32_t*)0,1u);q[0]=0x7ffffff0u;q[1]=1u;q[2]=54u;r=syscall3(SYS_RECOVERY_MANAGER,(uint32_t)q,2u,0u);if(r!=1u)return 0u;r=syscall3(SYS_RECOVERY_MANAGER,q[0],3u,0u);if(r!=0xffffffffu){put("TEST EXPECT RM_GUARDS: nonexistent old PID accepted\n");return 0u;}if(sys_recovery_manager((uint32_t*)0,1u)!=0u)return 0u;put("TEST EXPECT RM_GUARDS: OK\n");return 1u;}
__attribute__((section(".usertext"),noinline)) static uint32_t test_expect_uart_transport(void){
    uint8_t tx[256],rx[256];uint32_t info[10],diag[16],i,r,t0,t1;
#define UART_TEST_FAIL(msg) do{put("TEST EXPECT UART_TRANSPORT FAIL: " msg "\n");return 0u;}while(0)
    /* FIX60P: deterministic hooks must run offline. RESET intentionally does not change transport enable state. */
    if(syscall3(SYS_UART_TRANSPORT,0u,7u,0u)!=0u)UART_TEST_FAIL("DISABLE");
    if(syscall3(SYS_UART_TRANSPORT,0u,3u,0u)!=0u)UART_TEST_FAIL("RESET");
    if(syscall3(SYS_UART_TRANSPORT,(uint32_t)diag,11u,0u)!=0u||diag[0]!=0u||diag[3]!=0u||diag[4]!=0u||diag[14]!=0u)UART_TEST_FAIL("DIAG RESET");
    if(syscall3(SYS_UART_TRANSPORT,(uint32_t)info,0u,0u)!=0u||info[0]!=0u||info[1]!=0u)UART_TEST_FAIL("EMPTY");
    for(i=0u;i<255u;i++)tx[i]=(uint8_t)i;
    if(syscall3(SYS_UART_TRANSPORT,(uint32_t)tx,5u,255u)!=255u)UART_TEST_FAIL("RX INJECT");
    if(syscall3(SYS_UART_TRANSPORT,(uint32_t)info,0u,0u)!=0u||info[0]!=255u)UART_TEST_FAIL("RX COUNT");
    if(syscall3(SYS_UART_TRANSPORT,(uint32_t)rx,1u,255u)!=255u)UART_TEST_FAIL("RX READ");
    for(i=0u;i<255u;i++)if(rx[i]!=(uint8_t)i)UART_TEST_FAIL("RX ORDER");
    if(syscall3(SYS_UART_TRANSPORT,(uint32_t)tx,5u,255u)!=255u)UART_TEST_FAIL("RX REFILL");
    r=syscall3(SYS_UART_TRANSPORT,(uint32_t)tx,5u,1u);if(r!=1u)UART_TEST_FAIL("RX OVERFLOW INJECT");
    if(syscall3(SYS_UART_TRANSPORT,(uint32_t)info,0u,0u)!=0u||info[4]!=1u)UART_TEST_FAIL("RX OVERFLOW COUNT");
    if(syscall3(SYS_UART_TRANSPORT,(uint32_t)rx,1u,255u)!=255u)UART_TEST_FAIL("RX DRAIN");
    for(i=0u;i<200u;i++)tx[i]=(uint8_t)(255u-i);
    if(syscall3(SYS_UART_TRANSPORT,(uint32_t)tx,2u,200u)!=200u)UART_TEST_FAIL("TX QUEUE");
    if(syscall3(SYS_UART_TRANSPORT,(uint32_t)rx,6u,200u)!=200u)UART_TEST_FAIL("TX DRAIN");
    for(i=0u;i<200u;i++)if(rx[i]!=tx[i])UART_TEST_FAIL("TX ORDER");
    if(syscall3(SYS_UART_TRANSPORT,(uint32_t)diag,11u,0u)!=0u||diag[0]!=0u||diag[3]!=0u||diag[4]!=0u)UART_TEST_FAIL("TEST HOOK ISOLATION");
    if(syscall3(SYS_UART_TRANSPORT,0x500000u,1u,1u)!=0xffffffffu)UART_TEST_FAIL("BAD POINTER");
    t0=syscall3(SYS_UART_TRANSPORT,0u,4u,0u);for(i=0u;i<20000u;i++)__asm__ volatile("":::"memory");t1=syscall3(SYS_UART_TRANSPORT,0u,4u,0u);if((uint32_t)(t1-t0)==0u)UART_TEST_FAIL("SERIAL TIME");
    if(syscall3(SYS_UART_TRANSPORT,0u,3u,0u)!=0u)UART_TEST_FAIL("FINAL RESET");
    put("TEST EXPECT UART_TRANSPORT: OK\n");
#undef UART_TEST_FAIL
    return 1u;
}
__attribute__((section(".usertext"),noinline)) static uint32_t test_expect_data_channel(void){
    uint32_t req[11],out[14],info[7],ri[5],r1,r2,g1,g2,i,r;
#define DATA_TEST_FAIL(msg) do{put("TEST EXPECT DATA_CHANNEL FAIL: " msg "\n");return 0u;}while(0)
    if(syscall3(SYS_DATA_CHANNEL,0u,6u,0u)!=0u)DATA_TEST_FAIL("RESET");
    g1=syscall3(SYS_DATA_CHANNEL,0u,1u,0u);if(g1==0u||g1==0xffffffffu)DATA_TEST_FAIL("BEGIN");
    r1=syscall3(SYS_DATA_CHANNEL,0u,3u,0u);r2=syscall3(SYS_DATA_CHANNEL,0u,3u,0u);if(r1==0xffffffffu||r2==0xffffffffu||r1==r2)DATA_TEST_FAIL("READERS");
    for(i=0u;i<11u;i++)req[i]=0u;req[0]=0u;req[1]=0x55aau;req[2]=12u;req[3]=0x11111111u;req[4]=0x22222222u;req[5]=0x33333333u;
    if(syscall3(SYS_DATA_CHANNEL,(uint32_t)req,2u,0u)!=1u)DATA_TEST_FAIL("PUBLISH1");
    if(syscall3(SYS_DATA_CHANNEL,(uint32_t)out,4u,r1)!=1u||out[1]!=g1||out[2]!=1u||out[4]!=0x55aau||out[5]!=12u||out[6]!=0x11111111u||out[7]!=0x22222222u||out[8]!=0x33333333u)DATA_TEST_FAIL("READ1");
    if(syscall3(SYS_DATA_CHANNEL,(uint32_t)out,4u,r1)!=0u)DATA_TEST_FAIL("EMPTY");
    if(syscall3(SYS_DATA_CHANNEL,(uint32_t)out,4u,r2)!=1u||out[2]!=1u)DATA_TEST_FAIL("INDEPENDENT");
    for(i=0u;i<10u;i++){req[1]=i;req[2]=4u;req[3]=0x1000u+i;if(syscall3(SYS_DATA_CHANNEL,(uint32_t)req,2u,0u)==0xffffffffu)DATA_TEST_FAIL("FILL");}
    /* r2 consumed seq1 then fell behind by ten publishes: explicit overrun first, sample second. */
    if(syscall3(SYS_DATA_CHANNEL,(uint32_t)out,4u,r2)!=2u)DATA_TEST_FAIL("OVERRUN");
    if(syscall3(SYS_DATA_CHANNEL,(uint32_t)ri,7u,r2)!=0u||ri[3]!=1u)DATA_TEST_FAIL("OVERRUN COUNT");
    if(syscall3(SYS_DATA_CHANNEL,(uint32_t)out,4u,r2)!=1u||out[2]!=4u)DATA_TEST_FAIL("OLDEST AFTER OVERRUN");
    g2=syscall3(SYS_DATA_CHANNEL,0u,1u,0u);if(g2==g1||g2==0xffffffffu)DATA_TEST_FAIL("GENERATION BEGIN");
    if(syscall3(SYS_DATA_CHANNEL,(uint32_t)out,4u,r1)!=3u)DATA_TEST_FAIL("GENERATION NOTICE");
    req[1]=0x77u;req[2]=4u;req[3]=0xabcdef01u;if(syscall3(SYS_DATA_CHANNEL,(uint32_t)req,2u,0u)!=1u)DATA_TEST_FAIL("PUBLISH GEN2");
    if(syscall3(SYS_DATA_CHANNEL,(uint32_t)out,4u,r1)!=1u||out[1]!=g2||out[2]!=1u||out[6]!=0xabcdef01u)DATA_TEST_FAIL("READ GEN2");
    if(syscall3(SYS_DATA_CHANNEL,0x500000u,2u,0u)!=0xffffffffu)DATA_TEST_FAIL("BAD POINTER");
    if(syscall3(SYS_DATA_CHANNEL,r1,5u,0u)!=0u||syscall3(SYS_DATA_CHANNEL,r2,5u,0u)!=0u)DATA_TEST_FAIL("CLOSE");
    if(syscall3(SYS_DATA_CHANNEL,(uint32_t)info,0u,0u)!=0u||info[0]!=g2||info[3]!=1u||info[5]!=8u||info[6]!=32u)DATA_TEST_FAIL("INFO");
    if(syscall3(SYS_DATA_CHANNEL,0u,6u,0u)!=0u)DATA_TEST_FAIL("FINAL RESET");
    put("TEST EXPECT DATA_CHANNEL: OK\n");return 1u;
#undef DATA_TEST_FAIL
}

typedef unsigned short uint16_t;
extern uint16_t mb_crc16(const uint8_t*,uint32_t);
extern uint32_t mb_build_read_input(uint8_t,uint16_t,uint16_t,uint8_t*,uint32_t);
extern uint32_t mb_parse_read_input(const uint8_t*,uint32_t,uint8_t,uint16_t,uint16_t*,uint32_t,uint32_t*);
struct mb_master_state{uint32_t deadline_us,timeout_us,retries_left,attempts;};
extern void mb_master_start(struct mb_master_state*,uint32_t,uint32_t,uint32_t);
extern uint32_t mb_master_poll(struct mb_master_state*,uint32_t);
__attribute__((section(".usertext"),noinline)) static uint32_t test_expect_modbus_rtu(void){
 uint8_t q[8],f[32];uint16_t regs[8],crc;uint32_t r,e=0;struct mb_master_state st;
#define MBFAIL(x) do{put("TEST EXPECT MODBUS_RTU FAIL: " x "\n");return 0u;}while(0)
 if(mb_crc16((const uint8_t*)"123456789",9u)!=0x4b37u)MBFAIL("CRC VECTOR");
 if(mb_build_read_input(1u,0x0010u,3u,q,8u)!=8u||q[0]!=1u||q[1]!=4u||q[2]!=0u||q[3]!=0x10u||q[4]!=0u||q[5]!=3u)MBFAIL("BUILD F04");
 f[0]=1;f[1]=4;f[2]=6;f[3]=0x12;f[4]=0x34;f[5]=0xab;f[6]=0xcd;f[7]=0;f[8]=1;crc=mb_crc16(f,9u);f[9]=(uint8_t)crc;f[10]=(uint8_t)(crc>>8);
 r=mb_parse_read_input(f,11u,1u,3u,regs,8u,&e);if(r!=1u||regs[0]!=0x1234u||regs[1]!=0xabcdu||regs[2]!=1u)MBFAIL("PARSE F04");
 if(mb_parse_read_input(f,7u,1u,3u,regs,8u,&e)!=0u)MBFAIL("PARTIAL");
 f[10]^=1u;if(mb_parse_read_input(f,11u,1u,3u,regs,8u,&e)!=0xfffffff1u)MBFAIL("BAD CRC");f[10]^=1u;
 f[0]=1;f[1]=0x84;f[2]=2;crc=mb_crc16(f,3u);f[3]=(uint8_t)crc;f[4]=(uint8_t)(crc>>8);e=0;if(mb_parse_read_input(f,5u,1u,3u,regs,8u,&e)!=0xfffffff3u||e!=2u)MBFAIL("EXCEPTION");
 mb_master_start(&st,1000u,500u,2u);if(mb_master_poll(&st,1499u)!=0u)MBFAIL("EARLY TIMEOUT");if(mb_master_poll(&st,1500u)!=2u||st.attempts!=2u)MBFAIL("RETRY1");if(mb_master_poll(&st,2000u)!=2u||st.attempts!=3u)MBFAIL("RETRY2");if(mb_master_poll(&st,2500u)!=0xfffffff4u)MBFAIL("TIMEOUT");
 put("TEST EXPECT MODBUS_RTU: OK\n");return 1u;
#undef MBFAIL
}

struct sonar_sample {int x_mm,y_mm,z_mm;uint32_t status;};
extern uint32_t sonar_decode_xyz(const uint16_t*,uint32_t,struct sonar_sample*);
extern uint32_t sonar_pack_channel(uint32_t,const struct sonar_sample*,uint32_t*);
__attribute__((section(".usertext"),noinline)) static uint32_t test_expect_sonar_driver(void){
 uint8_t f[32];uint16_t regs[6],crc;uint32_t r,e=0,q[11];struct sonar_sample s;
#define SONFAIL(x) do{put("TEST EXPECT SONAR_DRIVER FAIL: " x "\n");return 0u;}while(0)
 /* Normal F04 response: X=123456, Y=-654321, Z=2000000000 mm. */
 f[0]=1;f[1]=4;f[2]=12;f[3]=0x00;f[4]=0x01;f[5]=0xe2;f[6]=0x40;f[7]=0xff;f[8]=0xf6;f[9]=0x04;f[10]=0x0f;f[11]=0x77;f[12]=0x35;f[13]=0x94;f[14]=0x00;crc=mb_crc16(f,15u);f[15]=(uint8_t)crc;f[16]=(uint8_t)(crc>>8);
 r=mb_parse_read_input(f,17u,1u,6u,regs,6u,&e);if(r!=1u)SONFAIL("F04 PARSE");if(sonar_decode_xyz(regs,6u,&s)!=1u||s.x_mm!=123456||s.y_mm!=-654321||s.z_mm!=2000000000)SONFAIL("XYZ DECODE");if(sonar_pack_channel(0u,&s,q)!=1u||q[0]!=0u||q[1]!=0u||q[2]!=12u||q[3]!=123456u||q[4]!=(uint32_t)-654321||q[5]!=2000000000u)SONFAIL("CHANNEL PACK");
 if(mb_parse_read_input(f,10u,1u,6u,regs,6u,&e)!=0u)SONFAIL("PARTIAL");f[16]^=1u;if(mb_parse_read_input(f,17u,1u,6u,regs,6u,&e)!=0xfffffff1u)SONFAIL("BAD CRC");f[16]^=1u;
 f[0]=1;f[1]=0x84;f[2]=2;crc=mb_crc16(f,3u);f[3]=(uint8_t)crc;f[4]=(uint8_t)(crc>>8);e=0;if(mb_parse_read_input(f,5u,1u,6u,regs,6u,&e)!=0xfffffff3u||e!=2u)SONFAIL("EXCEPTION");
 if(sonar_decode_xyz(regs,5u,&s)!=0xffffffe1u)SONFAIL("REGISTER COUNT");
 put("TEST EXPECT SONAR_DRIVER: OK\n");return 1u;
#undef SONFAIL
}

struct tlog_header {uint32_t magic,version,generation,next_sequence,write_index,valid_count,capacity,crc;};
struct tlog_record {uint32_t sequence,timestamp_us;int32_t x_mm,y_mm,z_mm;uint32_t status,generation,crc;};
extern void tlog_header_make(struct tlog_header*,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern uint32_t tlog_header_valid(const struct tlog_header*,uint32_t);
extern void tlog_record_make(struct tlog_record*,uint32_t,uint32_t,int32_t,int32_t,int32_t,uint32_t,uint32_t);
extern uint32_t tlog_record_valid(const struct tlog_record*);
extern uint32_t tlog_newer(uint32_t,uint32_t);
#define TLOG_CAPACITY 128u
#define TLOG_RECORD_BYTES 32u
#define TLOGFAIL(x) do{put("TEST EXPECT TELEMETRY_LOG FAIL: " x "\n");return 0u;}while(0)
__attribute__((section(".usertext"),noinline)) static uint32_t test_sonar_log_valid(uint32_t want){
 uint32_t fd,q[3],va,vb,active,idx;struct tlog_header a,b;struct tlog_record rec;
 if(want>TLOG_CAPACITY)return 0u;fd=sys_file_open("/SONAR.LOG",FAT16_MODE_READ);if(fd==0xffffffffu)return 0u;
 q[0]=fd;q[1]=(uint32_t)&a;q[2]=sizeof(a);if(syscall3(SYS_FILE_PREAD,(uint32_t)q,0u,0u)!=sizeof(a)){sys_file_close(fd);return 0u;}
 q[1]=(uint32_t)&b;if(syscall3(SYS_FILE_PREAD,(uint32_t)q,32u,0u)!=sizeof(b)){sys_file_close(fd);return 0u;}
 va=tlog_header_valid(&a,TLOG_CAPACITY);vb=tlog_header_valid(&b,TLOG_CAPACITY);if(!va&&!vb){sys_file_close(fd);return 0u;}
 active=(va&&vb)?(tlog_newer(b.generation,a.generation)?1u:0u):(vb?1u:0u);if((active?b.valid_count:a.valid_count)!=want){sys_file_close(fd);return 0u;}
 if(want){uint32_t wi=active?b.write_index:a.write_index;idx=(wi+TLOG_CAPACITY-1u)%TLOG_CAPACITY;q[1]=(uint32_t)&rec;q[2]=sizeof(rec);if(syscall3(SYS_FILE_PREAD,(uint32_t)q,64u+idx*TLOG_RECORD_BYTES,0u)!=sizeof(rec)||!tlog_record_valid(&rec)){sys_file_close(fd);return 0u;}}
 sys_file_close(fd);return 1u;
}
/* FIX60ZEE: a live cyclic logger advances concurrently with the shell.  Exact
   valid_count is appropriate for a stopped/saturated log (e.g. 128), but an
   intermediate exact value such as 8 is a race.  This assertion proves that
   at least N committed records exist and that the selected tail record is
   structurally valid. */
__attribute__((section(".usertext"),noinline)) static uint32_t test_sonar_log_min(uint32_t want){
 uint32_t fd,q[3],va,vb,active,idx,count;struct tlog_header a,b;struct tlog_record rec;
 if(want>TLOG_CAPACITY)return 0u;fd=sys_file_open("/SONAR.LOG",FAT16_MODE_READ);if(fd==0xffffffffu)return 0u;
 q[0]=fd;q[1]=(uint32_t)&a;q[2]=sizeof(a);if(syscall3(SYS_FILE_PREAD,(uint32_t)q,0u,0u)!=sizeof(a)){sys_file_close(fd);return 0u;}
 q[1]=(uint32_t)&b;if(syscall3(SYS_FILE_PREAD,(uint32_t)q,32u,0u)!=sizeof(b)){sys_file_close(fd);return 0u;}
 va=tlog_header_valid(&a,TLOG_CAPACITY);vb=tlog_header_valid(&b,TLOG_CAPACITY);if(!va&&!vb){sys_file_close(fd);return 0u;}
 active=(va&&vb)?(tlog_newer(b.generation,a.generation)?1u:0u):(vb?1u:0u);count=active?b.valid_count:a.valid_count;if(count<want){sys_file_close(fd);return 0u;}
 if(count){uint32_t wi=active?b.write_index:a.write_index;idx=(wi+TLOG_CAPACITY-1u)%TLOG_CAPACITY;q[1]=(uint32_t)&rec;q[2]=sizeof(rec);if(syscall3(SYS_FILE_PREAD,(uint32_t)q,64u+idx*TLOG_RECORD_BYTES,0u)!=sizeof(rec)||!tlog_record_valid(&rec)){sys_file_close(fd);return 0u;}}
 sys_file_close(fd);return 1u;
}
__attribute__((section(".usertext"),noinline)) static uint32_t test_expect_telemetry_log(void){
 const char name[]="TLOG.TMP";uint8_t fill[128],rd[16],pat[8];uint32_t q[3],fd,i,size;struct tlog_header a,b;struct tlog_record rec;
 (void)sys_file_delete(name);for(i=0u;i<sizeof(fill);i++)fill[i]=0u;for(i=0u;i<sizeof(pat);i++)pat[i]=(uint8_t)(0xa0u+i);
 fd=sys_file_open(name,FAT16_MODE_READ|FAT16_MODE_WRITE|FAT16_MODE_CREATE|FAT16_MODE_TRUNC);if(fd==0xffffffffu)TLOGFAIL("CREATE");
 if(sys_file_write(fd,fill,sizeof(fill))!=sizeof(fill)){sys_file_close(fd);TLOGFAIL("PREALLOC");}
 size=sys_file_size(name);if(size!=sizeof(fill)){sys_file_close(fd);TLOGFAIL("SIZE INITIAL");}
 q[0]=fd;q[1]=(uint32_t)pat;q[2]=sizeof(pat);if(syscall3(SYS_FILE_PWRITE,(uint32_t)q,40u,0u)!=sizeof(pat)){sys_file_close(fd);TLOGFAIL("PWRITE");}
 if(sys_file_read(fd,rd,1u)!=0u){sys_file_close(fd);TLOGFAIL("POSITION PRESERVE");} /* pos remained EOF */
 q[1]=(uint32_t)rd;q[2]=sizeof(pat);if(syscall3(SYS_FILE_PREAD,(uint32_t)q,40u,0u)!=sizeof(pat)){sys_file_close(fd);TLOGFAIL("PREAD");}for(i=0u;i<sizeof(pat);i++)if(rd[i]!=pat[i]){sys_file_close(fd);TLOGFAIL("DATA");}
 if(syscall3(SYS_FILE_PWRITE,(uint32_t)q,124u,0u)!=0xffffffffu){sys_file_close(fd);TLOGFAIL("NO GROW BOUND");}
 if(sys_file_size(name)!=sizeof(fill)){sys_file_close(fd);TLOGFAIL("NO GROW SIZE");}if(sys_file_close(fd)!=0u)TLOGFAIL("CLOSE");
 tlog_header_make(&a,10u,22u,3u,7u,128u);if(!tlog_header_valid(&a,128u))TLOGFAIL("HEADER CRC");b=a;b.generation=11u;b.crc=0u;tlog_header_make(&b,11u,23u,4u,8u,128u);if(!tlog_newer(b.generation,a.generation))TLOGFAIL("HEADER SELECT");a.crc^=1u;if(tlog_header_valid(&a,128u)||!tlog_header_valid(&b,128u))TLOGFAIL("HEADER CORRUPTION");
 tlog_record_make(&rec,99u,123456u,1000,-2000,3000,7u,5u);if(!tlog_record_valid(&rec))TLOGFAIL("RECORD CRC");rec.y_mm++;if(tlog_record_valid(&rec))TLOGFAIL("RECORD CORRUPTION");
 {uint32_t wi=0u,cnt=0u;for(i=0u;i<259u;i++){wi=(wi+1u)%128u;if(cnt<128u)cnt++;}if(wi!=3u||cnt!=128u)TLOGFAIL("WRAP");}
 if(sys_file_delete(name)!=0u)TLOGFAIL("DELETE");put("TEST EXPECT TELEMETRY_LOG: OK\n");return 1u;
}

__attribute__((section(".usertext"),noinline)) static uint32_t test_expect_uart_full_poll(void){uint32_t ier,t0,t1,i;if(syscall3(SYS_UART_TRANSPORT,1u,7u,0u)!=0u)return 0u;ier=syscall3(SYS_UART_TRANSPORT,0u,9u,0u);if(ier!=0u){(void)syscall3(SYS_UART_TRANSPORT,0u,7u,0u);return 0u;}t0=syscall3(SYS_UART_TRANSPORT,0u,10u,0u);for(i=0u;i<200000u;i++)__asm__ volatile("":::"memory");t1=syscall3(SYS_UART_TRANSPORT,0u,10u,0u);(void)syscall3(SYS_UART_TRANSPORT,0u,7u,0u);return (uint32_t)(t1-t0)<=1000000u;}
__attribute__((section(".usertext"),noinline)) static uint32_t test_expect_command(char*p){skip(&p);if(eq(p,"SAFE_DENY_RT"))return test_expect_safe_deny_rt();if(eq(p,"SAFE_DENY_MT_START"))return test_expect_safe_deny_mt_start();if(eq(p,"RM_GUARDS"))return test_expect_rm_guards();if(eq(p,"UART_TRANSPORT"))return test_expect_uart_transport();if(eq(p,"UART_FULL_POLL"))return test_expect_uart_full_poll();if(eq(p,"DATA_CHANNEL"))return test_expect_data_channel();if(eq(p,"MODBUS_RTU"))return test_expect_modbus_rtu();if(eq(p,"SONAR_DRIVER"))return test_expect_sonar_driver();if(eq(p,"TELEMETRY_LOG"))return test_expect_telemetry_log();put("TEST EXPECT: unsupported expectation\n");return 0u;}

__attribute__((section(".usertext"),noinline)) static uint32_t test_run_command(char*line){
    trim(line);
    if(prefix(line,"test ")||eq(line,"test")){put("TEST RUN: nested test is not allowed\n");return 0u;}
    execute_line(line);
    return 1u;
}

/* FIX60X: test execution is restored to the FIX60T console-only model.
 * No test result is buffered for a log and no TST.LOG file is created.
 * The no-argument form only reads /TST/ACCEPT.TXT and invokes the same
 * ordinary FIX60T test interpreter for each listed TEST*.TST file. */
__attribute__((section(".usertext"),noinline)) static void test_summary_record(char *path,uint32_t line){
    uint32_t i=0u,j=0u;
    if(!test_accept_active)return;
    if(test_fail_summary_count>=TEST_FAIL_SUMMARY_MAX){test_fail_summary_overflow=1u;return;}
    while(path[i])i++;
    while(i&&path[i-1u]!='/')i--;
    while(path[i]&&j+1u<sizeof(test_fail_summary[0].name))test_fail_summary[test_fail_summary_count].name[j++]=path[i++];
    test_fail_summary[test_fail_summary_count].name[j]=0;
    test_fail_summary[test_fail_summary_count].line=line;
    test_fail_summary_count++;
}

__attribute__((section(".usertext"),noinline)) static uint32_t test_run_file_console(char *path){
    char *line=test_line,*chunk=test_chunk;uint32_t fd,pos=0u,len=0u,r,ll=0u,longline=0u,lineno=0u,pass=0u,fail=0u,source_hash=2166136261u,source_bytes=0u,z;char nb[16];
    test_stage_valid=0u;
    fd=sys_file_open(path,FAT16_MODE_READ);if(fd==0xffffffffu){put("TEST: file not found\n");test_summary_record(path,0u);return 1u;}
    put("TEST: ");put(path);put("\n");
    for(;;){
        if(pos>=len){r=sys_file_read(fd,chunk,256u);if(r==0xffffffffu){put("TEST: read FAIL\n");test_summary_record(path,lineno+1u);fail++;break;}if(!r)break;for(z=0u;z<r;z++){source_hash^=(uint8_t)chunk[z];source_hash*=16777619u;}source_bytes+=r;len=r;pos=0u;}
        while(pos<len){char c=chunk[pos++];if(c=='\n'){
            lineno++;if(longline){put("TEST: line too long\n");test_summary_record(path,lineno);fail++;}else{line[ll]=0;trim(line);if(line[0]&&line[0]!='#'){
                if(prefix(line,"PRINT ")){put(line+6);put("\n");}
                else if(prefix(line,"RUN ")){put("TEST RUN> ");put(line+4);put("\n");if(!test_run_command(line+4)){put("TEST HARNESS FAIL line ");dec(lineno,nb);put(nb);put(": RUN rejected: ");put(line+4);put("\n");test_summary_record(path,lineno);fail++;}}
                else if(prefix(line,"START ")){if(test_start_command(line+6))pass++;else{test_summary_record(path,lineno);fail++;}}
                else if(prefix(line,"TYPE ")){if(test_type_command(line+5))pass++;else{test_summary_record(path,lineno);fail++;}}
                else if(prefix(line,"KEY ")){if(test_key_command(line+4))pass++;else{test_summary_record(path,lineno);fail++;}}
                else if(prefix(line,"EXPECT ")){if(test_expect_command(line+7))pass++;else{test_summary_record(path,lineno);fail++;}}
                else if(eq(line,"WAIT LAST")){if(!last_spawn_pid){put("TEST FAIL: no last spawn PID\n");test_summary_record(path,lineno);fail++;}else{char wb[16];test_last_wait_valid=0u;dec(last_spawn_pid,wb);wait_command(wb);if(test_last_wait_valid)pass++;else{put("TEST FAIL: WAIT LAST did not obtain process result\n");test_summary_record(path,lineno);fail++;}}}
                else if(prefix(line,"ASSERT ")){if(test_assert(line+7)){put("TEST PASS line ");dec(lineno,nb);put(nb);put("\n");pass++;}else{put("TEST FAIL line ");dec(lineno,nb);put(nb);put(": ");put(line+7);put("\n");test_summary_record(path,lineno);fail++;}}
                else{put("TEST FAIL line ");dec(lineno,nb);put(nb);put(": unknown directive\n");test_summary_record(path,lineno);fail++;}
            }}ll=0u;longline=0u;
        }else if(c!='\r'){if(!longline){if(ll+1u<400u)line[ll++]=c;else longline=1u;}}}
    }
    if(ll||longline){lineno++;if(longline){put("TEST: line too long\n");test_summary_record(path,lineno);fail++;}else{line[ll]=0;trim(line);if(line[0]&&line[0]!='#'){if(prefix(line,"PRINT ")){put(line+6);put("\n");}else if(prefix(line,"RUN ")){put("TEST RUN> ");put(line+4);put("\n");if(!test_run_command(line+4)){put("TEST HARNESS FAIL line ");dec(lineno,nb);put(nb);put(": RUN rejected: ");put(line+4);put("\n");test_summary_record(path,lineno);fail++;}}else if(prefix(line,"START ")){if(test_start_command(line+6))pass++;else{test_summary_record(path,lineno);fail++;}}else if(prefix(line,"TYPE ")){if(test_type_command(line+5))pass++;else{test_summary_record(path,lineno);fail++;}}else if(prefix(line,"KEY ")){if(test_key_command(line+4))pass++;else{test_summary_record(path,lineno);fail++;}}else if(prefix(line,"EXPECT ")){if(test_expect_command(line+7))pass++;else{test_summary_record(path,lineno);fail++;}}else if(eq(line,"WAIT LAST")){if(!last_spawn_pid){put("TEST FAIL: no last spawn PID\n");test_summary_record(path,lineno);fail++;}else{char wb[16];test_last_wait_valid=0u;dec(last_spawn_pid,wb);wait_command(wb);if(test_last_wait_valid)pass++;else{put("TEST FAIL: WAIT LAST did not obtain process result\n");test_summary_record(path,lineno);fail++;}}}else if(prefix(line,"ASSERT ")){if(test_assert(line+7)){put("TEST PASS line ");dec(lineno,nb);put(nb);put("\n");pass++;}else{put("TEST FAIL line ");dec(lineno,nb);put(nb);put(": ");put(line+7);put("\n");test_summary_record(path,lineno);fail++;}}else{put("TEST FAIL line ");dec(lineno,nb);put(nb);put(": unknown directive\n");test_summary_record(path,lineno);fail++;}}}}
    sys_file_close(fd);test_stage_valid=0u;if(fail){put("TEST SOURCE: bytes=");dec(source_bytes,nb);put(nb);put(" fnv1a32=");dec(source_hash,nb);put(nb);put("\n");}put("TEST RESULT: PASS=");dec(pass,nb);put(nb);put(" FAIL=");dec(fail,nb);put(nb);put(fail?" FAILED\n":" PASSED\n");return fail;
}

__attribute__((section(".usertext"),noinline)) static void test_print_failure_summary(uint32_t tests,uint32_t failed){
    uint32_t i;char nb[16];
    put("\n================ TEST SUMMARY =================\n");
    if(test_fail_summary_count){
        put("FAILED TESTS / LINES:\n");
        for(i=0u;i<test_fail_summary_count;i++){
            put(test_fail_summary[i].name);put(": ");
            if(test_fail_summary[i].line){put("FAIL line ");dec(test_fail_summary[i].line,nb);put(nb);}
            else put("FAIL (file open)");
            put("\n");
        }
        if(test_fail_summary_overflow)put("WARNING: failure summary overflow; additional FAIL lines omitted\n");
    }
    put("TESTS: ");dec(tests,nb);put(nb);put("  PASSED: ");dec(tests-failed,nb);put(nb);put("  FAILED: ");dec(failed,nb);put(nb);put("\n");
    put(failed?"TEST: FAILED\n":"TEST: PASSED\n");
}

__attribute__((section(".usertext"),noinline)) static uint32_t test_run_accept_console(void){
    uint32_t fd,r,n=0u,failed=0u,tests=0u;char c,name[32],path[40];
    test_fail_summary_count=0u;test_fail_summary_overflow=0u;test_accept_active=1u;
    fd=sys_file_open("/TST/ACCEPT.TXT",FAT16_MODE_READ);if(fd==0xffffffffu){test_accept_active=0u;put("TEST: /TST/ACCEPT.TXT not found\n");return 1u;}
    put("TEST: acceptance started (console only)\n");
    for(;;){
        r=sys_file_read(fd,&c,1u);if(r==0xffffffffu){put("TEST: ACCEPT.TXT read FAIL\n");failed++;break;}
        if(r==0u||c=='\n'){
            if(n){name[n]=0;trim(name);if(name[0]&&name[0]!='#'){
                uint32_t i=0u,j=0u;path[j++]='/';path[j++]='T';path[j++]='S';path[j++]='T';path[j++]='/';tests++;
                while(name[i]&&j+1u<sizeof(path))path[j++]=name[i++];path[j]=0;
                if(name[i]){put("TEST: invalid manifest name\n");failed++;}else if(test_run_file_console(path))failed++;
            }}n=0u;if(r==0u)break;
        }else if(c!='\r'){if(n+1u<sizeof(name))name[n++]=c;else{put("TEST: ACCEPT.TXT line too long\n");failed++;n=0u;}}
    }
    sys_file_close(fd);test_accept_active=0u;test_print_failure_summary(tests,failed);return failed;
}

__attribute__((section(".usertext"),noinline)) static void test_command(char*p){
    char path[40];uint32_t i=0u,j=0u;
    skip(&p);if(test_runner_active){put("TEST: nested test is not allowed\n");return;}test_runner_active=1u;test_stage_valid=0u;
    if(!*p){(void)test_run_accept_console();test_runner_active=0u;return;}
    if(p[0]=='/')while(p[i]&&j+1u<sizeof(path))path[j++]=p[i++];
    else{path[j++]='/';path[j++]='T';path[j++]='S';path[j++]='T';path[j++]='/';while(p[i]&&j+1u<sizeof(path))path[j++]=p[i++];}
    path[j]=0;if(p[i])put("TEST: file name too long\n");else(void)test_run_file_console(path);test_runner_active=0u;
}

/*
 * v65.11: /AUTOSTART.SH — автоматический запуск команд после входа в Ring 3.
 * Файл ищется по абсолютному пути, поэтому механизм не зависит от cwd. Каждая
 * непустая строка проходит через тот же execute_line(), что и интерактивная
 * консоль. Пустые строки и строки, начинающиеся с '#', игнорируются. Поддержан
 * CR/LF. Максимальная длина команды равна 399 символам.
 */
#define AUTOSTART_NAME "/AUTOSTART.SH"
#define AUTOSTART_CHUNK 256u

__attribute__((section(".usertext"))) static uint32_t autostart_run(void){
    char line[400];
    char chunk[AUTOSTART_CHUNK];
    uint32_t fd,pos=0u,len=0u,r,line_len=0u,long_line=0u;
    fd=sys_file_open(AUTOSTART_NAME,FAT16_MODE_READ);
    if(fd==0xffffffffu)return 0u;
    put("AUTOSTART.SH: running\n");
    for(;;){
        if(pos>=len){
            r=sys_file_read(fd,chunk,sizeof(chunk));
            if(r==0xffffffffu){sys_file_close(fd);put("AUTOSTART.SH: read FAIL\n");return 1u;}
            if(r==0u)break;
            len=r;pos=0u;
        }
        while(pos<len){
            char c=chunk[pos++];
            if(c=='\n'){
                if(long_line){put("AUTOSTART.SH: line too long\n");}
                else{
                    line[line_len]=0;trim(line);
                    if(line[0]&&line[0]!='#'){
                        put("AUTOSTART> ");put(line);put("\n");
                        if(execute_line(line)){sys_file_close(fd);return 1u;}
                    }
                }
                line_len=0u;long_line=0u;
            }else if(c!='\r'){
                if(!long_line){
                    if(line_len+1u<sizeof(line))line[line_len++]=c;
                    else long_line=1u;
                }
            }
        }
    }
    if(line_len&&!long_line){
        line[line_len]=0;trim(line);
        if(line[0]&&line[0]!='#'){put("AUTOSTART> ");put(line);put("\n");execute_line(line);}
    }else if(long_line){put("AUTOSTART.SH: line too long\n");}
    sys_file_close(fd);
    return 1u;
}

__attribute__((section(".usertext"))) static void shell_loop(void){
    char line[400];
    for(;;){
        put("toy0> ");shell_f10_event=0u;readline(line,sizeof(line));
        if(shell_f10_event){
            if(shell_f10_event==2u){trim(line);if(prefix(line,"exec RTD.EXE "))execute_line(line);else if(line[0])put("F10: current non-RT command was not executed; use ENTER\n");}
            rt_launch_pending();continue;
        }
        trim(line);if(eq(line,""))continue;if(prefix(line,"test ")){test_command(line+5);continue;}if(eq(line,"test")){test_command(line+4);continue;}execute_line(line);
    }
}

__attribute__((section(".usertext"))) void shell_run(void){
    /* FIX43: report a valid marker left before the previous reset. */
    boot_diag_show();
    /* При отсутствии сценария сохраняем прежнее приветствие и help. */
    if(!autostart_run()){put("\nToy OS ring-3 shell. Commands use INT 80h.\n");help();}
    shell_loop();
}
