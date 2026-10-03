#include <stdint.h>
#include <stdio.h>
#include "../src/rt_deadline.h"
static uint32_t next_release(uint32_t candidate,uint32_t now,uint32_t period,uint32_t *skipped){return rt_next_release_after(candidate,now,period,skipped);}
int main(void){uint32_t s,n;
 n=next_release(101u,105u,1u,&s); if(n!=105u||s!=4u){fprintf(stderr,"unit exact-grid failed n=%u s=%u\n",n,s);return 1;}
 n=next_release(105u,105u,5u,&s); if(n!=105u||s!=0u)return 2;
 n=next_release(105u,107u,5u,&s); if(n!=110u||s!=1u)return 3;
 n=next_release(105u,110u,5u,&s); if(n!=110u||s!=1u){fprintf(stderr,"period exact-grid failed n=%u s=%u\n",n,s);return 4;}
 n=next_release(0xfffffffdu,1u,1u,&s); if(n!=1u||s!=4u){fprintf(stderr,"wrap failed n=%u s=%u\n",n,s);return 5;}
 puts("rt_period_release FIX11: OK");return 0;}
