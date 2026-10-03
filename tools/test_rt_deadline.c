#include <limits.h>
#include "../src/rt_deadline.h"

static int expect_u(unsigned int got,unsigned int want,int code){return got==want?0:code;}
static int expect_i(int got,int want,int code){return got==want?0:code;}

int main(void){
    int r;

    /* Stage 6.1 RT clock: 100 Hz, ceil(ms / 10), with a one-tick minimum. */
    r=expect_u(rt_deadline_ms_to_ticks(1u),1u,1); if(r)return r;
    r=expect_u(rt_deadline_ms_to_ticks(10u),1u,2); if(r)return r;
    r=expect_u(rt_deadline_ms_to_ticks(20u),2u,3); if(r)return r;
    r=expect_u(rt_deadline_ms_to_ticks(100u),10u,4); if(r)return r;
    r=expect_u(rt_deadline_ms_to_ticks(UINT_MAX),429496730u,5); if(r)return r;

    /* Absolute deadline = release + deadline interval, independent of
       dispatch time. */
    r=expect_u(rt_deadline_absolute(100u,5u),105u,6); if(r)return r;
    r=expect_i(rt_deadline_reached(104u,105u),0,7); if(r)return r;
    r=expect_i(rt_deadline_reached(105u,105u),1,8); if(r)return r;
    r=expect_i(rt_deadline_reached(106u,105u),1,9); if(r)return r;

    /* Absolute arithmetic deliberately wraps as uint32_t; the comparison
       remains correct around the timer wrap. */
    r=expect_u(rt_deadline_absolute(0xfffffff0u,32u),16u,10); if(r)return r;
    r=expect_i(rt_deadline_reached(0u,0xfffffff0u),1,11); if(r)return r;
    r=expect_i(rt_deadline_reached(0xfffffff0u,16u),0,12); if(r)return r;
    return 0;
}
