#ifndef paging
#define paging

#define PAGING_LENGTH 1024 //entries in every PD and PT

#define MB_4 0x400000        //bytes in 4MB
#define KB_4 4096            //bytes in 4KB
#define MB_256 0x10000000    //bytes in 256MB
#define VID_MEM_ADDR 0xB8000 //video memory address
#define EXTRA_BITS 12

//bitmasks for working with page entries
#define EXTRA_BITS_MASK 0xFFFFF000 //zero the extra bits
#define PDE_4MB_PAGE 0x83          //extra bits for PDE 4MB kernel page
#define PDE_PT 0x7                 //extra bits for PDE PT 0b111
#define US_RW_P 0x7
#define PTE_VID_MEM 0x3 //extra bits for PTE video memory

//main function to initialise paging
uint32_t page_dir[PAGING_LENGTH] __attribute__((aligned(KB_4)));
uint32_t page_tab[PAGING_LENGTH] __attribute__((aligned(KB_4)));
uint32_t user_page_tab[PAGING_LENGTH] __attribute__((aligned(KB_4)));

void paging_init();
void update_vid_mem(int term_num, int prev_num);
void flush_tlb();

#endif
