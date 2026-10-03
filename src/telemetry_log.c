typedef unsigned char uint8_t; typedef unsigned int uint32_t; typedef signed int int32_t;
#define U __attribute__((section(".usertext")))
#define TLOG_MAGIC 0x31474c54u /* TLG1 */
#define TLOG_VERSION 1u
#define TLOG_HEADER_BYTES 32u
#define TLOG_RECORD_BYTES 32u
struct tlog_header {uint32_t magic,version,generation,next_sequence,write_index,valid_count,capacity,crc;};
struct tlog_record {uint32_t sequence,timestamp_us;int32_t x_mm,y_mm,z_mm;uint32_t status,generation,crc;};
U uint32_t tlog_crc32(const void*vp,uint32_t n){const uint8_t*p=(const uint8_t*)vp;uint32_t c=0xffffffffu,i,j;for(i=0;i<n;i++){c^=p[i];for(j=0;j<8u;j++)c=(c>>1)^((0u-(c&1u))&0xedb88320u);}return ~c;}
U void tlog_header_make(struct tlog_header*h,uint32_t generation,uint32_t next,uint32_t index,uint32_t count,uint32_t cap){h->magic=TLOG_MAGIC;h->version=TLOG_VERSION;h->generation=generation;h->next_sequence=next;h->write_index=index;h->valid_count=count;h->capacity=cap;h->crc=0u;h->crc=tlog_crc32(h,28u);}
U uint32_t tlog_header_valid(const struct tlog_header*h,uint32_t cap){return h&&h->magic==TLOG_MAGIC&&h->version==TLOG_VERSION&&h->capacity==cap&&h->write_index<cap&&h->valid_count<=cap&&tlog_crc32(h,28u)==h->crc;}
U void tlog_record_make(struct tlog_record*r,uint32_t seq,uint32_t ts,int32_t x,int32_t y,int32_t z,uint32_t status,uint32_t gen){r->sequence=seq;r->timestamp_us=ts;r->x_mm=x;r->y_mm=y;r->z_mm=z;r->status=status;r->generation=gen;r->crc=0u;r->crc=tlog_crc32(r,28u);}
U uint32_t tlog_record_valid(const struct tlog_record*r){return r&&tlog_crc32(r,28u)==r->crc;}
U uint32_t tlog_newer(uint32_t a,uint32_t b){return (int32_t)(a-b)>0;}
