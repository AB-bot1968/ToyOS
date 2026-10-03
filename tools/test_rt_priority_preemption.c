#include <stdint.h>
#include <stdio.h>

/* Host-side deterministic model of the exact Stage 4.2 arbiter used by the
 * kernel. The test focuses on the preemption decision: when current low-priority
 * job and newly READY high-priority job coexist, the high-priority job wins. */
static int pick(uint32_t cp,uint32_t cd,uint32_t ci,
                uint32_t bp,uint32_t bd,uint32_t bi){
    if(cp>bp)return 1;
    if(cp<bp)return 0;
    if(cd<bd)return 1;
    if(cd>bd)return 0;
    return ci<bi;
}
static int need(int cond){return cond?0:1;}
int main(void){
    if(need(pick(7,50,1,3,50,0)==1))return 1;
    if(need(pick(3,50,0,7,50,1)==0))return 2;
    if(need(pick(7,100,1,7,120,0)==1))return 3;
    if(need(pick(7,120,1,7,100,0)==0))return 4;
    puts("Stage 4.4 priority-preemption model passed");
    return 0;
}
