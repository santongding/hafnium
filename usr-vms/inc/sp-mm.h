#pragma once

#include <stdbool.h>
#include "hf/arch/vm/mm.h"
bool sp_mm_init(void);

void* mp_ipa(paddr_t begin, paddr_t end, bool is_NS);

uint64_t get_pgtable_root();

void flush_tlb(void);
// bool map_ipa(ipaddr_t begin, ipaddr_t end, paddr_t address);
