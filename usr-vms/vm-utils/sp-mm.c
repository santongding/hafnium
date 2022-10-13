#include "sp-mm.h"

#include "hf/dlog.h"

/* Number of pages reserved for page tables. Increase if necessary. */
#define PTABLE_PAGES 5
#define MM_MODE_NS UINT32_C(0x0080)
/**
 * Start address space mapping at 0x1000 for the mm to create a L2 table to
 * which the first L1 descriptor points to.
 * Provided SPMC and SP images reside below 1GB, same as peripherals, this
 * prevents a case in which the mm library has to break down the first
 * L1 block descriptor, while currently executing from a region within
 * the same L1 descriptor. This is not architecturally possible.
 */
#define STAGE1_START_ADDRESS (0x1000)

alignas(alignof(struct mm_page_table)) static char ptable_buf
	[sizeof(struct mm_page_table) * PTABLE_PAGES];

static struct mpool ppool;
static struct mm_ptable ptable;

struct mm_stage1_locked sp_mm_get_stage1(void)
{
	return (struct mm_stage1_locked){.ptable = &ptable};
}

bool sp_mm_init(void)
{
	struct mm_stage1_locked stage1_locked;

	/* Call arch init before calling below mapping routines */
	if (!arch_vm_mm_init()) {
		return false;
	}

	dlog("mpool size is %x\n", sizeof(ptable_buf));
	mpool_init(&ppool, sizeof(struct mm_page_table));
	if (!mpool_add_chunk(&ppool, ptable_buf, sizeof(ptable_buf))) {
		dlog_error("Failed to add buffer to page-table pool.");
		return false;
	}

	if (!mm_ptable_init(&ptable, 0, MM_FLAG_STAGE1, &ppool)) {
		dlog_error("Unable to allocate memory for page table.");
		return false;
	}
	dlog("start:%x end:%x\n", STAGE1_START_ADDRESS,
	     mm_ptable_addr_space_end(MM_FLAG_STAGE1));
	stage1_locked = sp_mm_get_stage1();
	dlog("success id map:%x\n",
	     mm_identity_map(stage1_locked,
			     pa_init((uintptr_t)STAGE1_START_ADDRESS),
			     pa_init(mm_ptable_addr_space_end(MM_FLAG_STAGE1)),
			     MM_MODE_R | MM_MODE_W | MM_MODE_X, &ppool));

	dlog("about to enable mm\n");
	arch_vm_mm_enable(ptable.root);

	return true;
}

void* mp_ipa(paddr_t begin, paddr_t end, bool is_NS)
{
	dlog("mp ipa:%x %x is_ns:%d\n", begin.pa, end.pa, is_NS);
	return mm_identity_map(sp_mm_get_stage1(), begin, end,
			       MM_MODE_R | MM_MODE_W | (is_NS ? MM_MODE_NS : 0),
			       &ppool);
}

uint64_t get_pgtable_root()
{
	return sp_mm_get_stage1().ptable->root.pa;
}

void flush_tlb(void)
{
	arch_mm_sync_table_writes();
}

bool map_va_at_pa(uint64_t va, uint64_t pa, bool is_ns)
{
	uint32_t mode = MM_MODE_R | MM_MODE_W | (is_ns ? MM_MODE_NS : 0);
	return mm_map_va_at_pa(sp_mm_get_stage1(), va_init(va), pa_init(pa),
			       mode, &ppool);
}
void print_table(uint64_t bg, uint64_t ed)
{
	mm_print_ptable(sp_mm_get_stage1().ptable, va_init(bg), va_init(ed), MM_FLAG_STAGE1);
}
