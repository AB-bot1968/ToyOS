#include <stdio.h>
#include <stdint.h>

struct task {
    int ready;
    int active;
    int missed;
    unsigned priority;
    uint32_t deadline;
    unsigned slot;
};

static int before(uint32_t a,uint32_t b){
    uint32_t d=(uint32_t)(b-a);
    return d!=0u&&d<0x80000000u;
}

static int choose_class(const struct task *a,unsigned n,unsigned missed_class){
    int best=-1;
    unsigned i;
    for(i=0;i<n;i++){
        if(!a[i].ready||!a[i].active)continue;
        if((a[i].missed?1u:0u)!=missed_class)continue;
        if(best<0){best=(int)i;continue;}
        if(a[i].priority>a[best].priority ||
           (a[i].priority==a[best].priority &&
            (before(a[i].deadline,a[best].deadline) ||
             (a[i].deadline==a[best].deadline&&a[i].slot<a[best].slot))))
            best=(int)i;
    }
    return best;
}

static int choose_next(const struct task *a,unsigned n){
    int ontime=choose_class(a,n,0u);
    if(ontime>=0)return ontime;
    return choose_class(a,n,1u);
}

int main(void){
    struct task a[2]={
        {1,1,0,7,105,0}, /* RTDMISS: on-time initially, higher priority */
        {1,1,0,3,120,1}  /* SENSOR1: on-time, lower priority */
    };
    int next;

    /* Exact first-dispatch situation: RTDMISS gets CPU first because it is
       still on-time and has the higher priority. */
    next=choose_next(a,2);
    if(next!=0){fprintf(stderr,"case1 expected initial RTDMISS slot 0, got %d\n",next);return 1;}

    /* Exact Stage 5.2 transition after RTDMISS misses its deadline: the
       on-time SENSOR1 must outrank the missed RTDMISS despite priority 3<7. */
    a[0].missed=1;
    next=choose_next(a,2);
    if(next!=1){fprintf(stderr,"case2 expected on-time SENSOR1 slot 1 after miss, got %d\n",next);return 2;}

    /* When SENSOR1 blocks in SYS_RT_WAIT, the missed RTDMISS remains runnable
       as best-effort and is allowed back onto the CPU. */
    a[1].ready=0;
    next=choose_next(a,2);
    if(next!=0){fprintf(stderr,"case3 expected missed RTDMISS slot 0 as best-effort fallback, got %d\n",next);return 3;}

    /* Deadline ordering itself must remain correct across timer wrap. */
    if(!before(0xfffffff8u,5u)){fprintf(stderr,"wrap-safe deadline ordering failed\n");return 4;}

    puts("RTD Stage 5.2 FIX1 exact runtime scenario model passed");
    return 0;
}
