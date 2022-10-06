/*
 * Copyright 2019 The Hafnium Authors.
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/BSD-3-Clause.
 */

#include "hf/call.h"
#include "hf/ffa.h"
#include "hf/types.h"

int64_t hf_call(uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3)
{
	register uint64_t r0 __asm__("x0") = arg0;
	register uint64_t r1 __asm__("x1") = arg1;
	register uint64_t r2 __asm__("x2") = arg2;
	register uint64_t r3 __asm__("x3") = arg3;

	__asm__ volatile(
		"smc #0"
		: /* Output registers, also used as inputs ('+' constraint). */
		"+r"(r0), "+r"(r1), "+r"(r2), "+r"(r3)
		:
		: /* Clobber registers. */
		"x4", "x5", "x6", "x7");

	return r0;
}
static struct ffa_value smc_internal(uint32_t func, uint64_t arg0,
				     uint64_t arg1, uint64_t arg2,
				     uint64_t arg3, uint64_t arg4,
				     uint64_t arg5, uint64_t arg6)
{
	register uint64_t r0 __asm__("x0") = func;
	register uint64_t r1 __asm__("x1") = arg0;
	register uint64_t r2 __asm__("x2") = arg1;
	register uint64_t r3 __asm__("x3") = arg2;
	register uint64_t r4 __asm__("x4") = arg3;
	register uint64_t r5 __asm__("x5") = arg4;
	register uint64_t r6 __asm__("x6") = arg5;
	register uint64_t r7 __asm__("x7") = arg6;

	__asm__ volatile(
		"smc #0"
		: /* Output registers, also used as inputs ('+' constraint). */
		"+r"(r0), "+r"(r1), "+r"(r2), "+r"(r3), "+r"(r4), "+r"(r5),
		"+r"(r6), "+r"(r7));

	return (struct ffa_value){.func = r0,
				  .arg1 = r1,
				  .arg2 = r2,
				  .arg3 = r3,
				  .arg4 = r4,
				  .arg5 = r5,
				  .arg6 = r6,
				  .arg7 = r7};
}

struct ffa_value ffa_call(struct ffa_value args)
{
	return smc_internal(args.func, args.arg1, args.arg2, args.arg3,
			    args.arg4, args.arg5, args.arg6, args.arg7);
}
