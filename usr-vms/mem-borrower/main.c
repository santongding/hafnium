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

const uint64_t const_addr = 0x6680000;
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
		print_table(0x1000,0x1001);
		print_table(addr, addr + 1);
		if (map_va_at_pa(addr, addr, true)) {
			print_table(addr, addr + 1);
			// read_at_addr(const_addr | (((1ull << 16) - 1) <<
			// 48));
			read_at_addr(const_addr);
			read_at_addr(addr);
		} else {
			dlog_error("unable to map va:%x at pa:%x\n", addr,
				   const_addr);
		}
#endif
		read_at_addr(const_addr);
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

	// print_table(0,1ull<<63);

	read_at_addr(const_addr);
	read_at_addr(const_addr + 0x1000);
	register_direct_resp();

	for (;;)
		;
	/* Do not expect to get to this point, so abort. */
	// abort();
}
