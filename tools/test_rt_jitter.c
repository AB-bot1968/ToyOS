/* Host-side arithmetic regression for Stage 6.2 measurement rules.
 * It mirrors the kernel's modulo-32-bit interval/lateness calculations with
 * deterministic synthetic IRQ observations.  The executable JITTER.EXE is
 * still the authoritative end-to-end test inside ToyOS; this host test guards
 * the measurement math against accidental changes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static uint32_t diff(uint32_t a,uint32_t b){return (uint32_t)(a-b);}
static uint32_t late(uint32_t observed,uint32_t release){
    uint32_t d=diff(observed,release);
    return d<0x80000000u?d:0u;
}
int main(void){
    const uint32_t release[]={100u,101u,102u,103u,104u};
    const uint32_t observed[]={100u,101u,103u,104u,105u};
    uint32_t i,min=0xffffffffu,max=0u,sum=0u,maxlate=0u;
    for(i=1u;i<5u;i++){
        uint32_t interval=diff(observed[i],observed[i-1u]);
        uint32_t l=late(observed[i],release[i]);
        if(interval<min)min=interval;
        if(interval>max)max=interval;
        sum+=interval;
        if(l>maxlate)maxlate=l;
    }
    if(min!=1u||max!=2u||sum!=5u||maxlate!=1u){
        fprintf(stderr,"RT jitter arithmetic FAIL: min=%u max=%u sum=%u maxlate=%u\n",min,max,sum,maxlate);
        return 1;
    }
    /* Explicit wrap-around case: 0xffffffff -> 0 is one tick apart. */
    if(diff(0u,0xffffffffu)!=1u||late(0u,0xffffffffu)!=1u){
        fprintf(stderr,"RT jitter wrap arithmetic FAIL\n");
        return 1;
    }
    puts("RT jitter arithmetic PASS");
    return 0;
}
