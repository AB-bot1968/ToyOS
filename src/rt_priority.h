#ifndef TOYOS_RT_PRIORITY_H
#define TOYOS_RT_PRIORITY_H

/* Stage 4.1: pure priority comparator.
 *
 * Higher numeric priority means higher scheduling priority.  The comparator
 * is deliberately side-effect free and is not wired into rt_pick_ready() in
 * Stage 4.1; arbitration remains unchanged until the dedicated Stage 4.2.
 * Return value follows the conventional three-way comparison:
 *   >0  when a has higher priority than b
 *    0  when priorities are equal
 *   <0  when a has lower priority than b
 */
static inline int rt_priority_compare(uint32_t a, uint32_t b){
    if(a>b)return 1;
    if(a<b)return -1;
    return 0;
}

#endif
