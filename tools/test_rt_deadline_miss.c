#include <stdio.h>
#include <stdint.h>

struct c { int ready; int active; int missed; unsigned priority; unsigned deadline; unsigned slot; };

static int choose(const struct c *a, unsigned n, unsigned missed_class){
    int best=-1; unsigned i;
    for(i=0;i<n;i++){
        int pc;
        if(!a[i].ready||!a[i].active)continue;
        if((a[i].missed?1u:0u)!=missed_class)continue;
        if(best<0){best=(int)i;continue;}
        pc=a[i].priority>a[best].priority?1:(a[i].priority<a[best].priority?-1:0);
        if(pc>0 || (pc==0 && (a[i].deadline<a[best].deadline || (a[i].deadline==a[best].deadline&&a[i].slot<a[best].slot))))best=(int)i;
    }
    return best;
}

int main(void){
    struct c a[4]={{1,1,1,250,100,0},{1,1,0,3,110,1},{1,1,0,7,90,2},{0,0,0,255,80,3}};
    if(choose(a,4,0u)!=2)return 1; /* any on-time job wins over missed high priority */
    if(choose(a,4,1u)!=0)return 2; /* among missed jobs, priority/deadline/slot still apply */
    a[0].priority=1; if(choose(a,4,1u)!=0)return 3; /* only one missed job */
    a[0].ready=0; if(choose(a,4,1u)!=-1)return 4;
    a[0].ready=1;a[0].priority=9;a[0].deadline=70;a[2].missed=1;
    if(choose(a,4,1u)!=0)return 5; /* higher priority missed job wins best-effort class */
    a[0].priority=7;a[2].priority=7;a[0].deadline=120;a[2].deadline=90;
    if(choose(a,4,1u)!=2)return 6; /* equal priority: earlier absolute deadline */
    puts("RTD Stage 5.2 deadline-miss arbitration model passed");
    return 0;
}
