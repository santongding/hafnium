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

static uint64_t read_el(){


	uint64_t __val;                        
    __asm__ volatile("mrs %0, currentEL" : "=r" (__val));    
    return (__val>>2 )&3;                            \
}
static uint64_t read_NS(){
	uint64_t __val;
	__val = (uint64_t)& __val;                        
    // __asm__ volatile("mrs %0, SP" : "=r" (__val));    
    return ((__val)>>63)&1;    
}

static void log_ret(struct ffa_value ret)
{
	dlog("recv func:%x, error code:%d\n", ret.func, ret.arg2);
}
static void register_direct_resp()
{
	dlog("start waiting\n");
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

noreturn void kmain(size_t memory_size)
{
	dlog_enable_lock();
	ffa_rxtx_map(send_addr, recv_addr);
	dlog("%d %d\n",read_el(),read_NS());
	register_direct_resp();
	for (;;);


	EXPECT_FFA_ERROR(ffa_rx_release(), FFA_DENIED);
	for (;;) {
		struct ffa_value ret;

		/* Receive the packet. */
		ret = ffa_msg_wait();
		/*while(1){
			int t = 10000000;
			while(t--);
			dlog(".");
		}*/

		EXPECT_EQ(ret.func, FFA_MSG_SEND_32);
		EXPECT_LE(ffa_msg_send_size(ret), FFA_MSG_PAYLOAD_MAX);

		/* Echo the message back to the sender. */
		memcpy_s(send, FFA_MSG_PAYLOAD_MAX, recv,
			 ffa_msg_send_size(ret));

		/* Swap the socket's source and destination ports */
		struct hf_msg_hdr *hdr = (struct hf_msg_hdr *)send;

		dlog("recv: %s\n", send + sizeof(struct hf_msg_hdr));
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
