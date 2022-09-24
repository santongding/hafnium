/*
 * Copyright 2019 The Hafnium Authors.
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/BSD-3-Clause.
 */

#include <stdalign.h>
#include <stdint.h>

#include "hf/ffa.h"
#include "hf/memiter.h"
#include "hf/std.h"
#include "hf/dlog.h"

#include "vmapi/hf/call.h"
#include "vmapi/hf/transport.h"
#include <stdnoreturn.h>


alignas(4096) uint8_t kstack[4096];

noreturn void kmain(size_t memory_size)
{   
    dlog("vm entering...\n");

    for(;;){

    }
		
}
