/*
 * Copyright 2019 The Hafnium Authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "vmapi/hf/call.h"

#include "../msr.h"
#include "test/hftest.h"

#define TRY_READ(REG) dlog(#REG "=%#x\n", read_msr(REG))

#define CHECK_READ(REG, VALUE)       \
	do {                         \
		uintreg_t x;         \
		x = read_msr(REG);   \
		EXPECT_EQ(x, VALUE); \
	} while (0)

/*
 * Checks that the register can be updated. The first value is written and read
 * back and then the second value is written and read back. The values must be
 * different so that success means the register value has been changed and
 * updated as expected without relying on the initial value of the register.
 */
#define CHECK_UPDATE(REG, FROM, TO)   \
	do {                          \
		uintreg_t x;          \
		EXPECT_NE(FROM, TO);  \
		write_msr(REG, FROM); \
		x = read_msr(REG);    \
		EXPECT_EQ(x, FROM);   \
		write_msr(REG, TO);   \
		x = read_msr(REG);    \
		EXPECT_EQ(x, TO);     \
	} while (0)
