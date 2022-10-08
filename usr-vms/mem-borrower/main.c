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

#include "hf/dlog.h"
#include "hf/ffa.h"
#include "hf/memiter.h"
#include "hf/std.h"

#include "vmapi/hf/call.h"
#include "vmapi/hf/transport.h"

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

// static uint8_t mem_des_buf[4096];

static hf_ipaddr_t send_addr = (hf_ipaddr_t)send;
static hf_ipaddr_t recv_addr = (hf_ipaddr_t)recv;
static void log_ret(struct ffa_value ret)
{
	dlog("recv func:%x, error code:%d\n", ret.func, ret.arg2);
}
static void handle_mem_share(uint32_t siz)
{
	memcpy_s(send, 4096, recv, siz);
	log_ret(ffa_rx_release());
	struct ffa_memory_region* region =
		(struct ffa_memory_region*)send;
	dlog("siz:%d, count:%d\n", siz, region->receiver_count);
	log_ret(ffa_mem_retrieve_req(48, 48));
	return;
}

static void register_direct_resp()
{
	// dlog("start waiting\n");
	struct ffa_value ret = ffa_msg_wait();
	for (;;) {
		if (ret.func == FFA_MSG_SEND_32) {
			ffa_vm_id_t sender = ffa_sender(ret);
			// ffa_vm_id_t recver = ffa_receiver(ret);
			dlog("recv from: %u, value: %d\n", sender, ret.arg3);
			handle_mem_share(ffa_msg_send_size(ret));
			ret = ffa_msg_wait();
			// ffa_msg_send_direct_resp(recver, sender, 0, 0, 0, 0,
			// 0);

		} else {
			log_ret(ret);
			ret = ffa_msg_wait();
		}
	}
}

noreturn void kmain(void)
{
	ffa_rxtx_map(send_addr, recv_addr);
	log_ret(ffa_rx_release());
	dlog("kstack addr before mm init:%x\n", kstack);
	if (!sp_mm_init()) {
		dlog_error("fall to init page table, spinning...");
		for (;;)
			;
	}
	dlog("kstack addr after mm init:%x\n", kstack);
	dlog("mm inited\n");
	register_direct_resp();

	for (;;)
		;
	/* Do not expect to get to this point, so abort. */
	// abort();
}
