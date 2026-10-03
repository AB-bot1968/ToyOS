#include <stdint.h>
#include "../src/rt_priority.h"

/* Host-side model of the exact Stage 4.2 ready-order rule:
 *   1. higher numeric priority wins;
 *   2. equal priority -> earlier deadline wins;
 *   3. equal priority/deadline -> lower slot id wins.
 */
static int ready_precedes(uint32_t cp,uint32_t cd,uint32_t ci,
                          uint32_t bp,uint32_t bd,uint32_t bi){
    int pc=rt_priority_compare(cp,bp);
    if(pc>0)return 1;
    if(pc<0)return 0;
    if(cd<bd)return 1;
    if(cd>bd)return 0;
    return ci<bi;
}

static int expect(int actual,int wanted){return actual==wanted?0:1;}

int main(void){
    if(expect(ready_precedes(7,100,1,3,20,0),1))return 1;
    if(expect(ready_precedes(3,20,1,7,100,0),0))return 2;
    if(expect(ready_precedes(5,10,2,5,20,1),1))return 3;
    if(expect(ready_precedes(5,20,2,5,10,1),0))return 4;
    if(expect(ready_precedes(5,20,1,5,20,2),1))return 5;
    if(expect(ready_precedes(5,20,2,5,20,1),0))return 6;
    if(expect(ready_precedes(255,1000,3,0,1,0),1))return 7;
    if(expect(ready_precedes(0,1,0,255,1000,3),0))return 8;
    return 0;
}
