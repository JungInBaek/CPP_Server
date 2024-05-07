#include "pch.h"
#include "Allocator.h"


/*-------------------------
		BaseAllocator
-------------------------*/

void* BaseAllocator::Alloc(int32 size)
{
	return std::malloc(size);
}

void BaseAllocator::Release(void* ptr)
{
	std::free(ptr);
}
