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
static alignas(HF_MAILBOX_SIZE) uint8_t send[HF_MAILBOX_SIZE];
static alignas(HF_MAILBOX_SIZE) uint8_t recv[HF_MAILBOX_SIZE];

static hf_ipaddr_t send_addr = (hf_ipaddr_t)send;
static hf_ipaddr_t recv_addr = (hf_ipaddr_t)recv;

static void swap(uint64_t *a, uint64_t *b)
{
	uint64_t t = *a;
	*a = *b;
	*b = t;
}
noreturn void kmain(size_t memory_size)
{
	ffa_rxtx_map(send_addr, recv_addr);
	EXPECT_FFA_ERROR(ffa_rx_release(), FFA_DENIED);
	for (;;) {
		struct ffa_value ret;

		/* Receive the packet. */
		ret = ffa_msg_wait();
		EXPECT_EQ(ret.func, FFA_MSG_SEND_32);
		EXPECT_LE(ffa_msg_send_size(ret), FFA_MSG_PAYLOAD_MAX);

		/* Echo the message back to the sender. */
		memcpy_s(send, FFA_MSG_PAYLOAD_MAX, recv,
			 ffa_msg_send_size(ret));
		

		/* Swap the socket's source and destination ports */
		struct hf_msg_hdr *hdr = (struct hf_msg_hdr *)send;

		dlog("recv: %s\n",send + sizeof(struct hf_msg_hdr));
		swap(&(hdr->src_port), &(hdr->dst_port));

		/* Swap the destination and source ids. */
		ffa_vm_id_t dst_id = ffa_sender(ret);
		ffa_vm_id_t src_id = ffa_receiver(ret);

		EXPECT_EQ(ffa_rx_release().func, FFA_SUCCESS_32);
		EXPECT_EQ(
			ffa_msg_send(src_id, dst_id, ffa_msg_send_size(ret), 0)
				.func,
			FFA_SUCCESS_32);
	}
}
