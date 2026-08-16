#include <stdint.h>
//finally paging.c is used woo
__attribute__((aligned(4096)))
static uint64_t extra_pdp[4][512];
__attribute__((aligned(4096)))
static uint64_t extra_pd[4][512];
static unsigned npdp = 0, npd = 0;

static uint64_t *pml4(void) {
    uint64_t cr3;
    __asm__ volatile("mov %%cr3, %0" : "=r"(cr3));
    return (uint64_t *)cr3; //cr3 in this case is physical of pml4 aka identity mapped
}

static uint64_t *alloc_table(uint64_t *pool, unsigned *used) {
    uint64_t *t = &pool[(*used) * 512];
    (*used)++;
    for (int i = 0; i < 512; i++) t[i] = 0;
    return t;
}
void map_page_2m(uint64_t phys) {
    uint64_t *p4 = pml4();
    uint64_t idx4 = (phys >> 39) & 0x1FF; //GOD I FUCKING HATE HEXBINARYYYYY
    uint64_t idx3 = (phys >> 30) & 0x1FF; //got i need less hexbinary and more nonbinary
    uint64_t idx2 = (phys >> 21) & 0x1FF;
    uint64_t *p3 = (p4[idx4] & 1) ? (uint64_t *)(p4[idx4] & 0x000FFFFFFFFFF000ULL) // ngl i used ai for this aint no way i would know that part
                                  : alloc_table((uint64_t *)extra_pdp, &npdp);
    p4[idx4] = (uint64_t)p3 | 0x3;
    uint64_t *p2 = (p3[idx3] & 1) ? (uint64_t *)(p3[idx3] & 0x000FFFFFFFFFF000ULL)
                                  : alloc_table((uint64_t *)extra_pd, &npd);
    p3[idx3] = (uint64_t)p2 | 0x3; //baby 0x3 :)
    p2[idx2] = (phys & 0x000FFFFFFFE00000ULL) | 0x83; //by ai this will work (it fucking wont) yes i use a bit of ai but i have barely used ai so far
    __asm__ volatile("invlpg (%0)" :: "r"(phys));
}