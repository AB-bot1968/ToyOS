#include "path.h"

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;

/*
 * v62: минимальный набор памяти вместо зависимости от libc.
 * Ядро собирается freestanding/nostdinc, поэтому все вспомогательные
 * функции находятся здесь и не требуют стандартной библиотеки.
 */
static uint32_t path_len(const char *s){
    uint32_t n=0;
    if(!s)return 0;
    while(s[n])n++;
    return n;
}

static void path_copy(char *dst,const char *src,uint32_t n){
    uint32_t i;
    for(i=0;i<n;i++)dst[i]=src[i];
}

static int path_eq(const char *a,const char *b){
    uint32_t i=0;
    while(a[i]&&b[i]&&a[i]==b[i])i++;
    return a[i]==0&&b[i]==0;
}

/*
 * Проверка одного FAT 8.3 компонента.
 *
 * В основном наборе имён по-прежнему используется классическое FAT 8.3.
 * Единственное исключение проекта — системный файл AUTOSTART.SH: его длинное
 * имя хранится через LFN и специальный короткий alias, но во внешнем пути
 * shell остаётся именно AUTOSTART.SH. Для остальных файлов действуют BASE
 * длиной 1..8, EXT длиной 1..3. Точка внутри имени только
 * одна. Пробелы, табуляция, разделители и управляющие символы запрещаются.
 * Это синтаксическая проверка; FAT-атрибуты и существование объекта проверяет
 * уже файловая система.
 */
static int valid_83(const char *s,uint32_t n){
    uint32_t i=0,base=0,ext=0,dot=0;
    /* AUTOSTART.SH — специальное системное имя. На диске оно хранится через
       LFN-запись и короткий внутренний alias AUTOST~1.SH, но наружу shell
       видит именно фиксированное имя из требования проекта. */
    if(s&&n==12u){
        const char special[]="AUTOSTART.SH";
        uint32_t k;
        for(k=0;k<12u;k++){
            unsigned char c=(unsigned char)s[k];
            if(c>='a'&&c<='z')c=(unsigned char)(c-'a'+'A');
            if(c!=(unsigned char)special[k])break;
        }
        if(k==12u)return 1;
    }
    if(!s||n==0)return 0;
    while(i<n){
        unsigned char c=(unsigned char)s[i];
        if(c<0x20u||c=='/'||c=='\\'||c==' '||c=='\t')return 0;
        if(c=='.'){
            if(dot||base==0)return 0;
            dot=1;i++;continue;
        }
        if(!dot){if(base>=8u)return 0;base++;}
        else{if(ext>=3u)return 0;ext++;}
        i++;
    }
    return base>0u && (!dot||ext>0u);
}

int path_is_absolute(const char *path){
    return path&&path[0]=='/';
}

/*
 * Нормализация пути.
 *
 * На входе:
 *   cwd  — уже канонический текущий каталог, например "/BIN";
 *   path — абсолютный или относительный путь.
 *
 * На выходе всегда:
 *   "/" или "/COMPONENT/COMPONENT".
 *
 * Алгоритм специально использует фиксированные буферы: это важно для ядра,
 * где динамическая память и malloc не используются. Одновременно это даёт
 * детерминированное ограничение на глубину и длину пути.
 */
int path_normalize(const char *cwd,const char *path,char *out,unsigned int cap){
    char tmp[256];
    char parts[32][13];
    uint32_t len,pos=0,depth=0,i,total=1;

    if(!cwd||!path||!out||cap<2u||!cwd[0]||!path[0])return 0;
    if(cwd[0]!='/'||path_len(cwd)>=128u)return 0;

    len=path_len(path);
    if(len>=128u)return 0;

    if(path_is_absolute(path)){
        if(len+1u>sizeof(tmp))return 0;
        path_copy(tmp,path,len+1u);
    }else{
        uint32_t cl=path_len(cwd);
        if(cl+1u+len+1u>sizeof(tmp))return 0;
        path_copy(tmp,cwd,cl);
        tmp[cl]='/';
        path_copy(tmp+cl+1u,path,len+1u);
        len=cl+1u+len;
    }

    while(pos<len){
        uint32_t n=0;
        while(pos<len&&tmp[pos]=='/')pos++;
        if(pos>=len)break;
        while(pos<len&&tmp[pos]!='/'){
            if(n>=12u)return 0;
            parts[depth][n++]=tmp[pos++];
        }
        parts[depth][n]=0;

        if(path_eq(parts[depth],"."))continue;
        if(path_eq(parts[depth],"..")){
            if(depth)depth--;
            continue;
        }
        if(!valid_83(parts[depth],n))return 0;
        for(i=0;i<n;i++)if(parts[depth][i]>='a'&&parts[depth][i]<='z')parts[depth][i]-=32;
        depth++;
        if(depth>=32u)return 0;
    }

    if(cap<2u)return 0;
    out[0]='/';
    if(depth==0u){out[1]=0;return 1;}

    for(i=0;i<depth;i++){
        uint32_t n=path_len(parts[i]);
        if(total+n+(i?1u:0u)+1u>cap)return 0;
        if(i)out[total++]='/';
        path_copy(out+total,parts[i],n);
        total+=n;
    }
    out[total]=0;
    return 1;
}

int path_join(const char *left,const char *right,char *out,unsigned int cap){
    char tmp[256];
    uint32_t a,b;
    if(!left||!right||!out||!cap)return 0;
    if(path_is_absolute(right)){
        /* Абсолютный right намеренно имеет приоритет над left. */
        return path_normalize("/",right,out,cap);
    }
    a=path_len(left);b=path_len(right);
    if(a+b+2u>sizeof(tmp))return 0;
    path_copy(tmp,left,a);
    if(a&&tmp[a-1]!='/')tmp[a++]='/';
    path_copy(tmp+a,right,b+1u);
    return path_normalize("/",tmp,out,cap);
}

int path_parent(const char *path,char *out,unsigned int cap){
    char abs[128];
    uint32_t n,i;
    if(!path_normalize("/",path,abs,sizeof(abs)))return 0;
    n=path_len(abs);
    if(n<=1u){if(cap<2u)return 0;out[0]='/';out[1]=0;return 1;}
    i=n;
    while(i>1u&&abs[i-1u]!='/')i--;
    if(i<=1u){if(cap<2u)return 0;out[0]='/';out[1]=0;return 1;}
    if(i>cap)return 0;
    path_copy(out,abs,i-1u);
    out[i-1u]=0;
    return 1;
}

int path_basename(const char *path,char *out,unsigned int cap){
    char abs[128];
    uint32_t n,start,i;
    if(!path_normalize("/",path,abs,sizeof(abs)))return 0;
    n=path_len(abs);
    if(n<=1u)return 0;
    i=n;
    while(i>1u&&abs[i-1u]!='/')i--;
    start=i;
    if(n-start+1u>cap)return 0;
    path_copy(out,abs+start,n-start+1u);
    return 1;
}
