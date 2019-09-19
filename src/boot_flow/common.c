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

#include "hf/boot_flow.h"
#include "hf/dlog.h"
#include "hf/fdt_handler.h"
#include "hf/plat/boot_flow.h"

/**
 * Extract the boot parameters from the FDT and the boot-flow driver.
 */
static bool boot_params_init(struct boot_params *p,
			     const struct fdt_node *fdt_root)
{
	p->mem_ranges_count = 0;
	p->kernel_arg = plat_boot_flow_get_kernel_arg();

	return plat_boot_flow_get_initrd_range(fdt_root, &p->initrd_begin,
					       &p->initrd_end) &&
	       fdt_find_cpus(fdt_root, p->cpu_ids, &p->cpu_count) &&
	       fdt_find_memory_ranges(fdt_root, p);
}

/**
 * Parses information from FDT needed to initialize Hafnium.
 * FDT is mapped at the beginning and unmapped before exiting the function.
 */
bool boot_flow_init(struct mm_stage1_locked stage1_locked,
		    struct manifest *manifest, struct boot_params *boot_params,
		    struct mpool *ppool)
{
	bool ret = false;
	struct fdt_header *fdt;
	struct fdt_node fdt_root;
	enum manifest_return_code manifest_ret;

	/* Get the memory map from the FDT. */
	fdt = fdt_map(stage1_locked, plat_boot_flow_get_fdt_addr(), &fdt_root,
		      ppool);
	if (fdt == NULL) {
		dlog("Unable to map FDT.\n");
		return false;
	}

	if (!fdt_find_child(&fdt_root, "")) {
		dlog("Unable to find FDT root node.\n");
		goto out_unmap_fdt;
	}

	manifest_ret = manifest_init(manifest, &fdt_root);
	if (manifest_ret != MANIFEST_SUCCESS) {
		dlog("Could not parse manifest: %s.\n",
		     manifest_strerror(manifest_ret));
		goto out_unmap_fdt;
	}

	if (!boot_params_init(boot_params, &fdt_root)) {
		dlog("Could not parse boot params.\n");
		goto out_unmap_fdt;
	}

	ret = true;

out_unmap_fdt:
	if (!fdt_unmap(stage1_locked, fdt, ppool)) {
		dlog("Unable to unmap FDT.\n");
		ret = false;
	}

	return ret;
}

/**
 * Takes action on any updates that were generated.
 */
bool boot_flow_update(struct mm_stage1_locked stage1_locked,
		      struct boot_params_update *p, struct mpool *ppool)
{
	return plat_boot_flow_update(stage1_locked, p, ppool);
}
