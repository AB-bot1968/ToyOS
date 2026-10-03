#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define SECTOR 512u
#define MAGIC 0x3159414cu
static void st32(unsigned char*p,uint32_t v){p[0]=(unsigned char)v;p[1]=(unsigned char)(v>>8);p[2]=(unsigned char)(v>>16);p[3]=(unsigned char)(v>>24);}
static uint32_t get32(const unsigned char*p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
int main(int argc,char**argv){FILE*f;unsigned char s[SECTOR];uint32_t v[16],sum=0;unsigned i;if(argc!=15){fprintf(stderr,"usage: mklayout TEMPLATE OUT kernel_lba kernel_bytes kernel_sectors reserve gap fat_lba fat_sectors image_sectors align free_pct load_addr load_limit\n");return 2;}f=fopen(argv[1],"rb");if(!f){perror(argv[1]);return 1;}if(fread(s,1,SECTOR,f)!=SECTOR){fprintf(stderr,"layout template must be 512 bytes\n");fclose(f);return 1;}fclose(f);memset(v,0,sizeof(v));v[0]=MAGIC;v[1]=1u;v[2]=64u;v[3]=0u;for(i=0;i<12u;i++)v[4u+i]=(uint32_t)strtoul(argv[3+i],0,0);for(i=0;i<16u;i++)sum+=v[i];v[3]=0u-sum;for(i=0;i<16u;i++)st32(s+i*4u,v[i]);sum=0;for(i=0;i<16u;i++)sum+=get32(s+i*4u);if(sum){fprintf(stderr,"internal checksum failure\n");return 1;}f=fopen(argv[2],"wb");if(!f){perror(argv[2]);return 1;}if(fwrite(s,1,SECTOR,f)!=SECTOR){fprintf(stderr,"write failed\n");fclose(f);return 1;}fclose(f);return 0;}
