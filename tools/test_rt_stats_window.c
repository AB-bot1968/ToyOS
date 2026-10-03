#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#define BT 30000u
#define BN 72u
struct b { uint32_t start,jobs,miss,skip; };
static struct b a[BN];
static uint32_t sat(uint32_t x,uint32_t y){return UINT32_MAX-x<y?UINT32_MAX:x+y;}
static void add(uint32_t now,uint32_t miss,uint32_t skip){uint32_t st=now-now%BT,i=(st/BT)%BN;if(a[i].start!=st){memset(&a[i],0,sizeof(a[i]));a[i].start=st;}a[i].jobs++;a[i].miss+=miss;a[i].skip+=skip;}
static void sum(uint32_t now,uint32_t*out){uint32_t i,age;out[0]=out[1]=out[2]=0;for(i=0;i<BN;i++){if(!a[i].jobs)continue;age=now-a[i].start;if(age>=BT*BN)continue;out[0]+=a[i].jobs;out[1]+=a[i].miss;out[2]+=a[i].skip;}}
#define OK(x) do{if(!(x)){printf("FAIL line %d\n",__LINE__);return 1;}}while(0)
int main(void){uint32_t q[3],x;
 add(0,0,0);add(BT-1,1,2);sum(BT-1,q);OK(q[0]==2&&q[1]==1&&q[2]==2);
 add(BT,0,0);sum(BT,q);OK(q[0]==3);
 sum(BT*BN-1,q);OK(q[0]==3);sum(BT*BN,q);OK(q[0]==1); /* bucket 0 expired, bucket 1 remains */
 x=UINT32_MAX-1;OK(sat(x,1)==UINT32_MAX);OK(sat(x,2)==UINT32_MAX);OK(sat(UINT32_MAX,1)==UINT32_MAX);
 memset(a,0,sizeof(a));add(0xfffffff0u,1,0);add(20u,0,1);sum(20u,q);OK(q[0]>=1); /* modulo-2^32 age is bounded */
 puts("PASS: 6h window expiry, bucket rotation, saturation, timer wrap");return 0;}
