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
/*
static alignas(HF_MAILBOX_SIZE) uint8_t send[HF_MAILBOX_SIZE];
static alignas(HF_MAILBOX_SIZE) uint8_t recv[HF_MAILBOX_SIZE];

static hf_ipaddr_t send_addr = (hf_ipaddr_t)send;
static hf_ipaddr_t recv_addr = (hf_ipaddr_t)recv;
*/


static void log_ret(struct ffa_value ret)
{
	dlog("recv func:%x, error code:%d\n", ret.func, ret.arg2);
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
			ret = ffa_msg_send_direct_resp(recver, sender, 0, 0, 0,
						       0, 0);
			log_ret(ret);
		} else {
			log_ret(ret);
			return;
		}
	}
}
void test_main_sp(bool is_boot_vcpu){
	for(;;){
		// dlog("is boot vcpu:%d\n",is_boot_vcpu);
		register_direct_resp();
	}
}

noreturn void kmain(void)
{
	extern void secondary_ep_entry(void);
	struct ffa_value res;
	dlog_info("start from begin\n");
	/*
	 * Initialize the stage-1 MMU and identity-map the entire address space.
	 */
/*
	if (!hftest_mm_init()) {
		HFTEST_LOG_FAILURE();
		HFTEST_LOG(HFTEST_LOG_INDENT "Memory initialization failed");
		abort();
	}*/

	/* Register entry point for secondary vCPUs. */
	res = ffa_secondary_ep_register((uintptr_t)secondary_ep_entry);
	log_ret(res);

	/* Register RX/TX buffers via FFA_RXTX_MAP */
	// set_up_mailbox();

	test_main_sp(true);

	for(;;);
	/* Do not expect to get to this point, so abort. */
	// abort();
}
