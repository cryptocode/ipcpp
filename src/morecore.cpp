#include "shmem.hpp"
#include <dlmalloc.h>
#include <iostream>

// Simulate shared memory
#define ALIGN 4096
static void * aligned_malloc (size_t size)
{
	uint8_t * mem = (uint8_t *)malloc (size + ALIGN + sizeof (void *));
	void ** ptr = (void **)((uintptr_t) (mem + ALIGN + sizeof (void *)) & ~(ALIGN - 1));
	ptr[-1] = mem;
	return ptr;
}

// TODO: make sure diff can be negative. With shared mem, it's just going to bump offset up and down.
// Alternatively, set MORECORE_CANNOT_TRIM=1
extern "C" void * shm_morecore (ptrdiff_t diff)
{
#ifdef NANOCATE_DEBUG
	std::cout << "ptrdiff_t size: " << sizeof (ptrdiff_t) << std::endl;
#endif
	// Replace with shared mem allocation
	const auto max = (4096 * 1024);
	static uint8_t * mem = 0;
	if (mem == 0)
	{
		// TODO: multiple, must get an identifer passed to shm_morecore
		mem = (uint8_t *)nanocate_allocate ("NANOCATE", max);
		//auto mem2 = (uint8_t *)aligned_malloc (max);
		//std::cout << "mem/mem2: " << (void *)mem << "/" << (void *)mem2 << std::endl;
	}

	static ptrdiff_t off = 0;

#ifdef NANOCATE_DEBUG
	std::cout << "Asking for more core memory: " << diff << ", offset: " << off << std::endl;
#endif

	void * offset = &mem[off];
	off += diff;

	//  (~(size_t)0) signals an error to dlmalloc
	void * res = off >= max ? (void *)(~(size_t)0) : offset;
#ifdef NANOCATE_DEBUG
	std::cout << "Previous break: " << res << std::endl;
#endif
	return res;
}