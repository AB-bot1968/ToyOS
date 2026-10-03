/*
 * LOADER.EXE — пользовательский оркестратор автоматической загрузки EXE1.
 *
 * Назначение:
 *   LOADER.EXE <PORT> RECV <FILE.EXE>
 *
 * Пример:
 *   exec LOADER.EXE 1 RECV FF.EXE
 *
 * Программа не загружает FF.EXE сама и не добавляет новый системный loader.
 * Она формирует существующую очередь EXE1 из двух элементов:
 *
 *   1. COMDRV.EXE PORT RECV FILE.EXE
 *   2. FILE.EXE
 *
 * В v57 ядро разрешает SYS_EXEC_QUEUE_ARGS вызывать из Ring-3 EXE1.
 * Поэтому после успешного SYS_EXIT(0) COMDRV очередь автоматически
 * передаёт управление загруженному FILE.EXE. При SYS_EXIT(1) COMDRV
 * очередь останавливается и FILE.EXE не запускается.
 *
 * Такой вариант сохраняет существующий EXE1 ABI и почти не меняет ядро:
 * добавлено только разрешение аргументной очереди из работающего EXE1
 * и остановка очереди по ненулевому коду SYS_EXIT.
 */
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;

/* Syscall 1 выводит одну логическую строку в консоль. */
#define SYS_CONSOLE_WRITE 1u
/* Syscall 12 завершает текущую Ring-3 программу; EBX содержит статус. */
#define SYS_EXIT 12u
/* Syscall 26 запускает очередь EXE1 с аргументами каждого элемента. */
#define SYS_EXEC_QUEUE_ARGS 26u

/* Один элемент аргументной очереди: имя EXE1 + три строки по 16 байт. */
#define QUEUE_NAME_SIZE 13u
#define EXEC_ARG_SIZE 16u
#define EXEC_ARG_COUNT 3u
#define EXEC_QUEUE_RECORD_SIZE (QUEUE_NAME_SIZE+(EXEC_ARG_SIZE*EXEC_ARG_COUNT))

/* Выполняем INT 80h с существующим ABI EAX/EBX/ECX/EDX. */
__attribute__((section(".usertext"))) static uint32_t syscall3(uint32_t n,uint32_t a,uint32_t b,uint32_t c){
    uint32_t r;
    __asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");
    return r;
}

/* Выводим строку через уже существующий SYS_CONSOLE_WRITE. */
__attribute__((section(".usertext"))) static void put(const char*s){
    uint32_t n=0;
    while(s[n])n++;
    syscall3(SYS_CONSOLE_WRITE,(uint32_t)s,n,0);
}

/* Сравниваем две короткие ASCII-строки без libc. */
__attribute__((section(".usertext"))) /* v67.3: LOADER получает RECV через EXE1 ABI, поэтому нормализуем только
   текстовый параметр направления. Имя файла и номер порта передаются дальше
   без семантического изменения; FAT16 сама работает без учёта регистра. */
static void upper_copy(char*d,const char*s,uint32_t cap){uint32_t i=0;
    if(!d||cap==0u)return;
    if(!s){d[0]=0;return;}
    while(s[i]&&i+1u<cap){char c=s[i];if(c>='a'&&c<='z')c=(char)(c-'a'+'A');d[i++]=c;}
    d[i]=0;
}

static int eq(const char*a,const char*b){
    uint32_t i=0;
    while(a[i]&&b[i]){
        if(a[i]!=b[i])return 0;
        i++;
    }
    return a[i]==0&&b[i]==0;
}

/* Копируем строку в фиксированное поле ABI с гарантированным NUL. */
__attribute__((section(".usertext"))) static int copy_field(char*d,const char*s,uint32_t cap){
    uint32_t i=0;
    if(!s||!s[0])return 0;
    while(s[i]&&i+1u<cap){
        d[i]=s[i];
        i++;
    }
    if(s[i]!=0)return 0;
    d[i]=0;
    while(++i<cap)d[i]=0;
    return 1;
}

/* Читаем три входных параметра EXE1 из EBX/ECX/EDX, заданных ядром. */
__attribute__((section(".usertext"))) static void get_args(const char**port,const char**dir,const char**file){
    __asm__ volatile("" : "=b"(*port), "=c"(*dir), "=d"(*file));
}

/* Создаём два последовательных задания: COMDRV, затем полученный EXE1. */
__attribute__((section(".usertext"))) static uint32_t start_receive(const char*port,const char*dir,const char*file){
    char jobs[EXEC_QUEUE_RECORD_SIZE*2u];
    char*job0=&jobs[0];
    char*job1=&jobs[EXEC_QUEUE_RECORD_SIZE];
    uint32_t i;
    for(i=0;i<sizeof(jobs);i++)jobs[i]=0;
    if(!copy_field(job0,"COMDRV.EXE",QUEUE_NAME_SIZE))return 0xffffffffu;
    if(!copy_field(job0+QUEUE_NAME_SIZE,port,EXEC_ARG_SIZE))return 0xffffffffu;
    if(!copy_field(job0+QUEUE_NAME_SIZE+EXEC_ARG_SIZE,dir,EXEC_ARG_SIZE))return 0xffffffffu;
    if(!copy_field(job0+QUEUE_NAME_SIZE+(2u*EXEC_ARG_SIZE),file,EXEC_ARG_SIZE))return 0xffffffffu;
    if(!copy_field(job1,file,QUEUE_NAME_SIZE))return 0xffffffffu;
    return syscall3(SYS_EXEC_QUEUE_ARGS,(uint32_t)jobs,2u,1u);
}

/* Точка входа Ring-3 LOADER.EXE. */
__attribute__((section(".usertext"))) void program_main(void){
    const char*port;
    const char*dir;
    const char*file;
    char dir_norm[EXEC_ARG_SIZE];
    uint32_t r;
    get_args(&port,&dir,&file);
    upper_copy(dir_norm,dir,sizeof(dir_norm));
    if(!port||!dir||!file||!port[0]||!dir[0]||!file[0]||!eq(dir_norm,"RECV")){
        put("LOADER: usage PORT RECV FILE.EXE\n");
        syscall3(SYS_EXIT,1u,0,0);
        for(;;){}
    }
    put("LOADER: receiving EXE1 and starting it\n");
    r=start_receive(port,dir_norm,file);
    if(r!=0){
        put("LOADER: queue start failed\n");
        syscall3(SYS_EXIT,1u,0,0);
    }
    for(;;){}
}

/* Стандартная точка входа EXE1 передаёт управление в C-функцию выше. */
__asm__(".section .start,\"ax\"\n.global _start\n_start:\njmp _program_main\n");
