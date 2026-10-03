typedef unsigned char uint8_t; typedef unsigned short uint16_t; typedef unsigned int uint32_t; typedef signed int int32_t;
#define U __attribute__((section(".usertext")))
#define SONAR_OK 1u
#define SONAR_ERR_REGS 0xffffffe1u
struct sonar_sample { int32_t x_mm,y_mm,z_mm; uint32_t status; };
U static int32_t sonar_i32(uint16_t hi,uint16_t lo){return (int32_t)(((uint32_t)hi<<16)|(uint32_t)lo);}
U uint32_t sonar_decode_xyz(const uint16_t*regs,uint32_t count,struct sonar_sample*out){if(!regs||!out||count!=6u)return SONAR_ERR_REGS;out->x_mm=sonar_i32(regs[0],regs[1]);out->y_mm=sonar_i32(regs[2],regs[3]);out->z_mm=sonar_i32(regs[4],regs[5]);out->status=0u;return SONAR_OK;}
U uint32_t sonar_pack_channel(uint32_t channel,const struct sonar_sample*s,uint32_t*q){uint32_t i;uint8_t*d,*v;if(!s||!q)return SONAR_ERR_REGS;q[0]=channel;q[1]=s->status;q[2]=12u;d=(uint8_t*)(q+3);v=(uint8_t*)&s->x_mm;for(i=0u;i<12u;i++)d[i]=v[i];for(;i<32u;i++)d[i]=0u;return SONAR_OK;}
