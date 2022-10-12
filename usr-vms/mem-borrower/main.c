/*
 * Copyright 2019 The Hafnium Authors.
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/BSD-3-Clause.
 */

#include <stdalign.h>
#include <stdint.h>
#include <stdnoreturn.h>

#include "hf/arch/vm/interrupts.h"

#include "hf/dlog.h"
#include "hf/ffa.h"
#include "hf/memiter.h"
#include "hf/std.h"

#include "vmapi/hf/call.h"
#include "vmapi/hf/transport.h"
#define ENABLE_MM

const uint64_t const_addr = 0x6480000;
#include "sp-mm.h"
#define USR_ASSERT(x)                                        \
	if (!(x)) {                                          \
		dlog("error in: %s %s", __FILE__, __LINE__); \
		for (;;) {                                   \
		}                                            \
	}
#define EXPECT_EQ(x, y) USR_ASSERT((x) == (y))
#define EXPECT_LE(x, y) USR_ASSERT((x) <= (y))

#define EXPECT_FFA_ERROR(value, ffa_error)                 \
	do {                                               \
		struct ffa_value v = (value);              \
		EXPECT_EQ(v.func, FFA_ERROR_32);           \
		EXPECT_EQ(ffa_error_code(v), (ffa_error)); \
	} while (0)

alignas(4096) uint8_t kstack[4096];

static alignas(HF_MAILBOX_SIZE) uint8_t send[HF_MAILBOX_SIZE];
static alignas(HF_MAILBOX_SIZE) uint8_t recv[HF_MAILBOX_SIZE];

static uint8_t mem_des_buf[4096];

static hf_ipaddr_t send_addr = (hf_ipaddr_t)send;
static hf_ipaddr_t recv_addr = (hf_ipaddr_t)recv;
static void log_ret(struct ffa_value ret)
{
	dlog("recv func:%x, error code:%d\n", ret.func, ret.arg2);
}
static void read_at_addr(uint64_t addr)
{
	dlog("read at addr: %x\n", addr);
	dlog("value: %x\n", *(uint64_t *)addr);
}
#ifdef ENABLE_MM

static void print_pgtable(uint64_t pte, uint64_t va, int level,
			  uint64_t target_va)
{
	// dlog_verbose("[START PRINT LEVEL %d]\n", level);
	// dlog_verbose("pte addr:%x\n", pte);
	uint64_t i, j;
	uint64_t *table = (void *)pte;
	for (i = 0; i < 512; i++) {
		if (table[i]) {
			uint64_t addr = (table[i] >> 12) << 12;
			uint64_t nva = va | (i << ((4 - level) * 9 + 3));
			if (nva >= (1ull << 36)) {
				break;
			}
			if (nva == target_va) {
				for (j = 0; j < level; j++)
					dlog("\t");
				dlog("pte:%x, attrs:%x, ipa:%x, level:%d\n",
				     addr, (table[i] & ((1 << 12) - 1)), nva,
				     level);
			}
			if (!(addr >> 40)) {
				print_pgtable(addr, nva, level + 1, target_va);
			}
		}
	}
}
#endif
static void log_shared_page(struct ffa_memory_region *region)
{
	dlog("start log pages\n");
	struct ffa_composite_memory_region *com_mems =
		ffa_memory_region_get_composite(region, 0);
	int i;
	dlog("count:%d\n", com_mems->constituent_count);
	for (i = 0; i < com_mems->constituent_count; i++) {
		uint64_t addr = com_mems->constituents[i].address;
#ifdef ENABLE_MM
		// print_pgtable(get_pgtable_root(), 0, 1, addr);
		/*if (!(addr = (uint64_t)mp_ipa(
			      pa_init(addr),
			      pa_init(addr +
				      com_mems->constituents[i].page_count *
					      PAGE_SIZE),
			      false))) {
			dlog("Fail to map ipa\n");
			continue;
		}*/
		(void)print_pgtable;
		print_pgtable(get_pgtable_root(), 0, 1, addr);
#endif
		read_at_addr(addr);
	}
}

static void handle_mem_share(uint64_t handle)
{
	struct ffa_value ret;
	uint32_t des_siz = ffa_memory_retrieve_request_init_single_receiver(
		(struct ffa_memory_region *)send, handle, 0, hf_vm_get_id(), 0,
		0, FFA_DATA_ACCESS_RW, FFA_INSTRUCTION_ACCESS_NOT_SPECIFIED,
		FFA_MEMORY_NORMAL_MEM, FFA_MEMORY_CACHE_WRITE_BACK,
		FFA_MEMORY_INNER_SHAREABLE);
	ret = ffa_mem_retrieve_req(des_siz, des_siz);
	if (ret.func == FFA_MEM_RETRIEVE_RESP_32) {
		des_siz = ret.arg2;
		dlog("des size:%d\n", des_siz);
		memcpy_s(mem_des_buf, 4096, recv, des_siz);
		log_shared_page((struct ffa_memory_region *)mem_des_buf);
		log_ret(ffa_rx_release());
	} else {
		log_ret(ret);
	}

	return;
}

static void register_direct_resp()
{
	// dlog("start waiting\n");
	struct ffa_value ret = ffa_msg_wait();
	for (;;) {
		if (ret.func == FFA_MSG_SEND_DIRECT_REQ_32) {
			ffa_vm_id_t sender = ffa_sender(ret);
			ffa_vm_id_t recver = ffa_receiver(ret);
			dlog("recv from: %u, value: %d\n", sender, ret.arg3);
			handle_mem_share(ret.arg3);
			ret = ffa_msg_send_direct_resp(recver, sender, 0, 0, 0,
						       0, 0);

		} else {
			log_ret(ret);
			ret = ffa_msg_wait();
		}
	}
}

noreturn void kmain(void)
{
	ffa_rxtx_map(send_addr, recv_addr);
	exception_setup(NULL, NULL);

#ifdef ENABLE_MM
	// log_ret(ffa_rx_release());
	// dlog("kstack addr before mm init:%x\n", kstack);
	if (!sp_mm_init()) {
		dlog_error("fall to init page table, spinning...");
		for (;;)
			;
	}
	// dlog("kstack addr after mm init:%x\n", kstack);
	dlog("mm inited\n");
#endif

	read_at_addr(const_addr);
	read_at_addr(const_addr + 0x1000);
	register_direct_resp();

	for (;;)
		;
	/* Do not expect to get to this point, so abort. */
	// abort();
}
