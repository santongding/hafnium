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
alignas(4096) uint8_t kstack[8][4096];/*
static  void log_ret(struct ffa_value ret)
{
	dlog("recv func:%x, error code:%d\n", ret.func, ret.arg2);
}*/
/*
static uint64_t read_mpidr_el1(void)
{
	uint64_t __val;
	__asm__ volatile("mrs %0, mpidr_el1" : "=r"(__val));
	return __val;
}*/

void test_main_sp(bool is_boot_vcpu){

	for(;;){
		int cnt = 100000;
		while(cnt--){
			__asm__ volatile("nop");

		}
		dlog_info("[dead-loop]finish a loop\n");

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
	//log_ret(res);
	res = ffa_msg_wait();
	//log_ret(res);
	// log_ret(res);
	// dlog_info("mpidr reg:%x",read_mpidr_el1());
	test_main_sp(true);
	for(;;);

}
