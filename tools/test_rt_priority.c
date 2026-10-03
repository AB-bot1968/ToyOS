#include <stdint.h>
#include "../src/rt_priority.h"

int main(void){
    if(rt_priority_compare(0u,0u)!=0)return 1;
    if(rt_priority_compare(1u,0u)<=0)return 2;
    if(rt_priority_compare(255u,1u)<=0)return 3;
    if(rt_priority_compare(0u,1u)>=0)return 4;
    if(rt_priority_compare(1u,255u)>=0)return 5;
    if(rt_priority_compare(127u,127u)!=0)return 6;
    return 0;
}
