#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/export.h>
#include <linux/err.h>
#include <linux/sched.h>
#include <linux/security.h>
#include <linux/swap.h>
#include <linux/swapops.h>
#include <linux/mman.h>
#include <linux/hugetlb.h>
#include <linux/vmalloc.h>
#include <linux/userfaultfd_k.h>

#include <asm/sections.h>
#include <asm/uaccess.h>

#define CREATE_TRACE_POINTS
#include <trace/events/kmem.h>

/**
 * kvmalloc_node - attempt to allocate physically contiguous memory, but upon
 * failure, fall back to non-contiguous (vmalloc) allocation.
 * @size: size of the request.
 * @flags: gfp mask for the allocation - must be compatible (superset) with GFP_KERNEL.
 * @node: numa node to allocate from
 *
 * Uses kmalloc to get the memory but if the allocation fails then falls back
 * to the vmalloc allocator. Use kvfree for freeing the memory.
 *
 * Reclaim modifiers - __GFP_NORETRY and __GFP_NOFAIL are not supported. __GFP_REPEAT
 * is supported only for large (>32kB) allocations, and it should be used only if
 * kmalloc is preferable to the vmalloc fallback, due to visible performance drawbacks.
 *
 * Any use of gfp flags outside of GFP_KERNEL should be consulted with mm people.
 */
void *kvmalloc_node(size_t size, gfp_t flags, int node)
{
	gfp_t kmalloc_flags = flags;
	void *ret;

	/*
	 * vmalloc uses GFP_KERNEL for some internal allocations (e.g page tables)
	 * so the given set of flags has to be compatible.
	 */
	WARN_ON_ONCE((flags & GFP_KERNEL) != GFP_KERNEL);

	/*
	 * Make sure that larger requests are not too disruptive - no OOM
	 * killer and no allocation failure warnings as we have a fallback
	 */
	if (size > PAGE_SIZE) {
		kmalloc_flags |= __GFP_NOWARN;

		/*
		 * We have to override __GFP_REPEAT by __GFP_NORETRY for !costly
		 * requests because there is no other way to tell the allocator
		 * that we want to fail rather than retry endlessly.
		 */
		if (!(kmalloc_flags & __GFP_REPEAT) ||
				(size <= PAGE_SIZE << PAGE_ALLOC_COSTLY_ORDER))
			kmalloc_flags |= __GFP_NORETRY;
	}

	ret = kmalloc_node(size, kmalloc_flags, node);

	/*
	 * It doesn't really make sense to fallback to vmalloc for sub page
	 * requests
	 */
	if (ret || size <= PAGE_SIZE)
		return ret;

	if (flags & __GFP_ZERO)
		return vzalloc_node(size, node);
	else
		return vmalloc_node(size, node);

//	return __vmalloc_node_flags(size, node, flags | __GFP_HIGHMEM);
}
EXPORT_SYMBOL(kvmalloc_node);
