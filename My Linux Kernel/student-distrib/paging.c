#include "types.h"
#include "x86_desc.h"
#include "lib.h"
#include "paging.h"

/* 
 *  paging_init
 *   DESCRIPTION:   Initialises one Page Dir with one Page Table 
 *                  and one 4MB kernel page entry. Also initialises the
 *                  CR0, CR3, CR4 registers to support paging.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS:  Sets paging on, initialises tables, and loads PD into CR3 
 */

void paging_init()
{
    int i;
    //initialise the PD and PT
    for (i = 0; i < PAGING_LENGTH; i++)
    {
        page_dir[i] = 0;
        page_tab[i] = 0;
        user_page_tab[i] = 0;
    }

    //set up a page table as PDE
    page_dir[0] = (uint32_t)page_tab | PDE_PT;

    //set up a 4MB page as PDE
    page_dir[1] = (MB_4 | PDE_4MB_PAGE);

    //set up video memory PTE
    page_tab[VID_MEM_ADDR >> EXTRA_BITS] = (VID_MEM_ADDR | PTE_VID_MEM);

    //set up video memory storage for terminals
    page_tab[(VID_MEM_ADDR + KB_4) >> EXTRA_BITS] = ((VID_MEM_ADDR + KB_4) | 0x7);
    page_tab[(VID_MEM_ADDR + KB_4 * 2) >> EXTRA_BITS] = ((VID_MEM_ADDR + KB_4 * 2) | 0x7);
    page_tab[(VID_MEM_ADDR + KB_4 * 3) >> EXTRA_BITS] = ((VID_MEM_ADDR + KB_4 * 3) | 0x7);

    //First enable paging
    //Then load page directory address into CR3
    //Next set the paging (PG) and protection (PE) bits of CR0
    //enable 4MB pages
    asm volatile("\n\
        movl $page_dir, %%eax     \n\
        movl %%eax, %%cr3         \n\
        movl %%cr4, %%eax         \n\
        orl $0x10, %%eax          \n\
        movl %%eax, %%cr4         \n\
        movl %%cr0, %%eax         \n\
        orl $0x80000001, %%eax    \n\
        movl %%eax, %%cr0"
                 :
                 : "r"(&page_dir)
                 : "eax", "memory", "cc");
}

void flush_tlb()
{
    asm volatile("\n\
        movl %%cr3, %%eax   \n\
        movl %%eax, %%cr3"
                 :
                 :
                 : "eax", "memory", "cc");
}

// Re map old to working copy
// Map new to vid mem
void update_vid_mem(int term_num, int prev_num)
{
    page_tab[(VID_MEM_ADDR + (KB_4 * (prev_num + 1))) >> EXTRA_BITS] = ((VID_MEM_ADDR + (KB_4 * (prev_num + 1))) | 0x7);
    page_tab[(VID_MEM_ADDR + (KB_4 * (term_num + 1))) >> EXTRA_BITS] = (VID_MEM_ADDR | 0x7);
    flush_tlb();
}
