#include <limits.h>
#include <stdio.h>
#include "../src/rt_deadline.h"

static int expect_u(unsigned int got,unsigned int want,int code){return got==want?0:code;}

int main(void){
    unsigned int rt=0u,sys=0u,phase=0u,i;
    int r;
    /* Stage 6.1: 100 Hz RT clock -> 10 ms per RT tick. */
    r=expect_u(rt_deadline_ms_to_ticks(1u),1u,1); if(r)return r;
    r=expect_u(rt_deadline_ms_to_ticks(10u),1u,2); if(r)return r;
    r=expect_u(rt_deadline_ms_to_ticks(11u),2u,3); if(r)return r;
    r=expect_u(rt_deadline_ms_to_ticks(20u),2u,4); if(r)return r;
    r=expect_u(rt_deadline_ms_to_ticks(40u),4u,5); if(r)return r;
    r=expect_u(rt_deadline_ms_to_ticks(100u),10u,6); if(r)return r;
    r=expect_u(rt_deadline_ms_to_ticks(UINT_MAX),429496730u,7); if(r)return r;

    /* Ten hardware RT ticks are five logical system ticks: 100 Hz hardware,
       50 Hz legacy SYS_TIMER_GET. */
    for(i=0u;i<10u;i++){
        rt++;
        phase^=1u;
        if(phase==0u)sys++;
    }
    r=expect_u(rt,10u,8); if(r)return r;
    r=expect_u(sys,5u,9); if(r)return r;
    puts("RTD Stage 6.1 dual-rate timebase model passed");
    return 0;
}
