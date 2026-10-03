/*
 * fat16check.c - строгий проверяющий FAT16-образ Toy OS v60.
 *
 * Поддерживает проверку пути 8.3, включая каталоги:
 *     fat16check disk.img BIN/HELLO.EXE
 *     fat16check disk.img DOC
 *
 * Для обычного файла дополнительно проверяется длина FAT-цепочки и зеркальность
 * обеих копий FAT. Для каталога проверяется наличие корректной FAT-цепочки.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define SECTOR 512u
#define EOC 0xfff8u
#define BAD 0xfff7u
#define MAX_PATH 127u

static uint16_t le16(const unsigned char *p){return (uint16_t)p[0]|((uint16_t)p[1]<<8);}
static uint32_t le32(const unsigned char *p){return (uint32_t)le16(p)|((uint32_t)le16(p+2)<<16);}

static int make83(const char *in,unsigned char out[11]){
    unsigned i=0,j=0;
    if(!in||!in[0])return 0;
    if(strcmp(in,"AUTOSTART.SH")==0){memcpy(out,"AUTOST~1SH ",11);return 1;}
    memset(out,' ',11);
    while(in[i]&&in[i]!='.'){
        unsigned char c=(unsigned char)in[i++];
        if(c=='/'||c=='\\'||c==' '||c=='\t'||j>=8u)return 0;
        if(c>='a'&&c<='z')c=(unsigned char)(c-'a'+'A');
        out[j++]=c;
    }
    if(j==0)return 0;
    if(in[i]=='.'){
        i++;j=8;if(!in[i])return 0;
        while(in[i]){unsigned char c=(unsigned char)in[i++];if(c=='/'||c=='\\'||c==' '||c=='\t'||c=='.'||j>=11u)return 0;if(c>='a'&&c<='z')c=(unsigned char)(c-'a'+'A');out[j++]=c;}
    }
    return 1;
}

static int read_sector(FILE *f,uint32_t lba,unsigned char *b){if(fseek(f,(long)(lba*SECTOR),SEEK_SET)!=0)return 0;return fread(b,1,SECTOR,f)==SECTOR;}
static int fat_get(FILE *f,uint32_t fat_base,uint16_t cluster,uint16_t *out,unsigned char *b){uint32_t o=(uint32_t)cluster*2u;if(!read_sector(f,fat_base+o/SECTOR,b))return 0;*out=le16(b+o%SECTOR);return 1;}

static int find_in_dir(FILE *f,uint16_t dir_cluster,uint32_t root_lba,uint32_t root_sectors,const unsigned char want[11],uint32_t data_lba,uint8_t spc,uint32_t fat1,uint16_t max_cluster,uint32_t *entry_lba,uint16_t *entry_off,unsigned char *b){
    uint32_t idx,s,e;uint16_t cl=dir_cluster,next;unsigned char *d;
    if(dir_cluster==0){s=root_lba;e=root_lba+root_sectors;for(;s<e;s++){if(!read_sector(f,s,b))return 0;for(idx=0;idx<16u;idx++){d=b+idx*32u;if(d[0]==0x00)return 0;if(d[0]==0xe5||d[11]==0x0f||(d[11]&0x08u))continue;if(memcmp(d,want,11)==0){*entry_lba=s;*entry_off=(uint16_t)(idx*32u);return 1;}}}return 0;}
    for(;;){uint32_t base=data_lba+(uint32_t)(cl-2u)*spc;for(idx=0;idx<(uint32_t)spc*16u;idx++){uint32_t si=idx/16u,ei=idx%16u;if(!read_sector(f,base+si,b))return 0;d=b+ei*32u;if(d[0]==0x00)return 0;if(d[0]==0xe5||d[11]==0x0f||(d[11]&0x08u))continue;if(memcmp(d,want,11)==0){*entry_lba=base+si;*entry_off=(uint16_t)(ei*32u);return 1;}}if(!fat_get(f,fat1,cl,&next,b))return 0;if(next>=EOC)return 0;if(next<2u||next>max_cluster||next==BAD)return 0;cl=next;}
}

static int find_path(FILE *f,const char *path,uint32_t root_lba,uint32_t root_sectors,uint32_t data_lba,uint8_t spc,uint32_t fat1,uint16_t max_cluster,uint32_t *entry_lba,uint16_t *entry_off,unsigned char *b){
    unsigned pos=0;uint16_t dir=0;char part[13];unsigned char want[11];
    while(path[pos]=='/'||path[pos]=='\\')pos++;
    if(!path[pos])return 0;
    for(;;){unsigned n=0;while(path[pos]&&path[pos]!='/'&&path[pos]!='\\'){if(n>=12u)return 0;part[n++]=path[pos++];}part[n]=0;if(!make83(part,want))return 0;if(!find_in_dir(f,dir,root_lba,root_sectors,want,data_lba,spc,fat1,max_cluster,entry_lba,entry_off,b))return 0;if(!path[pos])return 1;if(!read_sector(f,*entry_lba,b))return 0;if(!(b[*entry_off+11]&0x10u))return 0;dir=le16(b+*entry_off+26);if(dir<2u||dir>max_cluster)return 0;while(path[pos]=='/'||path[pos]=='\\')pos++;if(!path[pos])return 0;}
}

static int check_chain(FILE *f,uint16_t first,uint32_t required,uint32_t fat1,uint32_t fat2,uint16_t max_cluster,unsigned char *b,uint32_t *chain_out){
    uint16_t cl=first,next,a,z;uint32_t chain=0;
    if(first<2u||first>max_cluster)return 0;
    for(;;){uint32_t o=(uint32_t)cl*2u;if(!read_sector(f,fat1+o/SECTOR,b))return 0;a=le16(b+o%SECTOR);if(!read_sector(f,fat2+o/SECTOR,b))return 0;z=le16(b+o%SECTOR);if(a!=z)return 0;chain++;next=a;if(next>=EOC)break;if(next<2u||next>max_cluster||next==BAD)return 0;cl=next;if(chain>65534u)return 0;}
    if(chain<required)return 0;
    *chain_out=chain;return 1;
}

int main(int argc,char **argv){
    FILE *f;unsigned char b[SECTOR],layout[SECTOR];uint16_t bps,reserved,fats,root_entries,spf,max_cluster;uint8_t spc;uint32_t total,root_sectors,used,datasec,clusters,fat1,fat2,root_lba,data_lba,fat_lba,fat_sectors,image_sectors,sum=0u,i;uint32_t lba;uint16_t off,first;uint32_t size,chain,required;const char *name;long expected=-1;
    if(argc<2){fprintf(stderr,"usage: fat16check image.bin [path] [expected-size]\n");return 2;}name=argc>=3?argv[2]:"TEST.TXT";if(argc>=4)expected=atol(argv[3]);
    f=fopen(argv[1],"rb");if(!f){perror(argv[1]);return 1;}
    if(!read_sector(f,1u,layout)){fprintf(stderr,"cannot read FIX53 layout descriptor\n");fclose(f);return 1;}
    for(i=0u;i<16u;i++)sum+=le32(layout+i*4u);
    if(le32(layout)!=0x3159414cu||le32(layout+4)!=1u||le32(layout+8)!=64u||sum){fprintf(stderr,"invalid FIX53 layout descriptor\n");fclose(f);return 1;}
    fat_lba=le32(layout+36);fat_sectors=le32(layout+40);image_sectors=le32(layout+44);
    if(!read_sector(f,fat_lba,b)||b[510]!=0x55||b[511]!=0xaa){fprintf(stderr,"invalid FAT16 boot sector at dynamic LBA %lu\n",(unsigned long)fat_lba);fclose(f);return 1;}
    bps=le16(b+11);spc=b[13];reserved=le16(b+14);fats=b[16];root_entries=le16(b+17);spf=le16(b+22);total=le16(b+19);if(!total)total=le32(b+32);
    if(bps!=512u||!spc||(spc&(spc-1u))||!reserved||!fats||!spf||!root_entries||!total){fprintf(stderr,"invalid FAT16 BPB\n");fclose(f);return 1;}
    if(le32(b+28)!=fat_lba||total!=fat_sectors||fat_lba+fat_sectors!=image_sectors){fprintf(stderr,"FIX53 descriptor/BPB size mismatch\n");fclose(f);return 1;}
    root_sectors=((uint32_t)root_entries+15u)/16u;used=(uint32_t)reserved+(uint32_t)fats*spf+root_sectors;if(total<=used){fclose(f);return 1;}datasec=total-used;clusters=datasec/spc;if(clusters<4085u||clusters>=65525u){fprintf(stderr,"volume is not FAT16\n");fclose(f);return 1;}max_cluster=(uint16_t)(clusters+1u);fat1=fat_lba+reserved;fat2=fat1+spf;root_lba=fat2+spf;data_lba=root_lba+root_sectors;
    if(!find_path(f,name,root_lba,root_sectors,data_lba,spc,fat1,max_cluster,&lba,&off,b)){fprintf(stderr,"path %s not found\n",name);fclose(f);return 1;}
    if(!read_sector(f,lba,b)){fclose(f);return 1;}first=le16(b+off+26);size=le32(b+off+28);if(expected>=0&&size!=(uint32_t)expected){fprintf(stderr,"size mismatch: got %lu expected %ld\n",(unsigned long)size,expected);fclose(f);return 1;}
    if(b[off+11]&0x10u){if(first<2u||first>max_cluster){fprintf(stderr,"directory %s has invalid start cluster\n",name);fclose(f);return 1;}required=1u;if(!check_chain(f,first,required,fat1,fat2,max_cluster,b,&chain)){fprintf(stderr,"directory FAT chain check failed: %s\n",name);fclose(f);return 1;}printf("FAT16 CHECK OK: %s DIR first_cluster=%u clusters=%lu\n",name,first,(unsigned long)chain);fclose(f);return 0;}
    if(size==0){if(first!=0){fprintf(stderr,"empty file has non-zero start cluster\n");fclose(f);return 1;}printf("FAT16 CHECK OK: %s size=0 clusters=0\n",name);fclose(f);return 0;}
    required=(size+(uint32_t)spc*SECTOR-1u)/((uint32_t)spc*SECTOR);if(!check_chain(f,first,required,fat1,fat2,max_cluster,b,&chain)){fprintf(stderr,"FAT chain check failed: %s\n",name);fclose(f);return 1;}
    printf("FAT16 CHECK OK: %s size=%lu first_cluster=%u clusters=%lu\n",name,(unsigned long)size,first,(unsigned long)chain);fclose(f);return 0;
}
