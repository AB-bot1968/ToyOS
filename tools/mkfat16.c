/*
 * mkfat16.c - построитель FAT16-образа для Toy OS v60.4.
 *
 * В v60 утилита понимает пути 8.3 вида:
 *     BIN/HELLO.EXE
 *     DOC/README.TXT
 *
 * Для каждого промежуточного компонента автоматически создаётся обычный
 * FAT16-каталог. Каталоги являются кластерными цепочками и содержат '.'/'..'.
 * LFN намеренно не создаётся: все компоненты должны укладываться в 8.3.
 *
 * Пример:
 *     mkfat16 disk.img build/README.TXT=README.TXT build/HELLO.EXE=BIN/HELLO.EXE build/NET.CFG=DOC/NET.CFG
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define SECTOR 512u
#define RESERVED 1u
#define FATS 2u
#define ROOT_ENTRIES 512u
#define ROOT_SECTORS 32u
#define MAX_FILES ROOT_ENTRIES
static uint32_t g_part_lba=0u;
static uint32_t g_volume_sectors=0u;
static uint32_t g_total_sectors=0u;
static uint32_t g_spf=0u;
#define PART_LBA g_part_lba
#define TOTAL_SECTORS g_total_sectors
#define SPF g_spf
#define DATA_LBA_REL (RESERVED + FATS*SPF + ROOT_SECTORS)
#define DATA_LBA (PART_LBA + DATA_LBA_REL)
#define DATA_CLUSTERS (g_volume_sectors-DATA_LBA_REL)
#define MAX_CLUSTER (DATA_CLUSTERS+1u)
#define FIRST_ALLOC_CLUSTER 3u
#define MAX_PATH 127u
#define MAX_DIRS 128u

struct host_file {
    const char *path;
    char dest[MAX_PATH+1u];
    size_t size;
    unsigned char *data;
    uint16_t first;
    uint32_t clusters;
};

struct host_dir {
    char path[MAX_PATH+1u];
    uint16_t first;
    uint16_t parent;
    uint16_t extra_first;
    uint16_t clusters;
};

static void le16(unsigned char *p, uint16_t v){p[0]=(unsigned char)v;p[1]=(unsigned char)(v>>8);}
static void le32(unsigned char *p, uint32_t v){le16(p,(uint16_t)v);le16(p+2,(uint16_t)(v>>16));}


/*
 * Возвращает имя файла без каталога хостовой файловой системы.
 *
 * Важный контракт mkfat16:
 *   HOSTFILE                 -> файл помещается в ROOT под своим именем;
 *   HOSTFILE=DIR/FILE.EXE    -> файл помещается по указанному пути FAT16.
 *
 * Ранее при отсутствии '=' в качестве назначения ошибочно использовался
 * полный путь HOSTFILE (например build/MT25.EXE). Из-за этого mkfat16
 * создавал каталоги BUILD/... внутри FAT16 и уже затем не мог разместить
 * все записи в одном кластере промежуточного каталога. На Windows это
 * проявлялось как остановка сборки сразу после создания MT25.EXE.
 */
static const char *host_basename(const char *path){
    const char *a=strrchr(path,'/');
    const char *b=strrchr(path,'\\');
    const char *p=a>b?a:b;
    return p?p+1:path;
}

static unsigned char *read_host_file(const char *path, size_t *size){
    FILE *f; long n; unsigned char *p;
    *size=0; f=fopen(path,"rb"); if(!f){perror(path);return 0;}
    if(fseek(f,0,SEEK_END)){fclose(f);return 0;} n=ftell(f);
    if(n<0||fseek(f,0,SEEK_SET)){fclose(f);return 0;}
    if(n==0){p=(unsigned char*)malloc(1);if(!p){fclose(f);return 0;}}
    else {p=(unsigned char*)malloc((size_t)n);if(!p){fprintf(stderr,"out of memory\n");fclose(f);return 0;}}
    if(n && fread(p,1,(size_t)n,f)!=(size_t)n){fprintf(stderr,"read failed: %s\n",path);free(p);fclose(f);return 0;}
    fclose(f);*size=(size_t)n;return p;
}

/* Проверяет и переводит один компонент пути в FAT 8.3. */
static int make83_component(const char *in, unsigned char out[11]){
    unsigned i=0,j=0;
    if(!in||!in[0])return 0;
    /* AUTOSTART.SH — единственное системное имя проекта, длиннее 8 символов.
       FAT16 хранит его через LFN-запись и короткий alias AUTOST~1.SH. */
    if(strcmp(in,"AUTOSTART.SH")==0){
        memcpy(out,"AUTOST~1SH ",11);
        return 1;
    }
    memset(out,' ',11);
    while(in[i]&&in[i]!='.'){
        unsigned char c=(unsigned char)in[i++];
        if(c=='/'||c=='\\'||c==' '||c=='\t'||j>=8u)return 0;
        if(c>='a'&&c<='z')c=(unsigned char)(c-'a'+'A');
        out[j++]=c;
    }
    if(j==0)return 0;
    if(in[i]=='.'){
        i++;j=8;
        if(!in[i])return 0;
        while(in[i]){
            unsigned char c=(unsigned char)in[i++];
            if(c=='/'||c=='\\'||c==' '||c=='\t'||c=='.'||j>=11u)return 0;
            if(c>='a'&&c<='z')c=(unsigned char)(c-'a'+'A');
            out[j++]=c;
        }
    }
    return 1;
}

/* Возвращает компонент номер index из пути path, начиная с нулевого. */
static int path_component(const char *path,unsigned wanted,char out[13]){
    unsigned pos=0,n=0,current=0;
    while(path[pos]=='/'||path[pos]=='\\')pos++;
    while(path[pos]){
        n=0;
        while(path[pos]&&path[pos]!='/'&&path[pos]!='\\'){
            if(n>=12u)return 0;
            out[n++]=path[pos++];
        }
        out[n]=0;
        if(current==wanted)return n!=0;
        current++;
        while(path[pos]=='/'||path[pos]=='\\')pos++;
    }
    return 0;
}

/* Число компонентов пути. Пустые компоненты между '/' игнорируются. */
static unsigned path_count(const char *path){
    unsigned i=0,count=0;char c[13];
    while(path_component(path,count,c)){count++;i++;if(i>32u)return 0;}
    return count;
}

static int dir_equal(const struct host_dir *d,const char *path){return strcmp(d->path,path)==0;}

/* Ищет уже созданный каталог по его каноническому пути. */
static int find_dir(const struct host_dir dirs[],unsigned count,const char *path){
    unsigned i;for(i=0;i<count;i++)if(dir_equal(&dirs[i],path))return (int)i;return -1;
}

/* Собирает канонический путь из первых count компонентов исходного пути. */
static int prefix_path(const char *path,unsigned count,char out[MAX_PATH+1u]){
    unsigned i,pos=0;char c[13];
    out[0]=0;
    for(i=0;i<count;i++){
        unsigned j;
        if(!path_component(path,i,c))return 0;
        if(pos&&pos<MAX_PATH)out[pos++]='/';
        for(j=0;c[j];j++){if(pos>=MAX_PATH)return 0;out[pos++]=c[j];}
    }
    out[pos]=0;return 1;
}

/* Возвращает родителя каталога path; ROOT обозначается индексом 0. */
static int ensure_dir(struct host_dir dirs[],unsigned *dir_count,const char *path,uint32_t *next_cluster,uint16_t *out_first){
    unsigned count=path_count(path),i;char p[MAX_PATH+1u];
    int existing;
    if(count==0)return 0;
    existing=find_dir(dirs,*dir_count,path);
    if(existing>=0){*out_first=dirs[existing].first;return 1;}
    for(i=1;i<=count;i++){
        int idx;
        if(!prefix_path(path,i,p))return 0;
        idx=find_dir(dirs,*dir_count,p);
        if(idx<0){
            struct host_dir *d;
            if(*dir_count>=MAX_DIRS)return 0;
            d=&dirs[(*dir_count)++];
            strncpy(d->path,p,MAX_PATH);d->path[MAX_PATH]=0;
            d->first=*next_cluster;(*next_cluster)++;
            d->extra_first=0; d->clusters=1;
            if(i==1)d->parent=0;
            else{char parent[MAX_PATH+1u];int pi;if(!prefix_path(path,i-1u,parent))return 0;pi=find_dir(dirs,*dir_count-1u,parent);if(pi<0)return 0;d->parent=dirs[pi].first;}
            idx=(int)(*dir_count-1u);
        }
        if(i==count){*out_first=dirs[idx].first;return 1;}
    }
    return 0;
}

static int dir_path_parent(const char *file_path,char parent[MAX_PATH+1u],char leaf[13]){
    unsigned count=path_count(file_path),i,j;char c[13];
    if(count==0||!path_component(file_path,count-1u,leaf))return 0;
    parent[0]=0;
    for(i=0;i+1u<count;i++){
        if(!path_component(file_path,i,c))return 0;
        j=0;if(parent[0]){while(parent[j])j++;if(j>=MAX_PATH)return 0;parent[j++]='/';}
        {unsigned k=0;while(c[k]){if(j>=MAX_PATH)return 0;parent[j++]=c[k++];}parent[j]=0;}
    }
    return 1;
}

static unsigned char *dir_sector(unsigned char *img,uint16_t cluster){return img+(DATA_LBA+(uint32_t)(cluster-2u))*SECTOR;}

static int put_entry(unsigned char *img,const struct host_dir *dir,const unsigned char name[11],uint8_t attr,uint16_t first,uint32_t size){
    unsigned c,idx;
    for(c=0;c<dir->clusters;c++){
        uint16_t cl=(c==0u)?dir->first:(uint16_t)(dir->extra_first+c-1u);
        unsigned char *d=dir_sector(img,cl);
        for(idx=0;idx<16u;idx++){unsigned char *e=d+idx*32u;if(e[0]==0x00||e[0]==0xe5){memset(e,0,32);memcpy(e,name,11);e[11]=attr;le16(e+26,first);le32(e+28,size);return 1;}}
    }
    return 0;
}

/*
 * Добавляет запись в фиксированный FAT16 ROOT DIRECTORY.
 * Важно: один сектор содержит только 16 записей, но root у нашего образа
 * занимает 32 сектора и поэтому содержит 512 записей. Предыдущая реализация
 * ошибочно просматривала только первый сектор, из-за чего образ с большим
 * количеством системных файлов обрывался уже после 16-й записи.
 */
static unsigned char short_checksum(const unsigned char name[11]){
    unsigned i;unsigned char sum=0;
    for(i=0;i<11u;i++)sum=(unsigned char)(((sum&1u)?0x80u:0u)+(sum>>1)+name[i]);
    return sum;
}

/* Записывает один LFN-entry AUTOSTART.SH непосредственно перед short-entry.
 * AUTOSTART.SH имеет 12 UTF-16 символов, поэтому одного 13-символьного LFN
 * entry достаточно. */
static int put_root_autostart_entry(unsigned char *root,const unsigned char short_name[11],uint16_t first,uint32_t size){
    uint32_t idx;
    static const char long_name[]="AUTOSTART.SH";
    static const unsigned pos[13]={1u,3u,5u,7u,9u,14u,16u,18u,20u,22u,24u,28u,30u};
    unsigned char sum=short_checksum(short_name);
    for(idx=0;idx+1u<ROOT_ENTRIES;idx++){
        unsigned char *lfn=root+idx*32u;
        unsigned char *ent=lfn+32u;
        if((lfn[0]==0x00||lfn[0]==0xe5)&&(ent[0]==0x00||ent[0]==0xe5)){
            unsigned k;
            memset(lfn,0xff,32u);
            lfn[0]=0x41u;
            lfn[11]=0x0fu;
            lfn[12]=0x00u;
            lfn[13]=sum;
            le16(lfn+26,0u);
            for(k=0;k<12u;k++)le16(lfn+pos[k],(unsigned char)long_name[k]);
            le16(lfn+pos[12],0x0000u);
            memset(ent,0,32u);memcpy(ent,short_name,11);ent[11]=0x20u;le16(ent+26,first);le32(ent+28,size);
            return 1;
        }
    }
    return 0;
}

static int put_root_entry(unsigned char *img,unsigned char *root,const unsigned char name[11],uint8_t attr,uint16_t first,uint32_t size){
    uint32_t idx;
    for(idx=0;idx<ROOT_ENTRIES;idx++){
        unsigned char *e=root+idx*32u;
        if(e[0]==0x00||e[0]==0xe5){
            memset(e,0,32);
            memcpy(e,name,11);
            e[11]=attr;
            le16(e+26,first);
            le32(e+28,size);
            return 1;
        }
    }
    (void)img;
    return 0;
}

static void put_dot_entries(unsigned char *img,uint16_t cluster,uint16_t parent){
    unsigned char *d=dir_sector(img,cluster);
    memset(d,0,SECTOR);
    memset(d,' ',32);d[0]='.';d[11]=0x10;le16(d+26,cluster);
    memset(d+32,' ',32);d[32]='.';d[33]='.';d[43]=0x10;le16(d+58,parent);
}

int main(int argc,char **argv){
    FILE *f;unsigned char *img,*b,*root;unsigned i,j;uint32_t next_cluster=FIRST_ALLOC_CLUSTER;
    const char *image;unsigned count,argi,free_percent=25u;
    struct host_file files[MAX_FILES];struct host_dir dirs[MAX_DIRS];unsigned dir_count=0;uint32_t bytes;
    uint32_t fat1,fat2,root_lba,data_lba;
    if(argc<2){fprintf(stderr,"usage: mkfat16 image.bin [--part-lba N] [--free-percent N] [HOSTFILE[=DESTPATH] ...]\n");return 2;}
    image=argv[1];argi=2u;
    while(argi<(unsigned)argc && strncmp(argv[argi],"--",2)==0){
        if(strcmp(argv[argi],"--part-lba")==0 && argi+1u<(unsigned)argc){g_part_lba=(uint32_t)strtoul(argv[argi+1u],0,0);argi+=2u;continue;}
        if(strcmp(argv[argi],"--free-percent")==0 && argi+1u<(unsigned)argc){free_percent=(unsigned)strtoul(argv[argi+1u],0,0);argi+=2u;continue;}
        fprintf(stderr,"unknown/incomplete option: %s\n",argv[argi]);return 2;
    }
    if(g_part_lba<2u){fprintf(stderr,"part LBA must be >=2\n");return 1;}
    if(free_percent>75u){fprintf(stderr,"free-percent must be 0..75\n");return 1;}
    count=(unsigned)argc-argi;
    if(count>MAX_FILES){fprintf(stderr,"too many input files (max %u)\n",MAX_FILES);return 1;}
    memset(files,0,sizeof(files));memset(dirs,0,sizeof(dirs));
    for(i=0;i<count;i++){
        char leaf[13];char spec[MAX_PATH+1u];char *eqp;
        files[i].path=argv[argi+i];
        strncpy(spec,files[i].path,MAX_PATH);spec[MAX_PATH]=0;
        eqp=strchr(spec,'=');
        if(eqp){size_t dn;*eqp=0;dn=strlen(eqp+1);if(dn>MAX_PATH)goto fail;memcpy(files[i].dest,eqp+1,dn+1u);}else{const char *base=host_basename(spec);size_t dn=strlen(base);if(dn>MAX_PATH)goto fail;memcpy(files[i].dest,base,dn+1u);}
        files[i].data=read_host_file(spec,&files[i].size);
        if(!files[i].data)goto fail;
        if(files[i].size==0){fprintf(stderr,"empty file is not supported: %s\n",spec);goto fail;}
        { char parent_check[MAX_PATH+1u];
          int special=(strcmp(files[i].dest,"AUTOSTART.SH")==0);
          if(!dir_path_parent(files[i].dest,parent_check,leaf)||(!special&&!make83_component(leaf,(unsigned char[11]){0}))){fprintf(stderr,"invalid destination 8.3 path: %s\n",files[i].dest);goto fail;}
          if(special && parent_check[0]){fprintf(stderr,"AUTOSTART.SH must be stored in FAT16 root\n");goto fail;}
          if(special){unsigned z;for(z=0;z<i;z++)if(strcmp(files[z].dest,"AUTOST~1.SH")==0){fprintf(stderr,"AUTOSTART.SH alias collides with AUTOST~1.SH\n");goto fail;}}
        }
        files[i].clusters=(uint32_t)((files[i].size+SECTOR-1u)/SECTOR);
    }

    /* Reserve cluster numbers for directories and files first. */
    for(i=0;i<count;i++){
        char parent[MAX_PATH+1u];uint16_t first;
        if(!dir_path_parent(files[i].dest,parent,(char[13]){0}))goto fail;
        if(parent[0] && !ensure_dir(dirs,&dir_count,parent,&next_cluster,&first))goto fail;
    }
    /* FIX60U: каталоги FAT16 должны вмещать больше одного сектора.
       Подсчитываем прямые записи каждого каталога и резервируем дополнительные
       кластеры до размещения файлов. Первый кластер уже выделен ensure_dir(). */
    for(i=0;i<dir_count;i++){
        unsigned entries=2u,k,need; /* . и .. */
        for(k=0;k<dir_count;k++)if(dirs[k].parent==dirs[i].first)entries++;
        for(k=0;k<count;k++){char parent[MAX_PATH+1u],leaf[13];if(!dir_path_parent(files[k].dest,parent,leaf))goto fail;if(strcmp(parent,dirs[i].path)==0)entries++;}
        need=(entries+15u)/16u;if(need<1u)need=1u;
        dirs[i].clusters=(uint16_t)need;
        if(need>1u){if(next_cluster+(need-1u)>65525u){fprintf(stderr,"directory set exceeds FAT16 cluster limit\n");goto fail;}dirs[i].extra_first=(uint16_t)next_cluster;next_cluster+=(need-1u);}
    }
    for(i=0;i<count;i++){
        if(files[i].clusters==0u||next_cluster+files[i].clusters>65525u){fprintf(stderr,"file set exceeds FAT16 cluster limit\n");goto fail;}
        files[i].first=(uint16_t)next_cluster;next_cluster+=(uint16_t)files[i].clusters;
    }

    /* Dynamic FAT16 geometry. Cluster 2 is README.TXT; next_cluster is one
       beyond the final allocated cluster.  Keep the requested percentage of
       the data area free after packing, while respecting the FAT16 minimum. */
    {
        uint32_t used_clusters=next_cluster-2u;
        uint32_t target=used_clusters;
        if(free_percent<100u){uint32_t denom=100u-free_percent;target=(used_clusters*100u+denom-1u)/denom;}
        if(target<4085u)target=4085u;
        if(target>=65525u){fprintf(stderr,"FAT16 data area would exceed 65524 clusters\n");goto fail;}
        g_spf=((target+2u)*2u+SECTOR-1u)/SECTOR;
        if(!g_spf||g_spf>255u){fprintf(stderr,"FAT16 sectors/FAT out of supported range\n");goto fail;}
        g_volume_sectors=RESERVED+FATS*g_spf+ROOT_SECTORS+target;
        g_total_sectors=g_part_lba+g_volume_sectors;
        if(g_total_sectors<g_part_lba||g_total_sectors>0x7fffffffu){fprintf(stderr,"image too large\n");goto fail;}
    }
    if(next_cluster>MAX_CLUSTER+1u){fprintf(stderr,"allocated data does not fit computed FAT16 geometry\n");goto fail;}

    bytes=TOTAL_SECTORS*SECTOR;fat1=PART_LBA+RESERVED;fat2=fat1+SPF;root_lba=fat2+SPF;data_lba=DATA_LBA;
    img=(unsigned char*)calloc(1,bytes);if(!img){fprintf(stderr,"out of memory\n");goto fail;}
    b=img+PART_LBA*SECTOR;b[0]=0xeb;b[1]=0x3c;b[2]=0x90;memcpy(b+3,"TOYOS   ",8);
    le16(b+11,SECTOR);b[13]=1;le16(b+14,RESERVED);b[16]=FATS;le16(b+17,ROOT_ENTRIES);le16(b+19,0);b[21]=0xf8;le16(b+22,(uint16_t)SPF);le16(b+24,32);le16(b+26,64);le32(b+28,PART_LBA);le32(b+32,g_volume_sectors);
    b[36]=0x80;b[38]=0x29;le32(b+39,0x12345678u);memcpy(b+43,"TOY OS     ",11);memcpy(b+54,"FAT16   ",8);b[510]=0x55;b[511]=0xaa;
    b=img+fat1*SECTOR;le16(b+0,0xfff8);le16(b+2,0xffff);le16(b+4,0xffff);
    for(i=0;i<dir_count;i++){
        unsigned c;
        if(dirs[i].clusters==1u)le16(b+dirs[i].first*2u,0xffff);
        else{
            le16(b+dirs[i].first*2u,dirs[i].extra_first);
            for(c=0;c+1u<dirs[i].clusters-1u;c++)le16(b+(dirs[i].extra_first+c)*2u,(uint16_t)(dirs[i].extra_first+c+1u));
            le16(b+(dirs[i].extra_first+dirs[i].clusters-2u)*2u,0xffff);
        }
    }
    for(i=0;i<count;i++)for(j=0;j<files[i].clusters;j++){uint16_t cl=(uint16_t)(files[i].first+j),nx=(j+1u<files[i].clusters)?(uint16_t)(cl+1u):0xffffu;le16(b+cl*2u,nx);}
    memcpy(img+fat2*SECTOR,b,SPF*SECTOR);

    root=img+root_lba*SECTOR;memset(root,0,ROOT_SECTORS*SECTOR);
    memcpy(root,"README  TXT",11);root[11]=0x20;le16(root+26,2);{const char sample[]="Hello from the FAT16 filesystem!\r\n";le32(root+28,(uint32_t)sizeof(sample)-1u);memcpy(img+data_lba*SECTOR,sample,sizeof(sample)-1u);}
    {
        uint32_t root_required=1u;
        for(i=0;i<dir_count;i++){char parent[MAX_PATH+1u];char leaf[13];if(!dir_path_parent(dirs[i].path,parent,leaf))goto fail_img;if(!parent[0])root_required++;}
        for(i=0;i<count;i++){char parent[MAX_PATH+1u];char leaf[13];if(!dir_path_parent(files[i].dest,parent,leaf))goto fail_img;if(!parent[0])root_required+=(strcmp(files[i].dest,"AUTOSTART.SH")==0)?2u:1u;}
        if(root_required>ROOT_ENTRIES){fprintf(stderr,"ROOT DIRECTORY is full: need %lu entries, have %u\n",(unsigned long)root_required,ROOT_ENTRIES);goto fail_img;}
    }
    for(i=0;i<dir_count;i++)put_dot_entries(img,dirs[i].first,dirs[i].parent);
    for(i=0;i<dir_count;i++){
        char leaf[13],parent[MAX_PATH+1u];unsigned char name[11];int pi;
        if(!dir_path_parent(dirs[i].path,parent,leaf)||!make83_component(leaf,name))goto fail_img;
        if(parent[0]){pi=find_dir(dirs,dir_count,parent);if(pi<0||!put_entry(img,&dirs[pi],name,0x10u,dirs[i].first,0))goto fail_img;}
        else{if(!put_root_entry(img,root,name,0x10u,dirs[i].first,0))goto fail_img;}
    }
    for(i=0;i<count;i++){
        char parent[MAX_PATH+1u],leaf[13];unsigned char name[11];uint32_t off;int pi;
        if(!dir_path_parent(files[i].dest,parent,leaf)||!make83_component(leaf,name))goto fail_img;
        if(parent[0]){pi=find_dir(dirs,dir_count,parent);if(pi<0||!put_entry(img,&dirs[pi],name,0x20u,files[i].first,(uint32_t)files[i].size))goto fail_img;}
        else{
            if(strcmp(files[i].dest,"AUTOSTART.SH")==0){if(!put_root_autostart_entry(root,name,files[i].first,(uint32_t)files[i].size))goto fail_img;}
            else if(!put_root_entry(img,root,name,0x20u,files[i].first,(uint32_t)files[i].size))goto fail_img;
        }
        for(off=0;off<(uint32_t)files[i].size;off++)img[(data_lba+(files[i].first-2u)+off/SECTOR)*SECTOR+off%SECTOR]=files[i].data[off];
    }
    f=fopen(image,"wb");if(!f){perror(image);goto fail_img;}
    if(fwrite(img,1,bytes,f)!=bytes){fprintf(stderr,"write failed\n");fclose(f);goto fail_img;}
    fclose(f);printf("FAT16 image created: %s, %u files, %u dirs, FAT LBA=%lu, volume=%lu sectors, image=%lu sectors, SPF=%lu, data clusters=%lu\n",image,count,dir_count,(unsigned long)PART_LBA,(unsigned long)g_volume_sectors,(unsigned long)TOTAL_SECTORS,(unsigned long)SPF,(unsigned long)DATA_CLUSTERS);
    free(img);for(i=0;i<count;i++)free(files[i].data);return 0;
fail_img:
    free(img);
fail:
    {for(i=0;i<count;i++)free(files[i].data);}
    return 1;
}

