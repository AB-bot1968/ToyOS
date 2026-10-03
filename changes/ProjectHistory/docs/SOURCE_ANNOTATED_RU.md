# SOURCE ANNOTATED RU — v8

Ниже приведён построчный листинг текущих исходников и скриптов v8. Нумерация относится к файлам внутри проекта. Комментарии исходников сохранены; этот документ не является сокращённой копией.

## `src/boot.S`

**0001:** `.code16`
**0002:** `.section .text`
**0003:** `.global _start`
**0004:** `_start:`
**0005:** `    cli`
**0006:** `    xorw %ax,%ax`
**0007:** `    movw %ax,%ds`
**0008:** `    movw %ax,%es`
**0009:** `    movw %ax,%ss`
**0010:** `    movw $0x7c00,%sp`
**0011:** `    movb %dl, boot_drive`
**0012:** ``
**0013:** `    call enable_a20`
**0014:** `    call detect_memory`
**0015:** `    call load_kernel`
**0016:** ``
**0017:** `    cli`
**0018:** `    lgdt gdt_desc`
**0019:** `    movl %cr0,%eax`
**0020:** `    orl $1,%eax`
**0021:** `    movl %eax,%cr0`
**0022:** `    ljmp $0x08,$pm_entry`
**0023:** ``
**0024:** `hang:`
**0025:** `    cli`
**0026:** `    hlt`
**0027:** `    jmp hang`
**0028:** ``
**0029:** `enable_a20:`
**0030:** `    inb $0x92,%al`
**0031:** `    orb $0x02,%al`
**0032:** `    andb $0xfe,%al`
**0033:** `    outb %al,$0x92`
**0034:** `    ret`
**0035:** ``
**0036:** `# E820: sum usable (type 1) RAM, capped at 1 GiB.`
**0037:** `# The value is stored as a raw uint32_t at physical 0xB8000.`
**0038:** `detect_memory:`
**0039:** `    xorl %eax,%eax`
**0040:** `    movl %eax,memory_total`
**0041:** `    xorl %ebx,%ebx`
**0042:** `    movw $0x7000,%di`
**0043:** `    xorw %ax,%ax`
**0044:** `    movw %ax,%es`
**0045:** `.e820:`
**0046:** `    movl $0xE820,%eax`
**0047:** `    movl $0x534D4150,%edx`
**0048:** `    movl $24,%ecx`
**0049:** `    int $0x15`
**0050:** `    jc .fallback`
**0051:** `    cmpl $0x534D4150,%eax`
**0052:** `    jne .fallback`
**0053:** `    cmpl $1,16(%di)`
**0054:** `    jne .next`
**0055:** `    cmpl $0,4(%di)`
**0056:** `    jne .next`
**0057:** `    cmpl $0x40000000,0(%di)`
**0058:** `    jae .next`
**0059:** ``
**0060:** `    # Saturating add: total = min(total + length, 1 GiB).`
**0061:** `    movl memory_total,%ecx`
**0062:** `    cmpl $0x40000000,%ecx`
**0063:** `    jae .done`
**0064:** `    cmpl $0x40000000,%eax`
**0065:** `    jae .cap`
**0066:** `    movl %eax,%edx`
**0067:** `    negl %edx`
**0068:** `    cmpl %edx,%ecx`
**0069:** `    jae .cap`
**0070:** `    addl %eax,%ecx`
**0071:** `    movl %ecx,memory_total`
**0072:** `.next:`
**0073:** `    cmpl $0,%ebx`
**0074:** `    jne .e820`
**0075:** `.done:`
**0076:** `    call store_memory`
**0077:** `    ret`
**0078:** `.cap:`
**0079:** `    movl $0x40000000,memory_total`
**0080:** `    jmp .done`
**0081:** ``
**0082:** `# E801 fallback.`
**0083:** `.fallback:`
**0084:** `    movw $0xE801,%ax`
**0085:** `    int $0x15`
**0086:** `    jc .zero`
**0087:** `    # AX: KiB from 1 MiB through 16 MiB.`
**0088:** `    movzwl %ax,%eax`
**0089:** `    shll $10,%eax`
**0090:** `    addl $0x00100000,%eax`
**0091:** `    # BX: 64-KiB blocks above 16 MiB.`
**0092:** `    movzwl %bx,%ecx`
**0093:** `    shll $16,%ecx`
**0094:** `    addl %ecx,%eax`
**0095:** `    cmpl $0x40000000,%eax`
**0096:** `    jb .fallback_store`
**0097:** `    movl $0x40000000,%eax`
**0098:** `.fallback_store:`
**0099:** `    movl %eax,memory_total`
**0100:** `    call store_memory`
**0101:** `    ret`
**0102:** `.zero:`
**0103:** `    xorl %eax,%eax`
**0104:** `    movl %eax,memory_total`
**0105:** `    call store_memory`
**0106:** `    ret`
**0107:** ``
**0108:** `store_memory:`
**0109:** `    movw $0xB800,%ax`
**0110:** `    movw %ax,%es`
**0111:** `    movl memory_total,%eax`
**0112:** `    movl %eax,%es:0`
**0113:** `    xorw %ax,%ax`
**0114:** `    movw %ax,%es`
**0115:** `    ret`
**0116:** ``
**0117:** `load_kernel:`
**0118:** `    movw $dap,%si`
**0119:** `    movb boot_drive,%dl`
**0120:** `    movb $0x42,%ah`
**0121:** `    int $0x13`
**0122:** `    jc disk_fail`
**0123:** `    ret`
**0124:** ``
**0125:** `disk_fail:`
**0126:** `    movw $0xB800,%ax`
**0127:** `    movw %ax,%es`
**0128:** `    movb $'D',%es:0`
**0129:** `    movb $0x4f,%es:1`
**0130:** `    jmp hang`
**0131:** ``
**0132:** `.align 4`
**0133:** `memory_total: .long 0`
**0134:** `boot_drive:   .byte 0`
**0135:** `.align 4`
**0136:** `dap:`
**0137:** `    .byte 0x10,0`
**0138:** `    .word 40`
**0139:** `    .word 0x7e00`
**0140:** `    .word 0`
**0141:** `    .long 1`
**0142:** `    .long 0`
**0143:** ``
**0144:** `.align 8`
**0145:** `gdt:`
**0146:** `    .word 0,0,0,0`
**0147:** `    # code: base 0, limit 0x03FFFF, G=1 => exactly 1 GiB`
**0148:** `    .word 0xffff,0`
**0149:** `    .byte 0,0x9a,0xc3,0`
**0150:** `    # data: base 0, limit 0x03FFFF, G=1 => exactly 1 GiB`
**0151:** `    .word 0xffff,0`
**0152:** `    .byte 0,0x92,0xc3,0`
**0153:** `gdt_desc:`
**0154:** `    .word gdt_desc-gdt-1`
**0155:** `    .long gdt`
**0156:** ``
**0157:** `.code32`
**0158:** `pm_entry:`
**0159:** `    movw $0x10,%ax`
**0160:** `    movw %ax,%ds`
**0161:** `    movw %ax,%es`
**0162:** `    movw %ax,%ss`
**0163:** `    movw %ax,%fs`
**0164:** `    movw %ax,%gs`
**0165:** `    movl $0x200000,%esp`
**0166:** `    ljmp $0x08,$0x7e00`
**0167:** ``
**0168:** `.org 510`
**0169:** `.word 0xaa55`

## `src/isr.S`

**0001:** `.code32`
**0002:** `.section .text,"ax"`
**0003:** ``
**0004:** `.global _zero_bss`
**0005:** `_zero_bss:`
**0006:** `    pushl %edi`
**0007:** `    cld`
**0008:** `    movl $___bss_start,%edi`
**0009:** `    movl $___bss_end,%ecx`
**0010:** `    subl %edi,%ecx`
**0011:** `    xorl %eax,%eax`
**0012:** `    rep`
**0013:** `stosb`
**0014:** `    popl %edi`
**0015:** `    ret`
**0016:** ``
**0017:** `.global _isr_common`
**0018:** `_isr_common:`
**0019:** `    cld`
**0020:** `    pushl %ds`
**0021:** `    pushl %es`
**0022:** `    pushl %fs`
**0023:** `    pushl %gs`
**0024:** `    pushal`
**0025:** `    movw $0x10,%ax`
**0026:** `    movw %ax,%ds`
**0027:** `    movw %ax,%es`
**0028:** `    pushl %esp`
**0029:** `    call _interrupt_dispatch`
**0030:** `    addl $4,%esp`
**0031:** `    popal`
**0032:** `    popl %gs`
**0033:** `    popl %fs`
**0034:** `    popl %es`
**0035:** `    popl %ds`
**0036:** `    addl $8,%esp`
**0037:** `    iret`
**0038:** ``
**0039:** `.section .usertext,"ax"`
**0040:** `.global _user_entry`
**0041:** `_user_entry:`
**0042:** `    cld`
**0043:** `    movw $0x23,%ax`
**0044:** `    movw %ax,%ds`
**0045:** `    movw %ax,%es`
**0046:** `    movw %ax,%fs`
**0047:** `    movw %ax,%gs`
**0048:** `    sti`
**0049:** `    call _shell_run`
**0050:** `1:`
**0051:** `    cli`
**0052:** `    hlt`
**0053:** `    jmp 1b`
**0054:** ``
**0055:** `.section .text,"ax"`
**0056:** ``
**0057:** `.macro ISR_NOERR n`
**0058:** `.global _isr\n`
**0059:** `_isr\n:`
**0060:** `    pushl $0`
**0061:** `    pushl $\n`
**0062:** `    jmp _isr_common`
**0063:** `.endm`
**0064:** ``
**0065:** `.macro ISR_ERR n`
**0066:** `.global _isr\n`
**0067:** `_isr\n:`
**0068:** `    pushl $\n`
**0069:** `    jmp _isr_common`
**0070:** `.endm`
**0071:** ``
**0072:** `ISR_NOERR 0`
**0073:** `ISR_NOERR 1`
**0074:** `ISR_NOERR 2`
**0075:** `ISR_NOERR 3`
**0076:** `ISR_NOERR 4`
**0077:** `ISR_NOERR 5`
**0078:** `ISR_NOERR 6`
**0079:** `ISR_NOERR 7`
**0080:** `ISR_ERR   8`
**0081:** `ISR_NOERR 9`
**0082:** `ISR_ERR   10`
**0083:** `ISR_ERR   11`
**0084:** `ISR_ERR   12`
**0085:** `ISR_ERR   13`
**0086:** `ISR_ERR   14`
**0087:** `ISR_NOERR 15`
**0088:** `ISR_NOERR 16`
**0089:** `ISR_ERR   17`
**0090:** `ISR_NOERR 18`
**0091:** `ISR_NOERR 19`
**0092:** `ISR_NOERR 20`
**0093:** `ISR_NOERR 21`
**0094:** `ISR_NOERR 22`
**0095:** `ISR_NOERR 23`
**0096:** `ISR_NOERR 24`
**0097:** `ISR_NOERR 25`
**0098:** `ISR_NOERR 26`
**0099:** `ISR_NOERR 27`
**0100:** `ISR_NOERR 28`
**0101:** `ISR_NOERR 29`
**0102:** `ISR_NOERR 30`
**0103:** `ISR_NOERR 31`
**0104:** `ISR_NOERR 32`
**0105:** `ISR_NOERR 33`
**0106:** `ISR_NOERR 34`
**0107:** `ISR_NOERR 35`
**0108:** `ISR_NOERR 36`
**0109:** `ISR_NOERR 37`
**0110:** `ISR_NOERR 38`
**0111:** `ISR_NOERR 39`
**0112:** `ISR_NOERR 40`
**0113:** `ISR_NOERR 41`
**0114:** `ISR_NOERR 42`
**0115:** `ISR_NOERR 43`
**0116:** `ISR_NOERR 44`
**0117:** `ISR_NOERR 45`
**0118:** `ISR_NOERR 46`
**0119:** `ISR_NOERR 47`
**0120:** `ISR_NOERR 128`

## `src/kernel.c`

**0001:** `typedef unsigned char  uint8_t;`
**0002:** `typedef unsigned short uint16_t;`
**0003:** `typedef unsigned int   uint32_t;`
**0004:** ``
**0005:** `typedef int int32_t;`
**0006:** ``
**0007:** `#define VGA ((volatile uint16_t*)0xB8000u)`
**0008:** `#define W 80u`
**0009:** `#define H 25u`
**0010:** `#define ATTR 0x07u`
**0011:** `#define PIC1 0x20u`
**0012:** `#define PIC2 0xA0u`
**0013:** `#define PIT 0x40u`
**0014:** `#define KBD 0x60u`
**0015:** `#define ATA_DATA 0x1F0u`
**0016:** `#define ATA_ERR 0x1F1u`
**0017:** `#define ATA_NSECT 0x1F2u`
**0018:** `#define ATA_LBA0 0x1F3u`
**0019:** `#define ATA_LBA1 0x1F4u`
**0020:** `#define ATA_LBA2 0x1F5u`
**0021:** `#define ATA_DRIVE 0x1F6u`
**0022:** `#define ATA_STATUS 0x1F7u`
**0023:** `#define ATA_CTRL 0x3F6u`
**0024:** `#define FAT_LBA 42u`
**0025:** `#define SECTOR_SIZE 512u`
**0026:** `#define FAT16_EOC 0xfff8u`
**0027:** `#define FAT16_BAD 0xfff7u`
**0028:** `#define FAT16_FREE 0x0000u`
**0029:** `#define FAT16_MAX_HANDLES 8u`
**0030:** `#define FAT16_MODE_READ   0x01u`
**0031:** `#define FAT16_MODE_WRITE  0x02u`
**0032:** `#define FAT16_MODE_CREATE 0x04u`
**0033:** `#define FAT16_MODE_TRUNC  0x08u`
**0034:** `#define FAT16_MODE_APPEND 0x10u`
**0035:** ``
**0036:** `#define SYS_CONSOLE_WRITE 1u`
**0037:** `#define SYS_CONSOLE_READ  2u`
**0038:** `#define SYS_TIMER_GET     3u`
**0039:** `#define SYS_DISK_READ     4u`
**0040:** `#define SYS_DISK_WRITE    5u`
**0041:** `#define SYS_FILE_OPEN     6u`
**0042:** `#define SYS_FILE_READ     7u`
**0043:** `#define SYS_FILE_WRITE    8u`
**0044:** `#define SYS_FILE_CLOSE    9u`
**0045:** ``
**0046:** `#define FAT_OK 0`
**0047:** `#define FAT_ERR (-1)`
**0048:** `#define FAT_EOF 0`
**0049:** ``
**0050:** `static inline void outb(uint16_t p,uint8_t v){__asm__ volatile("outb %0,%1"::"a"(v),"Nd"(p));}`
**0051:** `static inline uint8_t inb(uint16_t p){uint8_t v;__asm__ volatile("inb %1,%0":"=a"(v):"Nd"(p));return v;}`
**0052:** `static inline void outw(uint16_t p,uint16_t v){__asm__ volatile("outw %0,%1"::"a"(v),"Nd"(p));}`
**0053:** `static inline uint16_t inw(uint16_t p){uint16_t v;__asm__ volatile("inw %1,%0":"=a"(v):"Nd"(p));return v;}`
**0054:** `static inline void io_delay(void){(void)inb(0x80);}`
**0055:** `static inline void cpu_hlt(void){__asm__ volatile("hlt");}`
**0056:** ``
**0057:** `static void mem_set(void *d,uint8_t v,uint32_t n){uint8_t *p=(uint8_t*)d;while(n--)*p++=v;}`
**0058:** `static void mem_copy(void *d,const void*s,uint32_t n){uint8_t *a=(uint8_t*)d;const uint8_t*b=(const uint8_t*)s;while(n--)*a++=*b++;}`
**0059:** `static uint32_t str_len(const char*s){uint32_t n=0;while(s[n])n++;return n;}`
**0060:** ``
**0061:** `static uint32_t vga_x,vga_y;`
**0062:** `static void vga_cell(uint32_t x,uint32_t y,char c){if(x<W&&y<H)VGA[y*W+x]=((uint16_t)ATTR<<8)|(uint8_t)c;}`
**0063:** `static void console_clear(void){uint32_t i;for(i=0;i<W*H;i++)VGA[i]=((uint16_t)ATTR<<8)|' ';vga_x=vga_y=0;}`
**0064:** `static void console_put(char c){if(c=='\n'){vga_x=0;if(++vga_y>=H)vga_y=0;return;}if(c=='\r'){vga_x=0;return;}if(c=='\b'){if(vga_x)vga_x--;vga_cell(vga_x,vga_y,' ');return;}vga_cell(vga_x,vga_y,c);if(++vga_x>=W){vga_x=0;if(++vga_y>=H)vga_y=0;}}`
**0065:** `static void console_write_n(const char*s,uint32_t n){while(n--)console_put(*s++);}`
**0066:** `static void console_write(const char*s){console_write_n(s,str_len(s));}`
**0067:** `static void u32_dec(uint32_t v,char*b){static const uint32_t p[10]={1000000000u,100000000u,10000000u,1000000u,100000u,10000u,1000u,100u,10u,1u};uint32_t i,d,st=0;for(i=0;i<10;i++){d=0;while(v>=p[i]){v-=p[i];d++;}if(d||st||i==9){*b++=(char)('0'+d);st=1;}}*b=0;}`
**0068:** `static void console_write_u32(uint32_t v){char b[11];u32_dec(v,b);console_write(b);}`
**0069:** ``
**0070:** `struct idt_gate{uint16_t off_lo,sel;uint8_t zero,type;uint16_t off_hi;} __attribute__((packed));`
**0071:** `static struct idt_gate idt[256];`
**0072:** `struct idtr{uint16_t limit;uint32_t base;} __attribute__((packed));`
**0073:** `struct gdtr{uint16_t limit;uint32_t base;} __attribute__((packed));`
**0074:** `struct gdt_desc{uint16_t limit_lo,base_lo;uint8_t base_mid,access,gran,base_hi;} __attribute__((packed));`
**0075:** `struct tss32{uint16_t link,res0;uint32_t esp0;uint16_t ss0,res1;uint32_t esp1;uint16_t ss1,res2;uint32_t esp2;uint16_t ss2,res3;uint32_t cr3,eip,eflags,eax,ecx,edx,ebx,esp,ebp,esi,edi;uint16_t es,res4,cs,res5,ss,res6,ds,res7,fs,res8,gs,res9,ldt,res10;uint16_t trap,iobase;} __attribute__((packed));`
**0076:** `static struct gdt_desc gdt[6];`
**0077:** `static struct tss32 tss;`
**0078:** ``
**0079:** `/* Page directory and the first 4-MiB page table are used to make the`
**0080:** `   CPL3 shell a real paged user area instead of relying only on segmentation. */`
**0081:** `static uint32_t page_directory[1024] __attribute__((aligned(4096)));`
**0082:** `static uint32_t page_table0[1024] __attribute__((aligned(4096)));`
**0083:** `extern char __user_text_start[];`
**0084:** `extern char __user_text_end[];`
**0085:** `extern char __user_rodata_start[];`
**0086:** `extern char __user_rodata_end[];`
**0087:** `#define PAGE_P 0x001u`
**0088:** `#define PAGE_RW 0x002u`
**0089:** `#define PAGE_US 0x004u`
**0090:** `#define USER_STACK_PAGE 0x003ff000u`
**0091:** `#define USER_STACK_TOP  0x003fffe0u`
**0092:** `#define KERNEL_STACK_TOP 0x001f0000u`
**0093:** `static void gdt_set(struct gdt_desc*d,uint32_t base,uint32_t limit,uint8_t access,uint8_t gran){d->limit_lo=(uint16_t)(limit&0xffffu);d->base_lo=(uint16_t)base;d->base_mid=(uint8_t)(base>>16);d->access=access;d->gran=(uint8_t)((limit>>16)&0x0f)|(uint8_t)(gran&0xf0);d->base_hi=(uint8_t)(base>>24);}`
**0094:** `static void gdt_init_ring3(void){`
**0095:** `    struct gdtr r;`
**0096:** `    uint32_t base=(uint32_t)&tss;`
**0097:** `    mem_set(gdt,0,sizeof(gdt));`
**0098:** `    gdt_set(&gdt[1],0,0x3ffff,0x9a,0xc0);`
**0099:** `    gdt_set(&gdt[2],0,0x3ffff,0x92,0xc0);`
**0100:** `    gdt_set(&gdt[3],0,0x3ffff,0xfa,0xc0);`
**0101:** `    gdt_set(&gdt[4],0,0x3ffff,0xf2,0xc0);`
**0102:** `    gdt_set(&gdt[5],base,(uint32_t)sizeof(struct tss32)-1u,0x89,0x00);`
**0103:** `    tss.esp0=KERNEL_STACK_TOP;`
**0104:** `    tss.ss0=0x10u;`
**0105:** `    tss.iobase=(uint16_t)sizeof(struct tss32);`
**0106:** `    r.limit=(uint16_t)(sizeof(gdt)-1u); r.base=(uint32_t)gdt;`
**0107:** `    __asm__ volatile("lgdtl %0"::"m"(r):"memory");`
**0108:** `    __asm__ volatile("movw $0x10,%%ax;movw %%ax,%%ds;movw %%ax,%%es;movw %%ax,%%ss;movw %%ax,%%fs;movw %%ax,%%gs;ljmp $0x08,$1f;1:":: :"ax","memory");`
**0109:** `    __asm__ volatile("ltr %w0"::"a"((uint16_t)0x28):"memory");`
**0110:** `}`
**0111:** `static void paging_mark_user_ro(uint32_t start,uint32_t end){`
**0112:** `    uint32_t p;`
**0113:** `    start&=~0xfffu;`
**0114:** `    end=(end+0xfffu)&~0xfffu;`
**0115:** `    for(p=start;p<end&&p<0x00400000u;p+=0x1000u){page_table0[p>>12]|=PAGE_US;page_table0[p>>12]&=~PAGE_RW;}`
**0116:** `}`
**0117:** `static void paging_init(void){`
**0118:** `    uint32_t i;`
**0119:** `    mem_set(page_directory,0,sizeof(page_directory));`
**0120:** `    mem_set(page_table0,0,sizeof(page_table0));`
**0121:** `    for(i=0;i<1024u;i++)page_table0[i]=(i<<12)|PAGE_P|PAGE_RW;`
**0122:** `    page_directory[0]=(uint32_t)page_table0|PAGE_P|PAGE_RW|PAGE_US;`
**0123:** `    paging_mark_user_ro((uint32_t)__user_text_start,(uint32_t)__user_text_end);`
**0124:** `    paging_mark_user_ro((uint32_t)__user_rodata_start,(uint32_t)__user_rodata_end);`
**0125:** `    page_table0[USER_STACK_PAGE>>12]=USER_STACK_PAGE|PAGE_P|PAGE_RW|PAGE_US;`
**0126:** `    __asm__ volatile("movl %0,%%cr3"::"r"((uint32_t)page_directory):"memory");`
**0127:** `    __asm__ volatile("movl %%cr0,%%eax; orl $0x80000000,%%eax; movl %%eax,%%cr0":: :"eax","memory");`
**0128:** `}`
**0129:** `static void enter_user_shell(void){`
**0130:** `    /* Build only the architectural IRET frame while still using kernel segments.`
**0131:** `       User data segments are loaded by _user_entry after the CPL3 transition.`
**0132:** `       IF stays clear until the user segment registers are valid. */`
**0133:** `    __asm__ volatile("cli;pushl $0x23;pushl $0x003fffe0;pushfl;pushl $0x1b;pushl $_user_entry;iret":: :"memory");`
**0134:** `}`
**0135:** `static void idt_set(uint8_t n,void(*fn)(void),uint8_t dpl){uint32_t a=(uint32_t)fn;idt[n].off_lo=(uint16_t)a;idt[n].sel=8;idt[n].zero=0;idt[n].type=(uint8_t)(0x8e|(dpl?0x60:0));idt[n].off_hi=(uint16_t)(a>>16);}`
**0136:** ``
**0137:** `/* The common stub pushes PUSHA after the four segment registers.`
**0138:** `   Therefore memory at the dispatcher argument starts with EDI..EAX,`
**0139:** `   followed by GS..DS, then INT number/error and the CPU frame. */`
**0140:** `struct frame{uint32_t edi,esi,ebp,oes,ebx,edx,ecx,eax;uint32_t gs,fs,es,ds;uint32_t int_no,error;uint32_t eip,cs,eflags;} __attribute__((packed));`
**0141:** `static volatile uint32_t timer_ticks;`
**0142:** `static volatile uint8_t ata_irq_seen;`
**0143:** `static int disk_read(uint32_t,void*,uint32_t); static int disk_write(uint32_t,const void*,uint32_t);`
**0144:** `static uint8_t keybuf[256];static volatile uint8_t khead,ktail;static uint8_t shift_state;`
**0145:** `static const char keymap[128]="\0\0\0331234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;'\`\0\\zxcvbnm,./\0\0\0 \0";`
**0146:** `static const char keymap_shift[128]="\0\0\033!@#$%^&*()_+\b\tQWERTYUIOP{}\n\0ASDFGHJKL:\"~\0|ZXCVBNM<>?\0\0\0 \0";`
**0147:** `static void key_push(uint8_t c){uint8_t n=(uint8_t)(khead+1);if(n!=ktail){keybuf[khead]=c;khead=n;}}`
**0148:** `static uint8_t key_pop(void){uint8_t c;if(khead==ktail)return 0;c=keybuf[ktail];ktail++;return c;}`
**0149:** ``
**0150:** `static void pic_remap(void){`
**0151:** `    /* Keep both 8259A chips completely masked during the whole ICW sequence.`
**0152:** `       Do not inherit BIOS masks: a stale/unexpected IRQ must not reach an`
**0153:** `       exception vector while protected mode is being prepared. */`
**0154:** `    outb(PIC1+1,0xff);`
**0155:** `    outb(PIC2+1,0xff);`
**0156:** `    io_delay();`
**0157:** ``
**0158:** `    /* ICW1: cascaded PICs, ICW4 follows. */`
**0159:** `    outb(PIC1,0x11); io_delay();`
**0160:** `    outb(PIC2,0x11); io_delay();`
**0161:** `    /* ICW2: IRQ0..7 -> INT 32..39, IRQ8..15 -> INT 40..47. */`
**0162:** `    outb(PIC1+1,0x20); io_delay();`
**0163:** `    outb(PIC2+1,0x28); io_delay();`
**0164:** `    /* ICW3: master has slave on IRQ2; slave identity is 2. */`
**0165:** `    outb(PIC1+1,0x04); io_delay();`
**0166:** `    outb(PIC2+1,0x02); io_delay();`
**0167:** `    /* ICW4: 8086/88 mode, normal EOI. */`
**0168:** `    outb(PIC1+1,0x01); io_delay();`
**0169:** `    outb(PIC2+1,0x01); io_delay();`
**0170:** ``
**0171:** `    /* Known-safe mask: only IRQ0 (timer) and IRQ1 (keyboard) may later be`
**0172:** `       enabled explicitly by irq_enable().  IRQ2 remains masked because the`
**0173:** `       slave PIC is not used by this toy kernel; ATA IRQ14 also stays masked. */`
**0174:** `    outb(PIC1+1,0xfc);`
**0175:** `    outb(PIC2+1,0xff);`
**0176:** `    io_delay();`
**0177:** `}`
**0178:** `static void irq_enable(uint8_t irq){uint16_t p=irq<8?PIC1+1:PIC2+1;uint8_t m=inb(p);if(irq<8)m&=(uint8_t)~(1u<<irq);else m&=(uint8_t)~(1u<<(irq-8));outb(p,m);}`
**0179:** `static void irq_eoi(uint32_t n){if(n>=40)outb(PIC2,0x20);if(n>=32&&n<48)outb(PIC1,0x20);}`
**0180:** `static void pit_init(void){uint32_t div=1193182u/100u;outb(0x43,0x36);outb(PIT,(uint8_t)div);outb(PIT,(uint8_t)(div>>8));}`
**0181:** ``
**0182:** `static void keyboard_irq(void){uint8_t s=inb(KBD);if(s==0xe0||s==0xe1)return;if(s==0x2a||s==0x36){shift_state=1;return;}if(s==0xaa||s==0xb6){shift_state=0;return;}if(!(s&0x80)&&s<128){char c=shift_state?keymap_shift[s]:keymap[s];if(c)key_push((uint8_t)c);}}`
**0183:** `static void ata_irq(void){(void)inb(ATA_STATUS);ata_irq_seen=1;}`
**0184:** ``
**0185:** `/* ---------- ATA PIO ---------- */`
**0186:** `static void ata_400ns(void){uint32_t i;for(i=0;i<4;i++)io_delay();}`
**0187:** `static int ata_ready(void){uint32_t i;uint8_t s;for(i=0;i<100000u;i++){s=inb(ATA_STATUS);if(!(s&0x80)&&(s&0x40))return 1;io_delay();}return 0;}`
**0188:** `static int ata_wait_drq(void){uint32_t i;uint8_t s;for(i=0;i<100000u;i++){s=inb(ATA_STATUS);if(s&1)return 0;if(s&0x20)return 0;if(!(s&0x80)&&(s&8))return 1;io_delay();}return 0;}`
**0189:** `static int ata_read(uint32_t lba,void*buf){uint32_t i;if(!ata_ready())return 0;outb(ATA_CTRL,0);outb(ATA_DRIVE,(uint8_t)(0xe0|((lba>>24)&15)));outb(ATA_NSECT,1);outb(ATA_LBA0,(uint8_t)lba);outb(ATA_LBA1,(uint8_t)(lba>>8));outb(ATA_LBA2,(uint8_t)(lba>>16));outb(ATA_STATUS,0x20);if(!ata_wait_drq())return 0;for(i=0;i<256;i++)((uint16_t*)buf)[i]=inw(ATA_DATA);ata_400ns();return 1;}`
**0190:** `static int ata_write(uint32_t lba,const void*buf){uint32_t i;if(!ata_ready())return 0;outb(ATA_CTRL,0);outb(ATA_DRIVE,(uint8_t)(0xe0|((lba>>24)&15)));outb(ATA_NSECT,1);outb(ATA_LBA0,(uint8_t)lba);outb(ATA_LBA1,(uint8_t)(lba>>8));outb(ATA_LBA2,(uint8_t)(lba>>16));outb(ATA_STATUS,0x30);if(!ata_wait_drq())return 0;for(i=0;i<256;i++)outw(ATA_DATA,((const uint16_t*)buf)[i]);outb(ATA_STATUS,0xe7);return ata_ready();}`
**0191:** `static int disk_read(uint32_t lba,void*buf,uint32_t n){uint32_t i;uint8_t*p=(uint8_t*)buf;for(i=0;i<n;i++,lba++,p+=SECTOR_SIZE)if(!ata_read(lba,p))return 0;return 1;}`
**0192:** `static int disk_write(uint32_t lba,const void*buf,uint32_t n){uint32_t i;const uint8_t*p=(const uint8_t*)buf;for(i=0;i<n;i++,lba++,p+=SECTOR_SIZE)if(!ata_write(lba,p))return 0;return 1;}`
**0193:** ``
**0194:** `/* ---------- FAT16 ---------- */`
**0195:** `struct fat16{uint32_t part,fat_start,root_start,data_start,total_sectors,data_sectors;uint16_t reserved,fats,spf,root_entries,max_cluster;uint8_t spc,shift,ok;} fat;`
**0196:** `static uint8_t sec[SECTOR_SIZE];`
**0197:** `static uint16_t le16(const uint8_t*p){return (uint16_t)p[0]|((uint16_t)p[1]<<8);}`
**0198:** `static uint32_t le32(const uint8_t*p){return (uint32_t)le16(p)|((uint32_t)le16(p+2)<<16);}`
**0199:** `static void st16(uint8_t*p,uint16_t v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);}`
**0200:** `static void st32(uint8_t*p,uint32_t v){st16(p,(uint16_t)v);st16(p+2,(uint16_t)(v>>16));}`
**0201:** `static int fat_read_sector(uint32_t lba){return ata_read(lba,sec);}`
**0202:** `static int fat_write_sector(uint32_t lba){return ata_write(lba,sec);}`
**0203:** ``
**0204:** `static int fat_mount(uint32_t part){uint32_t total,rootsec,used,datasec,clusters;uint8_t spc,shift=0;uint16_t bps,res,fats,root,spf;`
**0205:** `    if(!fat_read_sector(part))return 0;`
**0206:** `    if(sec[510]!=0x55||sec[511]!=0xaa)return 0;`
**0207:** `    bps=le16(sec+11);spc=sec[13];res=le16(sec+14);fats=sec[16];root=le16(sec+17);spf=le16(sec+22);`
**0208:** `    total=le16(sec+19);if(!total)total=le32(sec+32);`
**0209:** `    if(bps!=SECTOR_SIZE||!spc||spc>128||(spc&(spc-1))||!res||!fats||!spf||!root||!total)return 0;`
**0210:** `    rootsec=((uint32_t)root+15u)>>4;`
**0211:** `    used=(uint32_t)res+(uint32_t)fats*spf+rootsec;`
**0212:** `    if(total<=used)return 0;`
**0213:** `    datasec=total-used;`
**0214:** `    while(((uint8_t)1u<<shift)<spc)shift++;`
**0215:** `    clusters=datasec>>shift;`
**0216:** `    if(clusters<4085u||clusters>=65525u)return 0;`
**0217:** `    fat.part=part;fat.reserved=res;fat.fats=fats;fat.spf=spf;fat.root_entries=root;`
**0218:** `    fat.fat_start=part+res;fat.root_start=fat.fat_start+(uint32_t)fats*spf;fat.data_start=part+used;`
**0219:** `    fat.total_sectors=total;fat.data_sectors=datasec;fat.spc=spc;fat.shift=(uint8_t)(9u+shift);fat.max_cluster=(uint16_t)(clusters+1u);fat.ok=1;return 1;`
**0220:** `}`
**0221:** ``
**0222:** `static void make83(const char*in,uint8_t out[11]){uint32_t i=0,j=0;mem_set(out,' ',11);while(in[i]&&in[i]!='.'&&j<8){char c=in[i++];if(c>='a'&&c<='z')c-=32;out[j++]=(uint8_t)c;}if(in[i]=='.'){i++;j=8;while(in[i]&&j<11){char c=in[i++];if(c>='a'&&c<='z')c-=32;out[j++]=(uint8_t)c;}}}`
**0223:** ``
**0224:** `/* Returns directory entry address as sector+offset. Only the FAT16 root directory is supported. */`
**0225:** `static int fat_find(const char*name,uint32_t*dir_lba,uint16_t*off){uint8_t n[11];uint32_t s,e,idx;make83(name,n);s=fat.root_start;e=s+(((uint32_t)fat.root_entries+15u)>>4);`
**0226:** `    for(;s<e;s++){`
**0227:** `        if(!fat_read_sector(s))return 0;`
**0228:** `        for(idx=0;idx<16;idx++){`
**0229:** `            uint8_t*d=sec+idx*32;uint32_t i;`
**0230:** `            if(d[0]==0)return 0;`
**0231:** `            if(d[0]==0xe5||d[11]==0x0f||(d[11]&0x10))continue;`
**0232:** `            for(i=0;i<11&&d[i]==n[i];i++){}`
**0233:** `            if(i==11){*dir_lba=s;*off=(uint16_t)(idx*32);return 1;}`
**0234:** `        }`
**0235:** `    }`
**0236:** `    return 0;`
**0237:** `}`
**0238:** ``
**0239:** `static int fat_find_free_dir(uint32_t*dir_lba,uint16_t*off){uint32_t s,e,idx; s=fat.root_start;e=s+(((uint32_t)fat.root_entries+15u)>>4);`
**0240:** `    for(;s<e;s++){if(!fat_read_sector(s))return 0;for(idx=0;idx<16;idx++){uint8_t*d=sec+idx*32;if(d[0]==0x00||d[0]==0xe5){*dir_lba=s;*off=(uint16_t)(idx*32);return 1;}}}return 0;}`
**0241:** ``
**0242:** `static int fat_get(uint16_t cluster,uint16_t*out){uint32_t o=(uint32_t)cluster*2u,l=fat.fat_start+(o>>9);if(cluster<2||cluster>fat.max_cluster)return 0;if(!fat_read_sector(l))return 0;*out=le16(sec+(o&511));return 1;}`
**0243:** `static int fat_set_one(uint32_t fat_base,uint16_t cluster,uint16_t value){uint32_t o=(uint32_t)cluster*2u,l=fat_base+(o>>9),off=o&511;if(!fat_read_sector(l))return 0;st16(sec+off,value);return fat_write_sector(l);}`
**0244:** `static int fat_set(uint16_t cluster,uint16_t value){uint16_t i;if(cluster<2||cluster>fat.max_cluster)return 0;for(i=0;i<fat.fats;i++)if(!fat_set_one(fat.fat_start+(uint32_t)i*fat.spf,cluster,value))return 0;return 1;}`
**0245:** ``
**0246:** `static int fat_alloc_cluster(uint16_t*out){uint32_t c;uint16_t v;for(c=2;c<=fat.max_cluster;c++){if(!fat_get((uint16_t)c,&v))return 0;if(v==FAT16_FREE){if(!fat_set((uint16_t)c,FAT16_EOC))return 0;*out=(uint16_t)c;return 1;}}return 0;}`
**0247:** `static uint32_t fat_cluster_lba(uint16_t cluster){return fat.data_start+(uint32_t)(cluster-2u)*fat.spc;}`
**0248:** `static int fat_zero_cluster(uint16_t cluster){uint32_t i;mem_set(sec,0,SECTOR_SIZE);for(i=0;i<fat.spc;i++)if(!fat_write_sector(fat_cluster_lba(cluster)+i))return 0;return 1;}`
**0249:** ``
**0250:** `static int fat_chain_last(uint16_t first,uint16_t*out_last,uint32_t*count){uint16_t cur=first,next;uint32_t n=1;if(cur<2||cur>fat.max_cluster)return 0;for(;;){if(!fat_get(cur,&next))return 0;if(next>=FAT16_EOC){*out_last=cur;if(count)*count=n;return 1;}if(next<2||next>fat.max_cluster||next==FAT16_BAD)return 0;cur=next;if(++n>fat.data_sectors/fat.spc+1u)return 0;}}`
**0251:** ``
**0252:** `static int fat_chain_nth(uint16_t first,uint32_t index,uint16_t*out){uint16_t cur=first,next;while(index--){if(!fat_get(cur,&next))return 0;if(next>=FAT16_EOC||next<2||next>fat.max_cluster||next==FAT16_BAD)return 0;cur=next;}*out=cur;return 1;}`
**0253:** ``
**0254:** `static int fat_extend(uint16_t first,uint32_t needed,uint16_t*last_out){uint16_t last,newc;uint32_t count=0;if(!fat_chain_last(first,&last,&count))return 0;while(count<needed){if(!fat_alloc_cluster(&newc))return 0;if(!fat_set(last,newc))return 0;if(!fat_set(newc,FAT16_EOC))return 0;if(!fat_zero_cluster(newc))return 0;last=newc;count++;}if(last_out)*last_out=last;return 1;}`
**0255:** ``
**0256:** `struct fat_handle{uint8_t used,mode;uint16_t start;uint32_t size,pos;uint32_t dir_lba;uint16_t dir_off;};`
**0257:** `static struct fat_handle handles[FAT16_MAX_HANDLES];`
**0258:** ``
**0259:** `static int fat_sync_dir(struct fat_handle*h){if(!fat_read_sector(h->dir_lba))return 0;st16(sec+h->dir_off+26,h->start);st32(sec+h->dir_off+28,h->size);return fat_write_sector(h->dir_lba);}`
**0260:** ``
**0261:** `static int fat_create(const char*name,uint32_t mode,struct fat_handle*h){uint32_t lba;uint16_t off;uint8_t n[11];uint16_t cl=0;`
**0262:** `    if(fat_find(name,&lba,&off)){`
**0263:** `        if(!(mode&FAT16_MODE_TRUNC))return 0;`
**0264:** `        if(!fat_read_sector(lba))return 0;`
**0265:** `        cl=le16(sec+off+26);`
**0266:** `        if(cl>=2&&cl<=fat.max_cluster){`
**0267:** `            uint16_t cur=cl,next;`
**0268:** `            for(;;){`
**0269:** `                if(!fat_get(cur,&next))return 0;`
**0270:** `                if(!fat_set(cur,FAT16_FREE))return 0;`
**0271:** `                if(next>=FAT16_EOC)break;`
**0272:** `                if(next<2||next>fat.max_cluster||next==FAT16_BAD)return 0;`
**0273:** `                cur=next;`
**0274:** `            }`
**0275:** `        }`
**0276:** `        if(!fat_read_sector(lba))return 0;`
**0277:** `        st16(sec+off+26,0);`
**0278:** `        st32(sec+off+28,0);`
**0279:** `        if(!fat_write_sector(lba))return 0;`
**0280:** `        h->dir_lba=lba;h->dir_off=off;h->start=0;h->size=0;h->pos=0;`
**0281:** `        return 1;`
**0282:** `    }`
**0283:** `    if(!(mode&FAT16_MODE_CREATE))return 0;`
**0284:** `    if(!fat_find_free_dir(&lba,&off))return 0;`
**0285:** `    make83(name,n);`
**0286:** `    if(!fat_read_sector(lba))return 0;`
**0287:** `    mem_set(sec+off,0,32);`
**0288:** `    mem_copy(sec+off,n,11);`
**0289:** `    sec[off+11]=0x20;`
**0290:** `    st16(sec+off+26,0);`
**0291:** `    st32(sec+off+28,0);`
**0292:** `    if(!fat_write_sector(lba))return 0;`
**0293:** `    h->dir_lba=lba;h->dir_off=off;h->start=0;h->size=0;h->pos=0;`
**0294:** `    return 1;`
**0295:** `}`
**0296:** ``
**0297:** `/* Open/create a root-directory 8.3 file. Directories, paths and LFN are unsupported. */`
**0298:** `static int fat_open(const char*name,uint32_t mode){uint32_t i,lba;uint16_t off;struct fat_handle*h;uint8_t exists;`
**0299:** `    if(!fat.ok||!name||!name[0])return -1;`
**0300:** `    for(i=0;i<FAT16_MAX_HANDLES;i++){`
**0301:** `        if(!handles[i].used)break;`
**0302:** `    }`
**0303:** `    if(i==FAT16_MAX_HANDLES)return -1;`
**0304:** `    h=&handles[i];mem_set(h,0,sizeof(*h));h->mode=(uint8_t)mode;`
**0305:** `    exists=(uint8_t)fat_find(name,&lba,&off);`
**0306:** `    if(!exists){if(!(mode&FAT16_MODE_CREATE)||!(mode&FAT16_MODE_WRITE))return -1;if(!fat_create(name,mode,h))return -1;}`
**0307:** `    else {if(!fat_read_sector(lba))return -1;h->dir_lba=lba;h->dir_off=off;h->start=le16(sec+off+26);h->size=le32(sec+off+28);h->pos=(mode&FAT16_MODE_APPEND)?h->size:0;if((mode&FAT16_MODE_TRUNC)&&!(mode&FAT16_MODE_WRITE))return -1;if(mode&FAT16_MODE_TRUNC){if(!fat_create(name,mode,h))return -1;}}`
**0308:** `    if((mode&FAT16_MODE_APPEND)==0&&!(mode&FAT16_MODE_WRITE))h->pos=0;`
**0309:** `    h->used=1;`
**0310:** `    return (int)i;`
**0311:** `}`
**0312:** ``
**0313:** `static int fat_close(int fd){if(fd<0||fd>=(int)FAT16_MAX_HANDLES||!handles[fd].used)return 0;handles[fd].used=0;return 1;}`
**0314:** ``
**0315:** `static int fat_read_file_fd(int fd,void*dst,uint32_t count){struct fat_handle*h;uint8_t*out=(uint8_t*)dst;uint32_t done=0;uint32_t cluster_size=(uint32_t)fat.spc*SECTOR_SIZE;if(fd<0||fd>=(int)FAT16_MAX_HANDLES||!handles[fd].used||!dst)return -1;h=&handles[fd];if(h->pos>=h->size)return FAT_EOF;if(count>h->size-h->pos)count=h->size-h->pos;while(done<count){uint32_t cluster_index=h->pos/cluster_size,within=h->pos%cluster_size,sector_index=within/SECTOR_SIZE,sector_off=within%SECTOR_SIZE,n=SECTOR_SIZE-sector_off;uint16_t cl;if(n>count-done)n=count-done;if(!fat_chain_nth(h->start,cluster_index,&cl))return -1;if(!fat_read_sector(fat_cluster_lba(cl)+sector_index))return -1;mem_copy(out+done,sec+sector_off,n);done+=n;h->pos+=n;}return (int)done;}`
**0316:** ``
**0317:** `static int fat_write_file_fd(int fd,const void*src,uint32_t count){struct fat_handle*h;const uint8_t*in=(const uint8_t*)src;uint32_t done=0,cluster_size=(uint32_t)fat.spc*SECTOR_SIZE,need_clusters;uint16_t first,last;`
**0318:** `    if(fd<0||fd>=(int)FAT16_MAX_HANDLES||!handles[fd].used||!src)return -1;`
**0319:** `    h=&handles[fd];`
**0320:** `    if(!(h->mode&FAT16_MODE_WRITE))return -1;`
**0321:** `    if(!count)return 0;`
**0322:** `    if(h->pos>0xffffffffu-count)return -1;`
**0323:** `    if(h->pos+count>(uint32_t)fat.data_sectors*SECTOR_SIZE)return -1;`
**0324:** `    if(h->start==0){if(!fat_alloc_cluster(&first))return -1;if(!fat_zero_cluster(first))return -1;h->start=first;if(!fat_sync_dir(h))return -1;}`
**0325:** `    need_clusters=(h->pos+count+cluster_size-1u)/cluster_size;if(!fat_extend(h->start,need_clusters,&last))return -1;`
**0326:** `    while(done<count){uint32_t ci=h->pos/cluster_size,within=h->pos%cluster_size,si=within/SECTOR_SIZE,so=within%SECTOR_SIZE,n=SECTOR_SIZE-so;uint16_t cl;if(n>count-done)n=count-done;if(!fat_chain_nth(h->start,ci,&cl))return -1;`
**0327:** `        if(so!=0||n!=SECTOR_SIZE){if(!fat_read_sector(fat_cluster_lba(cl)+si))return -1;}else mem_set(sec,0,SECTOR_SIZE);mem_copy(sec+so,in+done,n);if(!fat_write_sector(fat_cluster_lba(cl)+si))return -1;done+=n;h->pos+=n;if(h->pos>h->size)h->size=h->pos;}`
**0328:** `    if(!fat_sync_dir(h))return -1;`
**0329:** `    return (int)done;`
**0330:** `}`
**0331:** ``
**0332:** `/* Convenience read-only API retained for simple kernel tests. */`
**0333:** `static int fat_read_file(const char*name,void*dst,uint32_t cap,uint32_t*got){`
**0334:** `    int fd=fat_open(name,FAT16_MODE_READ);int n;`
**0335:** `    if(fd<0)return 0;`
**0336:** `    n=fat_read_file_fd(fd,dst,cap);`
**0337:** `    fat_close(fd);`
**0338:** `    if(n<0)return 0;`
**0339:** `    *got=(uint32_t)n;`
**0340:** `    return 1;`
**0341:** `}`
**0342:** ``
**0343:** `/* ---------- Syscalls ---------- */`
**0344:** `static uint32_t syscall3(uint32_t n,uint32_t a,uint32_t b,uint32_t c){uint32_t r;__asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");return r;}`
**0345:** `uint32_t sys_console_write(const char*s,uint32_t n){return syscall3(SYS_CONSOLE_WRITE,(uint32_t)s,n,0);}uint32_t sys_console_read(char*b,uint32_t n){return syscall3(SYS_CONSOLE_READ,(uint32_t)b,n,0);}uint32_t sys_timer_get(void){return syscall3(SYS_TIMER_GET,0,0,0);}uint32_t sys_disk_read(uint32_t lba,void*b,uint32_t n){return syscall3(SYS_DISK_READ,lba,(uint32_t)b,n);}uint32_t sys_disk_write(uint32_t lba,const void*b,uint32_t n){return syscall3(SYS_DISK_WRITE,lba,(uint32_t)b,n);}`
**0346:** `uint32_t sys_file_open(const char*name,uint32_t mode){return syscall3(SYS_FILE_OPEN,(uint32_t)name,mode,0);}uint32_t sys_file_read(uint32_t fd,void*b,uint32_t n){return syscall3(SYS_FILE_READ,fd,(uint32_t)b,n);}uint32_t sys_file_write(uint32_t fd,const void*b,uint32_t n){return syscall3(SYS_FILE_WRITE,fd,(uint32_t)b,n);}uint32_t sys_file_close(uint32_t fd){return syscall3(SYS_FILE_CLOSE,fd,0,0);}`
**0347:** ``
**0348:** `#define DECL_ISR(n) extern void isr##n(void)`
**0349:** `DECL_ISR(0);DECL_ISR(1);DECL_ISR(2);DECL_ISR(3);DECL_ISR(4);DECL_ISR(5);DECL_ISR(6);DECL_ISR(7);DECL_ISR(8);DECL_ISR(9);DECL_ISR(10);DECL_ISR(11);DECL_ISR(12);DECL_ISR(13);DECL_ISR(14);DECL_ISR(15);DECL_ISR(16);DECL_ISR(17);DECL_ISR(18);DECL_ISR(19);DECL_ISR(20);DECL_ISR(21);DECL_ISR(22);DECL_ISR(23);DECL_ISR(24);DECL_ISR(25);DECL_ISR(26);DECL_ISR(27);DECL_ISR(28);DECL_ISR(29);DECL_ISR(30);DECL_ISR(31);DECL_ISR(32);DECL_ISR(33);DECL_ISR(34);DECL_ISR(35);DECL_ISR(36);DECL_ISR(37);DECL_ISR(38);DECL_ISR(39);DECL_ISR(40);DECL_ISR(41);DECL_ISR(42);DECL_ISR(43);DECL_ISR(44);DECL_ISR(45);DECL_ISR(46);DECL_ISR(47);DECL_ISR(128);`
**0350:** `static void idt_init(void){uint32_t i;void(*v[48])(void)={isr0,isr1,isr2,isr3,isr4,isr5,isr6,isr7,isr8,isr9,isr10,isr11,isr12,isr13,isr14,isr15,isr16,isr17,isr18,isr19,isr20,isr21,isr22,isr23,isr24,isr25,isr26,isr27,isr28,isr29,isr30,isr31,isr32,isr33,isr34,isr35,isr36,isr37,isr38,isr39,isr40,isr41,isr42,isr43,isr44,isr45,isr46,isr47};struct idtr r;for(i=0;i<48;i++)idt_set((uint8_t)i,v[i],0);idt_set(0x80,isr128,1);r.limit=sizeof(idt)-1;r.base=(uint32_t)idt;__asm__ volatile("lidtl %0"::"m"(r));}`
**0351:** ``
**0352:** `static int user_range(uint32_t p,uint32_t n){`
**0353:** `    uint32_t end;`
**0354:** `    if(p>=0x00400000u)return 0;`
**0355:** `    if(n>0x00400000u-p)return 0;`
**0356:** `    end=p+n;`
**0357:** `    if(n==0)return 1;`
**0358:** `    /* User memory must be inside the explicitly mapped user shell regions. */`
**0359:** `    if(p>=(uint32_t)__user_text_start&&end<=(uint32_t)__user_text_end)return 1;`
**0360:** `    if(p>=(uint32_t)__user_rodata_start&&end<=(uint32_t)__user_rodata_end)return 1;`
**0361:** `    if(p>=USER_STACK_PAGE&&end<=USER_STACK_PAGE+0x1000u)return 1;`
**0362:** `    return 0;`
**0363:** `}`
**0364:** `static int user_rw_range(uint32_t p,uint32_t n){`
**0365:** `    uint32_t end;`
**0366:** `    if(n>0x1000u)return 0;`
**0367:** `    if(p<USER_STACK_PAGE||p>=USER_STACK_PAGE+0x1000u)return 0;`
**0368:** `    end=p+n;`
**0369:** `    return end<=USER_STACK_PAGE+0x1000u;`
**0370:** `}`
**0371:** `static int user_cstr(uint32_t p){uint32_t i;if(!user_range(p,1))return 0;for(i=0;i<128u;i++){if(!user_range(p+i,1))return 0;if(((const char*)p)[i]==0)return 1;}return 0;}`
**0372:** `void interrupt_dispatch(struct frame*f){uint32_t n=f->int_no;if(n==32)timer_ticks++;else if(n==33)keyboard_irq();else if(n==46)ata_irq();else if(n<32){`
**0373:** `        /* При аварийном исключении печатаем номер, EIP, CS и error code.`
**0374:** `           Для #PF дополнительно печатаем CR2 — адрес, вызвавший fault. */`
**0375:** `        console_write("EXC ");console_write_u32(n);`
**0376:** `        console_write(" EIP=");console_write_u32(f->eip);`
**0377:** `        console_write(" CS=");console_write_u32(f->cs);`
**0378:** `        console_write(" ERR=");console_write_u32(f->error);`
**0379:** `        if(n==14){uint32_t cr2;__asm__ volatile("movl %%cr2,%0":"=r"(cr2));console_write(" CR2=");console_write_u32(cr2);}`
**0380:** `        console_write("\n");`
**0381:** `        for(;;){__asm__ volatile("cli");cpu_hlt();}}`
**0382:** `    else if(n==0x80){uint32_t a=f->ebx,b=f->ecx,c=f->edx;switch(f->eax){`
**0383:** `        case SYS_CONSOLE_WRITE:if((f->cs&3u)&&!user_range(a,b)){f->eax=0xffffffffu;break;}f->eax=(b&&a)?(console_write_n((const char*)a,b),b):0;break;`
**0384:** `        case SYS_CONSOLE_READ:{uint32_t r=0;if((f->cs&3u)&&!user_rw_range(a,b)){f->eax=0xffffffffu;break;}while(r<b){uint8_t ch=key_pop();if(!ch)break;((uint8_t*)a)[r++]=ch;if(ch=='\n')break;}f->eax=r;break;}`
**0385:** `        case SYS_TIMER_GET:f->eax=timer_ticks;break;`
**0386:** `        case SYS_DISK_READ:if((f->cs&3u)&&(c>8u||!user_rw_range(b,c*SECTOR_SIZE))){f->eax=0xffffffffu;break;}f->eax=disk_read(a,(void*)b,c)?0:0xffffffffu;break;`
**0387:** `        case SYS_DISK_WRITE:if((f->cs&3u)&&(c>8u||!user_range(b,c*SECTOR_SIZE))){f->eax=0xffffffffu;break;}f->eax=disk_write(a,(const void*)b,c)?0:0xffffffffu;break;`
**0388:** `        case SYS_FILE_OPEN:if((f->cs&3u)&&!user_cstr(a)){f->eax=0xffffffffu;break;}f->eax=(uint32_t)fat_open((const char*)a,b);break;`
**0389:** `        case SYS_FILE_READ:{int r;if((f->cs&3u)&&!user_rw_range(b,c)){f->eax=0xffffffffu;break;}r=fat_read_file_fd((int)a,(void*)b,c);f->eax=(r<0)?0xffffffffu:(uint32_t)r;break;}`
**0390:** `        case SYS_FILE_WRITE:{int r;if((f->cs&3u)&&!user_range(b,c)){f->eax=0xffffffffu;break;}r=fat_write_file_fd((int)a,(const void*)b,c);f->eax=(r<0)?0xffffffffu:(uint32_t)r;break;}`
**0391:** `        case SYS_FILE_CLOSE:f->eax=fat_close((int)a)?0:0xffffffffu;break;`
**0392:** `        default:f->eax=0xffffffffu;break;}}`
**0393:** `    if(n>=32&&n<48)irq_eoi(n);}`
**0394:** ``
**0395:** `extern void zero_bss(void);`
**0396:** `extern void user_entry(void);`
**0397:** ``
**0398:** `void kernel_main(void){`
**0399:** `    char b[11];`
**0400:** `    char sample[96];`
**0401:** `    static uint8_t test_out[700];`
**0402:** `    static uint8_t test_in[700];`
**0403:** `    uint32_t mem,got=0,i,wr;`
**0404:** `    int fd;`
**0405:** ``
**0406:** `    zero_bss();`
**0407:** `    mem=*(volatile uint32_t*)0xB8000;`
**0408:** `    console_clear();`
**0409:** `    console_write("Toy OS kernel\nRAM: ");`
**0410:** `    u32_dec(mem,b);console_write(b);console_write(" bytes\n");`
**0411:** `    console_write("IDT/PIC/PIT/KBD/ATA/FAT16 init...\n");`
**0412:** ``
**0413:** `    idt_init();`
**0414:** `    gdt_init_ring3();`
**0415:** `    /* PIC must be remapped before any hardware IRQ can be enabled. */`
**0416:** `    pic_remap();`
**0417:** `    paging_init();`
**0418:** `    pit_init();`
**0419:** `    irq_enable(0);`
**0420:** `    irq_enable(1);`
**0421:** `    /* ATA is a polling driver; keep IRQ14 masked. This prevents an ATA`
**0422:** `       completion interrupt from re-entering the common ISR while a PIO`
**0423:** `       command is still being handled synchronously. */`
**0424:** ``
**0425:** `    if(fat_mount(FAT_LBA))console_write("FAT16 mounted at LBA 42\n");`
**0426:** `    else console_write("FAT16: not mounted\n");`
**0427:** `    /* Keep interrupts disabled while the kernel performs its synchronous FAT/ATA self-test.`
**0428:** `       The shell enables interrupts only after all kernel initialization is complete. */`
**0429:** `    sys_console_write("syscall console: OK\n",20);`
**0430:** ``
**0431:** `    if(fat_read_file("README.TXT",sample,sizeof(sample)-1,&got)){`
**0432:** `        sample[got]=0;`
**0433:** `        console_write("README.TXT: ");`
**0434:** `        console_write(sample);`
**0435:** `        if(got&&sample[got-1]!='\n')console_write("\n");`
**0436:** `    }`
**0437:** ``
**0438:** `    for(i=0;i<sizeof(test_out);i++)test_out[i]=(uint8_t)('A'+(i%26u));`
**0439:** `    fd=(int)sys_file_open("TEST.TXT",FAT16_MODE_READ|FAT16_MODE_WRITE|FAT16_MODE_CREATE|FAT16_MODE_TRUNC);`
**0440:** `    if(fd>=0){`
**0441:** `        wr=sys_file_write((uint32_t)fd,test_out,sizeof(test_out));`
**0442:** `        sys_file_close((uint32_t)fd);`
**0443:** `        console_write("File write 700B: ");`
**0444:** `        if(wr==sizeof(test_out))console_write("OK\n");else console_write("FAIL\n");`
**0445:** ``
**0446:** `        fd=(int)sys_file_open("TEST.TXT",FAT16_MODE_READ);`
**0447:** `        if(fd>=0){`
**0448:** `            uint32_t rd=sys_file_read((uint32_t)fd,test_in,sizeof(test_in));`
**0449:** `            sys_file_close((uint32_t)fd);`
**0450:** `            for(i=0;i<sizeof(test_in)&&test_in[i]==test_out[i];i++){}`
**0451:** `            console_write("File read/verify: ");`
**0452:** `            if(rd==sizeof(test_in)&&i==sizeof(test_in))console_write("OK\n");else console_write("FAIL\n");`
**0453:** `        }`
**0454:** `    }else console_write("File open: FAIL\n");`
**0455:** ``
**0456:** `    /* Only now are timer/keyboard interrupts allowed to preempt the kernel. */`
**0457:** `    __asm__ volatile("sti" ::: "memory");`
**0458:** `    console_write("Timer ticks: ");`
**0459:** `    console_write_u32(sys_timer_get());`
**0460:** `    console_write("\nReady. Entering ring-3 shell.\n");`
**0461:** `    enter_user_shell();`
**0462:** `    for(;;)cpu_hlt();`
**0463:** `}`
**0464:** `__asm__(`
**0465:** `".section .start,\"ax\"\n"`
**0466:** `".global _start\n"`
**0467:** `"_start:\n"`
**0468:** `"jmpl $0x08, $_kernel_main\n"`
**0469:** `);`

## `src/user_shell.c`

**0001:** `/* Ring-3 interactive shell. The shell contains its own tiny INT 80h wrappers,`
**0002:** `   so it never calls a kernel C function directly from CPL3. */`
**0003:** `typedef unsigned char uint8_t;`
**0004:** `typedef unsigned int uint32_t;`
**0005:** `#define SYS_CONSOLE_WRITE 1u`
**0006:** `#define SYS_CONSOLE_READ  2u`
**0007:** `#define SYS_TIMER_GET     3u`
**0008:** `#define SYS_FILE_OPEN     6u`
**0009:** `#define SYS_FILE_READ     7u`
**0010:** `#define SYS_FILE_WRITE    8u`
**0011:** `#define SYS_FILE_CLOSE    9u`
**0012:** `#define FAT16_MODE_READ   0x01u`
**0013:** `#define FAT16_MODE_WRITE  0x02u`
**0014:** `#define FAT16_MODE_CREATE 0x04u`
**0015:** `#define FAT16_MODE_TRUNC  0x08u`
**0016:** `#define FAT16_MODE_APPEND 0x10u`
**0017:** ``
**0018:** `/* Syscall ABI: EAX=number, EBX=arg1, ECX=arg2, EDX=arg3, EAX=return. */`
**0019:** `__attribute__((section(".usertext"))) static uint32_t syscall3(uint32_t n,uint32_t a,uint32_t b,uint32_t c){`
**0020:** `    uint32_t r;`
**0021:** `    __asm__ volatile("int $0x80":"=a"(r):"a"(n),"b"(a),"c"(b),"d"(c):"memory");`
**0022:** `    return r;`
**0023:** `}`
**0024:** `__attribute__((section(".usertext"))) static uint32_t sys_console_write(const char*s,uint32_t n){return syscall3(SYS_CONSOLE_WRITE,(uint32_t)s,n,0);}`
**0025:** `__attribute__((section(".usertext"))) static uint32_t sys_console_read(char*b,uint32_t n){return syscall3(SYS_CONSOLE_READ,(uint32_t)b,n,0);}`
**0026:** `__attribute__((section(".usertext"))) static uint32_t sys_timer_get(void){return syscall3(SYS_TIMER_GET,0,0,0);}`
**0027:** `__attribute__((section(".usertext"))) static uint32_t sys_file_open(const char*name,uint32_t mode){return syscall3(SYS_FILE_OPEN,(uint32_t)name,mode,0);}`
**0028:** `__attribute__((section(".usertext"))) static uint32_t sys_file_read(uint32_t fd,void*b,uint32_t n){return syscall3(SYS_FILE_READ,fd,(uint32_t)b,n);}`
**0029:** `__attribute__((section(".usertext"))) static uint32_t sys_file_write(uint32_t fd,const void*b,uint32_t n){return syscall3(SYS_FILE_WRITE,fd,(uint32_t)b,n);}`
**0030:** `__attribute__((section(".usertext"))) static uint32_t sys_file_close(uint32_t fd){return syscall3(SYS_FILE_CLOSE,fd,0,0);}`
**0031:** ``
**0032:** `__attribute__((section(".usertext"))) static uint32_t sl(const char*s){uint32_t n=0;while(s[n])n++;return n;}`
**0033:** `__attribute__((section(".usertext"))) static void put(const char*s){sys_console_write(s,sl(s));}`
**0034:** `__attribute__((section(".usertext"))) static void putn(const char*s,uint32_t n){sys_console_write(s,n);}`
**0035:** `__attribute__((section(".usertext"))) static void dec(uint32_t v,char*b){static const uint32_t p[10]={1000000000u,100000000u,10000000u,1000000u,100000u,10000u,1000u,100u,10u,1u};uint32_t d,st=0,i;for(i=0;i<10;i++){d=0;while(v>=p[i]){v-=p[i];d++;}if(d||st||i==9){*b++=(char)('0'+d);st=1;}}*b=0;}`
**0036:** `__attribute__((section(".usertext"))) static uint32_t eq(const char*a,const char*b){uint32_t i=0;while(a[i]&&b[i]&&a[i]==b[i])i++;return a[i]==0&&b[i]==0;}`
**0037:** `__attribute__((section(".usertext"))) static uint32_t prefix(const char*a,const char*b){uint32_t i=0;while(b[i]){if(a[i]!=b[i])return 0;i++;}return 1;}`
**0038:** `__attribute__((section(".usertext"))) static void skip(char**p){while(**p==' '||**p=='\t')(*p)++;}`
**0039:** `__attribute__((section(".usertext"))) static uint32_t number(char**p){uint32_t v=0;skip(p);while(**p>='0'&&**p<='9'){v=v*10u+(uint32_t)(*(*p)-'0');(*p)++;}return v;}`
**0040:** `__attribute__((section(".usertext"))) static void trim(char*s){uint32_t n=sl(s);while(n&& (s[n-1]==' '||s[n-1]=='\t'||s[n-1]=='\n'||s[n-1]=='\r'))s[--n]=0;}`
**0041:** `__attribute__((section(".usertext"))) static void readline(char*b,uint32_t cap){uint32_t n=0;while(n+1<cap){char c;uint32_t r=sys_console_read(&c,1);if(r!=1)continue;if(c=='\n'){b[n]=0;put("\n");return;}if(c=='\b'){if(n){n--;put("\b");}continue;}b[n++]=c;putn(&c,1);}b[n]=0;put("\n");}`
**0042:** `__attribute__((section(".usertext"))) static void help(void){put("Commands:\n  help\n  clear\n  echo TEXT\n  ticks\n  cat FILE\n  write FILE TEXT\n  append FILE TEXT\n  read FILE\n  syscall N [argument]\n  exit\n");}`
**0043:** `__attribute__((section(".usertext"))) static void clear(void){uint32_t i;char s[81];for(i=0;i<80;i++)s[i]=' ';s[80]=0;for(i=0;i<25;i++){put(s);put("\n");}}`
**0044:** `__attribute__((section(".usertext"))) static void cat(const char*name){char b[512];uint32_t fd,n;fd=sys_file_open(name,FAT16_MODE_READ);if(fd==0xffffffffu){put("open: FAIL\n");return;}n=sys_file_read(fd,b,511);sys_file_close(fd);if(n==0xffffffffu){put("read: FAIL\n");return;}b[n]=0;put(b);if(!n||b[n-1]!='\n')put("\n");}`
**0045:** `__attribute__((section(".usertext"))) static void write_file(const char*name,const char*text,uint32_t append){uint32_t fd,n;uint32_t mode=FAT16_MODE_WRITE|FAT16_MODE_CREATE|(append?FAT16_MODE_APPEND:FAT16_MODE_TRUNC);fd=sys_file_open(name,mode);if(fd==0xffffffffu){put("open: FAIL\n");return;}n=sys_file_write(fd,text,sl(text));sys_file_close(fd);if(n==sl(text))put("write: OK\n");else put("write: FAIL\n");}`
**0046:** `__attribute__((section(".usertext"))) static void do_syscall(char*p){uint32_t n;char out[12];skip(&p);n=number(&p);skip(&p);if(n==SYS_CONSOLE_WRITE){put("syscall 1 -> ");put(p);put("\n");sys_console_write(p,sl(p));return;}if(n==SYS_TIMER_GET){dec(sys_timer_get(),out);put("syscall 3 -> ");put(out);put(" ticks\n");return;}if(n==SYS_CONSOLE_READ){put("syscall 2: type one line> ");{char x[128];uint32_t r=sys_console_read(x,127);x[r]=0;put("read=\"");put(x);put("\n");}return;}put("syscall: unsupported safe demo number\n");}`
**0047:** `__attribute__((section(".usertext"))) void shell_run(void){char line[128];put("\nToy OS ring-3 shell. Commands use INT 80h.\n");help();for(;;){put("toy0> ");readline(line,sizeof(line));trim(line);if(eq(line,""))continue;if(eq(line,"help")){help();continue;}if(eq(line,"clear")){clear();continue;}if(eq(line,"ticks")){char b[12];dec(sys_timer_get(),b);put(b);put("\n");continue;}if(eq(line,"exit")){put("shell halted.\n");for(;;){} }if(prefix(line,"echo ")){put(line+5);put("\n");continue;}if(prefix(line,"cat ")){cat(line+4);continue;}if(prefix(line,"write ")){char*p=line+6,*name,*text;skip(&p);name=p;while(*p&&*p!=' ')p++;if(!*p){put("usage: write FILE TEXT\n");continue;}*p++=0;text=p;write_file(name,text,0);continue;}if(prefix(line,"append ")){char*p=line+7,*name,*text;skip(&p);name=p;while(*p&&*p!=' ')p++;if(!*p){put("usage: append FILE TEXT\n");continue;}*p++=0;text=p;write_file(name,text,1);continue;}if(prefix(line,"read ")){cat(line+5);continue;}if(prefix(line,"syscall ")){do_syscall(line+8);continue;}put("unknown command; type help\n");}}`

## `liker.ld`

**0001:** `/* PE/COFF linker script for the 32-bit Toy OS.`
**0002:** `   Kernel text/data are supervisor-only; the ring-3 shell is aligned to page`
**0003:** `   boundaries so paging can grant user access only to its code/rodata. */`
**0004:** `SECTIONS`
**0005:** `{`
**0006:** `    . = 0x7e00;`
**0007:** `    .text :`
**0008:** `    {`
**0009:** `        KEEP(*(.start))`
**0010:** `        *(.text*)`
**0011:** `        *(.krodata*)`
**0012:** `        *(.data*)`
**0013:** `    }`
**0014:** ``
**0015:** `    . = ALIGN(0x1000);`
**0016:** `    ___user_text_start = .;`
**0017:** `    .utext :`
**0018:** `    {`
**0019:** `        *(.usertext*)`
**0020:** `    }`
**0021:** `    ___user_text_end = .;`
**0022:** ``
**0023:** `    . = ALIGN(0x1000);`
**0024:** `    ___user_rodata_start = .;`
**0025:** `    .urodata :`
**0026:** `    {`
**0027:** `        *(.rdata*)`
**0028:** `        *(.rodata*)`
**0029:** `    }`
**0030:** `    ___user_rodata_end = .;`
**0031:** ``
**0032:** `    ___bss_start = .;`
**0033:** `    .bss (NOLOAD) :`
**0034:** `    {`
**0035:** `        *(.bss*)`
**0036:** `        *(COMMON)`
**0037:** `    }`
**0038:** `    ___bss_end = .;`
**0039:** `}`

## `build.sh`

**0001:** `#!/bin/sh`
**0002:** `# Строгий shell mode: прекращаем сборку при любой ошибке и при unset variable.`
**0003:** `set -eu`
**0004:** `mkdir -p build`
**0005:** ``
**0006:** `# Требуем именно 32-битный target W64DevKit: kernel ABI = i386.`
**0007:** `case "\`gcc -dumpmachine\`" in`
**0008:** `    i[3-6]86-*|i686-*) ;;`
**0009:** `    *) echo "ERROR: use the x86 W64DevKit (gcc target must be i686)."; exit 1;;`
**0010:** `esac`
**0011:** ``
**0012:** `# --32 = i386 assembler.`
**0013:** `ASFLAGS="--32"`
**0014:** ``
**0015:** `# -Os уменьшает kernel image.`
**0016:** `# -ffreestanding запрещает предполагать hosted OS.`
**0017:** `# -fno-pie нужен для абсолютных адресов raw kernel.`
**0018:** `# -fno-stack-protector убирает зависимость от stack-protector runtime.`
**0019:** `# -fno-asynchronous-unwind-tables и -fno-unwind-tables убирают ненужные metadata.`
**0020:** `# -fno-builtin предотвращает скрытые вызовы libc.`
**0021:** `# -fno-tree-vectorize/-fno-tree-slp-vectorize запрещают автоматическую SIMD-векторизацию.`
**0022:** `# -mno-sse/-mno-sse2/-mno-mmx/-mno-80387 запрещают инструкции FPU/SSE/MMX:`
**0023:** `# в ядре ещё не настроен контекст FPU, поэтому Ring 3 не должен получить`
**0024:** `# скрытый MOVAPS/MOVDQA и не должен зависеть от состояния CR0/CR4.`
**0025:** `# -nostdinc и -nostdlib полностью исключают стандартные headers/CRT/libc.`
**0026:** `CFLAGS="-Os -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-builtin -fno-tree-vectorize -fno-tree-slp-vectorize -mno-sse -mno-sse2 -mno-mmx -mno-80387 -nostdinc -nostdlib"`
**0027:** ``
**0028:** `# 1. BIOS boot sector.`
**0029:** `as $ASFLAGS src/boot.S -o build/boot.o`
**0030:** `# PE/COFF 32-bit mode; image base 0; _start — единственный entry point.`
**0031:** `ld -m i386pe --image-base 0 -e _start -T boot.ld -o build/boot.pe build/boot.o`
**0032:** `# Извлекаем raw 512-byte boot sector.`
**0033:** `objcopy --only-section=.text -O binary build/boot.pe build/boot.bin`
**0034:** `test \`wc -c < build/boot.bin\` -eq 512`
**0035:** ``
**0036:** `# 2. Kernel + отдельный Ring-3 shell object.`
**0037:** `gcc $CFLAGS -c src/kernel.c -o build/kernel.o`
**0038:** `gcc $CFLAGS -c src/user_shell.c -o build/user_shell.o`
**0039:** `as $ASFLAGS src/isr.S -o build/isr.o`
**0040:** ``
**0041:** `# Kernel .rdata переименовывается в .krodata: это supervisor-only read-only data.`
**0042:** `# User .rdata оставляем отдельным: linker помещает его в page-aligned .urodata.`
**0043:** `objcopy --rename-section .rdata=.krodata,alloc,load,readonly,data,contents,code build/kernel.o build/kernel.coff.o`
**0044:** ``
**0045:** `# Линкуем kernel, user text, user rodata и ISR. liker.ld page-aligns user sections.`
**0046:** `ld -m i386pe --image-base 0 -e _start -T liker.ld -o build/kernel.pe build/kernel.coff.o build/user_shell.o build/isr.o`
**0047:** ``
**0048:** `# В raw image нужны .text + .utext + .urodata вместе с промежуточными gap,`
**0049:** `# потому что виртуальные адреса user sections должны остаться правильными.`
**0050:** `objcopy --only-section=.text --only-section=.utext --only-section=.urodata -O binary build/kernel.pe build/kernel.bin`
**0051:** ``
**0052:** `# После добавления paging и page-aligned user areas kernel занимает до 40 секторов.`
**0053:** `test \`wc -c < build/kernel.bin\` -le 20480`
**0054:** ``
**0055:** `# 3. Host FAT16 image tools.`
**0056:** `# -O2 оптимизирует host utilities, -s убирает символы из их EXE.`
**0057:** `gcc tools/mkfat16.c -O2 -s -o build/mkfat16.exe`
**0058:** `gcc tools/fat16check.c -O2 -s -o build/fat16check.exe`
**0059:** `build/mkfat16.exe build/disk.img`
**0060:** ``
**0061:** `# LBA0 = BIOS boot sector.`
**0062:** `dd if=build/boot.bin of=build/disk.img bs=512 count=1 conv=notrunc`
**0063:** `# LBA1..40 = ровно 40-секторный kernel slot.`
**0064:** `dd if=/dev/zero of=build/disk.img bs=512 seek=1 count=40 conv=notrunc`
**0065:** `# Фактический raw kernel накладывается на начало slot.`
**0066:** `dd if=build/kernel.bin of=build/disk.img bs=512 seek=1 conv=notrunc`
**0067:** `cp build/disk.img build/toy_os.img`
**0068:** ``
**0069:** `# 4. Базовые 10 проверок + независимая тройная проверка.`
**0070:** `./check10.sh`
**0071:** `./check3.sh`

## `build.bat`

**0001:** `@echo off`
**0002:** `setlocal`
**0003:** `cd /d %~dp0`
**0004:** `where sh >nul 2>&1`
**0005:** `if errorlevel 1 (`
**0006:** `  echo ERROR: Run this script from the W64DevKit shell.`
**0007:** `  exit /b 1`
**0008:** `)`
**0009:** `sh -lc "./build.sh"`
**0010:** `if errorlevel 1 exit /b 1`
**0011:** `echo Build and triple verification OK.`
**0012:** `endlocal`

## `check3.sh`

**0001:** `#!/bin/sh`
**0002:** `set -eu`
**0003:** `fail(){ echo "FAIL: $1"; exit 1; }`
**0004:** ``
**0005:** `grep -q 'movl $0x200000,%esp' src/boot.S || fail "boot stack must be separate from TSS stack"`
**0006:** `grep -q 'USER_STACK_TOP  0x003fffe0u' src/kernel.c || fail "user stack top must stay inside mapped page"`
**0007:** `grep -q 'KERNEL_STACK_TOP 0x001f0000u' src/kernel.c || fail "TSS kernel stack address missing"`
**0008:** `grep -q 'gdt_set(&gdt\[1\],0,0x3ffff,0x9a,0xc0)' src/kernel.c || fail "kernel code must be exactly 1 GiB"`
**0009:** `grep -q 'gdt_set(&gdt\[2\],0,0x3ffff,0x92,0xc0)' src/kernel.c || fail "kernel data must be exactly 1 GiB"`
**0010:** `grep -q 'gdt_set(&gdt\[3\],0,0x3ffff,0xfa,0xc0)' src/kernel.c || fail "user code must be exactly 1 GiB"`
**0011:** `grep -q 'gdt_set(&gdt\[4\],0,0x3ffff,0xf2,0xc0)' src/kernel.c || fail "user data must be exactly 1 GiB"`
**0012:** `grep -q 'tss.esp0=KERNEL_STACK_TOP' src/kernel.c || fail "TSS ESP0 missing"`
**0013:** `grep -q 'tss.ss0=0x10u' src/kernel.c || fail "TSS SS0 missing"`
**0014:** `grep -q 'pushl \$0x23;pushl \$0x003fffe0;pushfl;pushl \$0x1b;pushl \$_user_entry;iret' src/kernel.c || fail "CPL3 IRET frame missing"`
**0015:** `grep -q 'movw \$0x23,%ax' src/isr.S || fail "user data segment setup missing"`
**0016:** `grep -q 'int \$0x80' src/user_shell.c || fail "user syscall wrapper missing"`
**0017:** `grep -q 'struct frame{uint32_t edi,esi,ebp,oes,ebx,edx,ecx,eax;uint32_t gs,fs,es,ds;uint32_t int_no,error;uint32_t eip,cs,eflags;}' src/kernel.c || fail "ISR frame layout must match push-sequence"`
**0018:** `grep -q 'ISR_ERR   8' src/isr.S || fail "double-fault vector must preserve CPU error code"`
**0019:** `! grep -q 'SYS_CONSOLE_READ.*sti.*hlt' src/kernel.c || fail "blocking STI/HLT inside syscall handler"`
**0020:** `grep -q 'page_directory\[0\]=(uint32_t)page_table0|PAGE_P|PAGE_RW|PAGE_US;' src/kernel.c || fail "PDE[0] must allow user page-table walk"`
**0021:** `grep -q 'outb(PIC1+1,0xff)' src/kernel.c || fail "PIC master must be masked before remap"`
**0022:** `grep -q 'outb(PIC2+1,0xff)' src/kernel.c || fail "PIC slave must be masked before remap"`
**0023:** `grep -q 'outb(PIC1+1,0x20)' src/kernel.c || fail "PIC master must remap IRQ0..7 to INT 32..39"`
**0024:** `grep -q 'outb(PIC2+1,0x28)' src/kernel.c || fail "PIC slave must remap IRQ8..15 to INT 40..47"`
**0025:** `grep -q 'outb(PIC1+1,0x04)' src/kernel.c || fail "PIC cascade ICW3 missing"`
**0026:** `grep -q 'outb(PIC2+1,0x02)' src/kernel.c || fail "PIC slave ICW3 missing"`
**0027:** `grep -q 'outb(PIC1+1,0x01)' src/kernel.c || fail "PIC master ICW4 missing"`
**0028:** `grep -q 'outb(PIC2+1,0x01)' src/kernel.c || fail "PIC slave ICW4 missing"`
**0029:** `grep -q 'outb(PIC1+1,0xfc)' src/kernel.c || fail "PIC final master mask missing"`
**0030:** `grep -q 'outb(PIC2+1,0xff)' src/kernel.c || fail "PIC final slave mask missing"`
**0031:** `grep -q 'if(n==14){uint32_t cr2' src/kernel.c || fail "page-fault CR2 diagnostic missing"`
**0032:** `grep -q 'page_directory\[0\]=(uint32_t)page_table0|PAGE_P|PAGE_RW|PAGE_US;' src/kernel.c || fail "PDE[0] must allow user page-table walk"`
**0033:** `grep -q 'case SYS_DISK_READ:if((f->cs&3u)&&(c>8u' src/kernel.c || fail "user disk-read sector overflow guard missing"`
**0034:** `grep -q 'case SYS_DISK_WRITE:if((f->cs&3u)&&(c>8u' src/kernel.c || fail "user disk-write sector overflow guard missing"`
**0035:** `! grep -R -qE '\b(movaps|movups|movdqa|movdqu|xorps|xorpd|addps|subps|mulps|divps|pxor|pcmp|fld|fst|fadd|fsub|fmul|fdiv)\b' build/kernel.dis 2>/dev/null || fail "SIMD/FPU instruction detected"`
**0036:** `objdump -d build/kernel.pe 2>/dev/null | grep -q 'iret' || fail "IRET missing in kernel binary"`
**0037:** `echo "PASS: Ring-3 frame/GDT/TSS/paging/syscall static checks"`

## `check10.sh`

**0001:** `#!/bin/sh`
**0002:** `set -eu`
**0003:** `fail(){ echo "CHECK FAILED: $1"; exit 1; }`
**0004:** `[ -f build/boot.bin ] || fail boot.bin`
**0005:** `[ -f build/kernel.bin ] || fail kernel.bin`
**0006:** `[ -f build/disk.img ] || fail disk.img`
**0007:** `[ -f build/isr.o ] || fail isr.o`
**0008:** `[ -f build/user_shell.o ] || fail user_shell.o`
**0009:** `[ \`wc -c < build/boot.bin\` -eq 512 ] || fail "boot size"`
**0010:** `[ \`wc -c < build/kernel.bin\` -le 20480 ] || fail "kernel > 40 sectors"`
**0011:** `[ "\`od -An -t x1 -j 510 -N 2 build/boot.bin | tr -d ' \n'\`" = 55aa ] || fail "boot signature"`
**0012:** `[ "\`od -An -t x1 -j 0 -N 1 build/kernel.bin | tr -d ' \n'\`" = ea ] || fail "kernel far jump opcode"`
**0013:** `[ -f liker.ld ] && [ -f src/boot.S ] && [ -f src/isr.S ] && [ -f src/kernel.c ] && [ -f src/user_shell.c ] || fail "required sources"`
**0014:** `[ -z "\`nm -u build/kernel.pe\`" ] || fail "unresolved symbols"`
**0015:** `[ "\`od -An -t x1 -j 510 -N 2 build/disk.img | tr -d ' \n'\`" = 55aa ] || fail "disk boot signature"`
**0016:** `[ "\`dd if=build/disk.img bs=512 skip=42 count=1 2>/dev/null | od -An -t x1 -N 2 | tr -d ' \n'\`" = eb3c ] || fail "FAT16 BPB signature"`
**0017:** `[ "\`dd if=build/disk.img bs=512 skip=1 count=1 2>/dev/null | od -An -t x1 -N 1 | tr -d ' \n'\`" = \`od -An -t x1 -N 1 build/kernel.bin | tr -d ' \n'\` ] || fail "kernel overlay"`
**0018:** `[ \`dd if=build/disk.img bs=512 skip=1 count=40 2>/dev/null | wc -c\` -eq 20480 ] || fail "kernel slot is not 40 sectors"`
**0019:** `echo "10/10 checks passed"`

## V9 — критическое исправление перехода в CPL3

В текущем исходном коде `src/kernel.c` функция `enter_user_shell()` формирует IRET frame и устанавливает `EFLAGS.IF` через `orl $0x200,(%esp)`. Это сделано из Ring 0, поэтому пользовательский код не обязан и не должен выполнять `STI`.

В `src/isr.S` `_user_entry` больше не содержит `STI`, `CLI` или `HLT`. После загрузки `DS/ES/FS/GS` он вызывает `_shell_run`; если shell когда-либо вернётся, выполняется только безопасный бесконечный `JMP`.

Причина исправления: `STI`, `CLI` и `HLT` являются привилегированными инструкциями. При `CPL=3` и `IOPL=0` их выполнение из пользовательского code segment приводит к `#GP(13)`. Поэтому разрешение аппаратных прерываний должно передаваться через IRET frame, а не через `STI` из Ring 3.


## v11 — исправление прокрутки консоли

Исправлена ошибка консоли после Enter на нижних строках экрана. Ранее `console_put()` при достижении строки 25 сбрасывал `vga_y` в 0, поэтому новый вывод начинался с верхней строки и затирал старый текст. В v11 реализована настоящая прокрутка VGA: строки 1..24 сдвигаются на одну строку вверх, последняя строка очищается, а курсор остаётся в строке 24. Отдельная функция `console_newline()` используется для Enter и обычного перевода строки.

Команда `clear` по-прежнему очищает весь экран напрямую через kernel console; изменение прокрутки не меняет остальные требования проекта.

Команда `delete FILE` удаляет файл из корневого каталога FAT16, освобождает его цепочку кластеров и помечает запись каталога свободной.

# Приложение v13 — EXE loader

## `src/kernel.c`

Новые константы `SYS_EXEC` и `SYS_EXIT` определяют syscall ABI для запуска и
завершения отдельной Ring-3 программы. `struct exe_header` соответствует
16-байтовому формату EXE1.

`paging_set_exec_user()` меняет user-бит PTE для области `0x00100000` и
перезагружает CR3, чтобы изменения таблицы страниц стали видимы немедленно.

`exe_load()` открывает файл через FAT16, проверяет header и границы, загружает
raw image, обнуляет BSS, сохраняет shell frame и меняет сохранённые EIP/ESP/SS.

Важно: существующий `struct frame` намеренно не расширен, потому что прежние
проверки проекта завязаны на точную последовательность PUSHA/segment pushes.
При входе в ISR из CPL3 процессор добавляет `user ESP` и `user SS` сразу после
трёх слов `EIP/CS/EFLAGS`; v13 обращается к ним как к словам 17 и 18 полного
стека frame.

`exe_exit()` возвращает сохранённые 19 слов frame и снимает user-доступ с
области EXE.

## `src/hello.c`

Это минимальная standalone программа без libc/CRT. Её `_start` выполняет
переход к `program_main`, программа выводит строку через `SYS_CONSOLE_WRITE`
и завершает себя через `SYS_EXIT`.

## `program.ld`

Все runtime-адреса программы рассчитываются относительно фиксированного
адреса `0x00100000`, куда её затем копирует kernel loader. Поэтому relocation
таблица не нужна.

## `tools/mkexe.c`

Утилита добавляет к raw image 16-байтовый little-endian header EXE1.


## v14 — исправление EXE stack page

Исправлена ошибка page fault при запуске EXE. Начальный ESP программы равен `0x003FF000`; первая инструкция `call`/`push` записывает данные по адресу `0x003FEFFC`, то есть использует страницу `0x003FE000..0x003FEFFF`. В v13 эта страница имела запись PTE, но не имела `PAGE_US`, поэтому Ring 3 получал `#PF` с `ERR=7` (user write to a supervisor page). Теперь `paging_set_exec_user()` при запуске EXE явно устанавливает для `EXEC_STACK_PAGE` флаги `P|RW|US`, а при завершении снимает `US`. Добавлена отдельная регрессионная проверка этого условия.


## v18: исправление SYS_CONSOLE_READ

В исходнике `src/kernel.c` обработчик `SYS_CONSOLE_READ` после проверки пользовательского буфера не возвращает ложный `0` при отсутствии символа. Он ожидает IRQ1 с помощью `sti; hlt; cli`, затем извлекает один символ из клавиатурного буфера и возвращает `1`. Для `n=0` возвращается `0`; при недопустимом пользовательском буфере возвращается `0xffffffff`. В `src/user_shell.c` `readline()` отдельно проверяет `0xffffffff`, поэтому ошибка syscall не используется как размер строки.
