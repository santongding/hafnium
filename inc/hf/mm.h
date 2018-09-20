#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "hf/arch/mm.h"

#include "hf/addr.h"

struct mm_ptable {
	paddr_t table;
};

#define PAGE_SIZE (1 << PAGE_BITS)

/* The following are arch-independent page mapping modes. */
#define MM_MODE_R 0x01 /* read */
#define MM_MODE_W 0x02 /* write */
#define MM_MODE_X 0x04 /* execute */
#define MM_MODE_D 0x08 /* device */

/*
 * This flag indicates that memory allocation must not use locks. This is
 * relevant in systems where interlocked operations are only available after
 * virtual memory is enabled.
 */
#define MM_MODE_NOSYNC 0x10

/*
 * This flag indicates that the mapping is intended to be used in a first
 * stage translation table, which might have different encodings for the
 * attribute bits than the second stage table.
 */
#define MM_MODE_STAGE1 0x20

/*
 * This flag indicates that no TLB invalidations should be issued for the
 * changes in the page table.
 */
#define MM_MODE_NOINVALIDATE 0x40

bool mm_ptable_init(struct mm_ptable *t, int mode);
void mm_ptable_dump(struct mm_ptable *t, int mode);
void mm_ptable_defrag(struct mm_ptable *t, int mode);
bool mm_ptable_unmap_hypervisor(struct mm_ptable *t, int mode);

bool mm_vm_identity_map(struct mm_ptable *t, paddr_t begin, paddr_t end,
			int mode, ipaddr_t *ipa);
bool mm_vm_identity_map_page(struct mm_ptable *t, paddr_t begin, int mode,
			     ipaddr_t *ipa);
bool mm_vm_unmap(struct mm_ptable *t, paddr_t begin, paddr_t end, int mode);
bool mm_vm_is_mapped(struct mm_ptable *t, ipaddr_t ipa, int mode);
bool mm_vm_translate(struct mm_ptable *t, ipaddr_t ipa, paddr_t *pa);

bool mm_init(void);
bool mm_cpu_init(void);
void *mm_identity_map(paddr_t begin, paddr_t end, int mode);
bool mm_unmap(paddr_t begin, paddr_t end, int mode);
void mm_defrag(void);
