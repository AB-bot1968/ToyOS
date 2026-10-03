typedef unsigned char uint8_t; typedef unsigned short uint16_t; typedef unsigned int uint32_t;
#define U __attribute__((section(".usertext")))
#define MB_OK 1u
#define MB_MORE 0u
#define MB_ERR_CRC 0xfffffff1u
#define MB_ERR_FRAME 0xfffffff2u
#define MB_EXCEPTION 0xfffffff3u
#define MB_TIMEOUT 0xfffffff4u
#define MB_RETRY 2u
U uint16_t mb_crc16(const uint8_t*p,uint32_t n){uint16_t c=0xffffu;uint32_t i,j;for(i=0;i<n;i++){c^=p[i];for(j=0;j<8;j++)c=(c&1u)?(uint16_t)((c>>1)^0xa001u):(uint16_t)(c>>1);}return c;}
U uint32_t mb_build_read_input(uint8_t slave,uint16_t reg,uint16_t count,uint8_t*out,uint32_t cap){uint16_t c;if(!slave||!count||count>125u||cap<8u)return 0xffffffffu;out[0]=slave;out[1]=4u;out[2]=(uint8_t)(reg>>8);out[3]=(uint8_t)reg;out[4]=(uint8_t)(count>>8);out[5]=(uint8_t)count;c=mb_crc16(out,6u);out[6]=(uint8_t)c;out[7]=(uint8_t)(c>>8);return 8u;}
U uint32_t mb_parse_read_input(const uint8_t*p,uint32_t n,uint8_t slave,uint16_t count,uint16_t*out,uint32_t cap,uint32_t*exc){uint16_t c,got;uint32_t i,need;if(n<5u)return MB_MORE;if(p[0]!=slave)return MB_ERR_FRAME;if(p[1]==(uint8_t)(4u|0x80u)){if(n<5u)return MB_MORE;c=mb_crc16(p,3u);got=(uint16_t)p[3]|((uint16_t)p[4]<<8);if(c!=got)return MB_ERR_CRC;if(exc)*exc=p[2];return MB_EXCEPTION;}if(p[1]!=4u)return MB_ERR_FRAME;need=5u+(uint32_t)p[2];if(n<need)return MB_MORE;if(p[2]!=(uint8_t)(count*2u)||need!=n)return MB_ERR_FRAME;c=mb_crc16(p,n-2u);got=(uint16_t)p[n-2u]|((uint16_t)p[n-1u]<<8);if(c!=got)return MB_ERR_CRC;if(cap<count)return MB_ERR_FRAME;for(i=0;i<count;i++)out[i]=(uint16_t)(((uint16_t)p[3u+i*2u]<<8)|p[4u+i*2u]);return MB_OK;}
struct mb_master_state{uint32_t deadline_us;uint32_t timeout_us;uint32_t retries_left;uint32_t attempts;};
U void mb_master_start(struct mb_master_state*s,uint32_t now,uint32_t timeout,uint32_t retries){s->timeout_us=timeout;s->deadline_us=now+timeout;s->retries_left=retries;s->attempts=1u;}
U uint32_t mb_master_poll(struct mb_master_state*s,uint32_t now){if((int)(now-s->deadline_us)<0)return MB_MORE;if(s->retries_left){s->retries_left--;s->attempts++;s->deadline_us=now+s->timeout_us;return MB_RETRY;}return MB_TIMEOUT;}
