#ifndef TOYOS_RT_DEADLINE_H
#define TOYOS_RT_DEADLINE_H

/* Stage 6.1: RT job timing uses the hardware PIT 100 Hz timebase.
   One RT tick is 10 ms. Legacy SYS_TIMER_GET remains a separate 50 Hz clock. */
static inline unsigned int rt_deadline_ms_to_ticks(unsigned int ms){
    unsigned int q=ms/10u;
    if(ms%10u)q++;
    return q?q:1u;
}

/* A deadline is reached at or after target in modulo-2^32 RT time space.
   The comparison is valid for the normal RT interval, which is below
   2^31 ticks. This also handles timer wrap without using signed overflow. */
static inline int rt_deadline_reached(unsigned int now,unsigned int target){
    return (unsigned int)(now-target)<0x80000000u;
}

/* Stage 5.2 FIX1: wrap-safe ordering of two absolute RT time values.
   Returns non-zero when a is strictly before b in modulo-2^32 time, provided
   the compared interval stays below 2^31 ticks (the same bounded interval
   required by rt_deadline_reached()). */
static inline int rt_deadline_before(unsigned int a,unsigned int b){
    unsigned int d=(unsigned int)(b-a);
    return d!=0u&&d<0x80000000u;
}

static inline unsigned int rt_deadline_absolute(unsigned int release_tick,unsigned int deadline_ticks){
    return release_tick+deadline_ticks;
}

/* Stage 5.3: periodic jobs use a no-backlog release policy after SYS_RT_WAIT.
   Given the next nominal release candidate and the current time, return the
   first release that is not strictly in the past.  Releases skipped because
   the previous job finished late are counted separately.  Equality is kept: a
   job that finishes exactly at its next release may start the next job now. */
static inline unsigned int rt_next_release_after(unsigned int candidate,
                                                  unsigned int now,
                                                  unsigned int period_ticks,
                                                  unsigned int *skipped){
    unsigned int delta,count;
    if(skipped)*skipped=0u;
    if(period_ticks==0u)return candidate;
    if(!rt_deadline_before(candidate,now))return candidate;
    delta=(unsigned int)(now-candidate);
    count=delta/period_ticks;
    if(delta%period_ticks)count++;
    if(skipped)*skipped=count;
    return candidate+count*period_ticks;
}

#endif
