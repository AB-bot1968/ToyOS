#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define SECTOR 512u
#define MAGIC 0x3159414cu
static uint32_t le32(const unsigned char*p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static uint16_t le16(const unsigned char*p){return (uint16_t)p[0]|((uint16_t)p[1]<<8);}
int main(int argc,char**argv){
    FILE*f;unsigned char d[SECTOR],b[SECTOR];long size;uint32_t w[16],sum=0,i;
    uint32_t klba,kbytes,ksecs,reserve,gap,fat_lba,fatsecs,imgsecs,align,freepct,load,limit;
    uint32_t x,reserve_end,min_fat,bpbtot,rootsec,usedmeta,datasec,clusters,freec=0,fat1;
    if(argc!=2){fprintf(stderr,"usage: layoutcheck disk.img\n");return 2;}
    f=fopen(argv[1],"rb");if(!f){perror(argv[1]);return 1;}
    if(fseek(f,SECTOR,SEEK_SET)||fread(d,1,SECTOR,f)!=SECTOR){fprintf(stderr,"cannot read layout sector\n");fclose(f);return 1;}
    for(i=0;i<16u;i++){w[i]=le32(d+i*4u);sum+=w[i];}
    if(w[0]!=MAGIC||w[1]!=1u||w[2]!=64u||sum){fprintf(stderr,"invalid layout descriptor\n");fclose(f);return 1;}
    klba=w[4];kbytes=w[5];ksecs=w[6];reserve=w[7];gap=w[8];fat_lba=w[9];fatsecs=w[10];imgsecs=w[11];align=w[12];freepct=w[13];load=w[14];limit=w[15];
    if(klba<2u||!kbytes||!ksecs||ksecs!=1u+(kbytes-1u)/SECTOR||ksecs>0xffffu||!reserve||!gap||!fatsecs||!align||freepct>75u||fat_lba%align){fprintf(stderr,"layout scalar invariant failed\n");fclose(f);return 1;}
    x=klba+ksecs;if(x<klba){fprintf(stderr,"kernel disk range overflow\n");fclose(f);return 1;}
    reserve_end=x+reserve;if(reserve_end<x){fprintf(stderr,"kernel reserve overflow\n");fclose(f);return 1;}
    min_fat=reserve_end+gap;if(min_fat<reserve_end||fat_lba<min_fat){fprintf(stderr,"layout overlap/gap invariant failed\n");fclose(f);return 1;}
    x=fat_lba+fatsecs;if(x<fat_lba||x!=imgsecs){fprintf(stderr,"image range invariant failed\n");fclose(f);return 1;}
    if(load!=0x7e00u||limit<=load||limit>0x000a0000u||ksecs>(0xffffffffu-load)/SECTOR||load+ksecs*SECTOR>limit){fprintf(stderr,"kernel memory invariant failed\n");fclose(f);return 1;}
    if(fseek(f,(long)fat_lba*SECTOR,SEEK_SET)||fread(b,1,SECTOR,f)!=SECTOR){fprintf(stderr,"cannot read FAT16 BPB\n");fclose(f);return 1;}
    if(b[510]!=0x55||b[511]!=0xaa||le16(b+11)!=SECTOR||!b[13]){fprintf(stderr,"invalid FAT16 BPB\n");fclose(f);return 1;}
    bpbtot=le16(b+19);if(!bpbtot)bpbtot=le32(b+32);
    if(bpbtot!=fatsecs||le32(b+28)!=fat_lba){fprintf(stderr,"descriptor/BPB mismatch\n");fclose(f);return 1;}
    rootsec=((uint32_t)le16(b+17)+15u)/16u;usedmeta=(uint32_t)le16(b+14)+(uint32_t)b[16]*le16(b+22)+rootsec;
    if(bpbtot<=usedmeta){fprintf(stderr,"invalid FAT16 geometry\n");fclose(f);return 1;}
    datasec=bpbtot-usedmeta;clusters=datasec/b[13];if(clusters<4085u||clusters>=65525u){fprintf(stderr,"not FAT16 cluster range\n");fclose(f);return 1;}
    fat1=fat_lba+le16(b+14);if(fat1<fat_lba){fprintf(stderr,"FAT address overflow\n");fclose(f);return 1;}
    for(i=2u;i<clusters+2u;i++){unsigned char e[2];long off=(long)fat1*SECTOR+(long)i*2L;if(fseek(f,off,SEEK_SET)||fread(e,1,2,f)!=2u){fprintf(stderr,"cannot scan FAT\n");fclose(f);return 1;}if(le16(e)==0u)freec++;}
    if(freec*100u<clusters*freepct){fprintf(stderr,"FAT16 free-space policy not met: free=%lu/%lu target=%lu%%\n",(unsigned long)freec,(unsigned long)clusters,(unsigned long)freepct);fclose(f);return 1;}
    if(fseek(f,0,SEEK_END)){fclose(f);return 1;}size=ftell(f);fclose(f);
    if(size<0||(uint32_t)(size/SECTOR)!=imgsecs||size%(long)SECTOR){fprintf(stderr,"descriptor/file-size mismatch\n");return 1;}
    printf("FIX53 layout: kernel LBA=%lu bytes=%lu sectors=%lu reserve=%lu gap=%lu FAT LBA=%lu FAT sectors=%lu image=%lu sectors align=%lu free=%lu%% load=[0x%lx,0x%lx) PASS\n",(unsigned long)klba,(unsigned long)kbytes,(unsigned long)ksecs,(unsigned long)reserve,(unsigned long)gap,(unsigned long)fat_lba,(unsigned long)fatsecs,(unsigned long)imgsecs,(unsigned long)align,(unsigned long)freepct,(unsigned long)load,(unsigned long)limit);
    return 0;
}
