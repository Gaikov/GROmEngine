#include "Memory.h"
#include "sys.h"
#include "nsLib/log.h"
#include "nsLib/StrTools.h"
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <mutex>

namespace {
thread_local bool g_inLoop = false;
thread_local int g_loopAllocations = 0;
thread_local int g_loopAllocationScope = 0;
std::atomic<int> g_allocatedBlocks = 0;

constexpr uintptr_t ALIGNED_ALLOCATION_ID = 0xA11CA7ED;

struct AlignedAllocationHeader {
	void *rawData;
	uintptr_t id;
};

void RecordAllocationEvent() {
	if (g_inLoop && g_loopAllocationScope == 0) {
		g_loopAllocations++;
	}
}

void RecordBlockAllocated() {
	RecordAllocationEvent();
	g_allocatedBlocks.fetch_add(1, std::memory_order_relaxed);
}

void RecordBlockFreed() {
	g_allocatedBlocks.fetch_sub(1, std::memory_order_relaxed);
}

size_t NormalizeSize(size_t size) {
	return size == 0 ? 1 : size;
}

size_t NormalizeAlignment(size_t alignment) {
	if (alignment == 0 || (alignment & (alignment - 1)) != 0) {
		throw std::bad_alloc();
	}
	return std::max(alignment, alignof(AlignedAllocationHeader));
}

template <class Allocator>
void *AllocateWithNewHandler(const Allocator &allocator) {
	for (;;) {
		if (void *data = allocator()) {
			return data;
		}

		std::new_handler handler = std::get_new_handler();
		if (!handler) {
			throw std::bad_alloc();
		}
		handler();
	}
}

void *AllocateRaw(size_t size) {
	const size_t allocationSize = NormalizeSize(size);
	void *data = AllocateWithNewHandler([allocationSize]() {
		return std::malloc(allocationSize);
	});
	std::memset(data, 0, allocationSize);
	return data;
}

void *AllocateAlignedRaw(size_t size, size_t requestedAlignment) {
	const size_t allocationSize = NormalizeSize(size);
	const size_t alignment = NormalizeAlignment(requestedAlignment);
	constexpr size_t headerSize = sizeof(AlignedAllocationHeader);

	if (allocationSize > std::numeric_limits<size_t>::max() - headerSize - (alignment - 1)) {
		throw std::bad_alloc();
	}

	const size_t realSize = allocationSize + headerSize + alignment - 1;
	void *rawData = AllocateWithNewHandler([realSize]() {
		return std::malloc(realSize);
	});

	const uintptr_t begin = reinterpret_cast<uintptr_t>(rawData) + headerSize;
	const uintptr_t aligned = (begin + alignment - 1) & ~(alignment - 1);
	auto *header = reinterpret_cast<AlignedAllocationHeader *>(aligned) - 1;
	header->rawData = rawData;
	header->id = ALIGNED_ALLOCATION_ID;

	void *data = reinterpret_cast<void *>(aligned);
	std::memset(data, 0, allocationSize);
	return data;
}

void FreeAlignedRaw(void *data) {
	if (!data) {
		return;
	}

	auto *header = reinterpret_cast<AlignedAllocationHeader *>(data) - 1;
	assert(header->id == ALIGNED_ALLOCATION_ID);
	if (header->id != ALIGNED_ALLOCATION_ID) {
		return;
	}

	void *rawData = header->rawData;
	header->id = 0;
	std::free(rawData);
}
}

#define MEM_ID 0xAFAF

#ifdef _TRACK_MEMORY_

namespace {
struct memblock_t {
	uword id;
	size_t realSize;
	size_t allocationSize;
	const char *file;
	int line;
	void *rawData;
	memblock_t *next;
	memblock_t *prev;
};

memblock_t *g_memList = nullptr;
size_t g_heapSize = 0;
size_t g_userSize = 0;

std::mutex &MemoryMutex() {
	// Global deletes may run after static destructors, so the allocator lock has process lifetime.
	static std::mutex *mutex = []() {
		void *storage = std::malloc(sizeof(std::mutex));
		if (!storage) {
			std::abort();
		}
		return ::new (storage) std::mutex();
	}();
	return *mutex;
}

void *AllocateTracked(size_t size, size_t requestedAlignment, const char *file, int line) {
	const size_t allocationSize = NormalizeSize(size);
	const size_t alignment = std::max(NormalizeAlignment(requestedAlignment), alignof(memblock_t));
	constexpr size_t headerSize = sizeof(memblock_t);

	if (allocationSize > std::numeric_limits<size_t>::max() - headerSize - (alignment - 1)) {
		throw std::bad_alloc();
	}

	const size_t realSize = allocationSize + headerSize + alignment - 1;
	void *rawData = AllocateWithNewHandler([realSize]() {
		return std::malloc(realSize);
	});

	const uintptr_t begin = reinterpret_cast<uintptr_t>(rawData) + headerSize;
	const uintptr_t aligned = (begin + alignment - 1) & ~(alignment - 1);
	auto *block = reinterpret_cast<memblock_t *>(aligned) - 1;
	block->id = MEM_ID;
	block->realSize = realSize;
	block->allocationSize = size;
	block->file = file ? file : "unknown file";
	block->line = line;
	block->rawData = rawData;
	block->prev = nullptr;

	{
		std::lock_guard<std::mutex> lock(MemoryMutex());
		block->next = g_memList;
		if (g_memList) {
			g_memList->prev = block;
		}
		g_memList = block;
		g_heapSize += block->realSize;
		g_userSize += block->allocationSize;
	}

	void *data = reinterpret_cast<void *>(aligned);
	std::memset(data, 0, allocationSize);
	RecordBlockAllocated();
	return data;
}

void FreeTracked(void *data) {
	if (!data) {
		return;
	}

	auto *block = reinterpret_cast<memblock_t *>(data) - 1;
	assert(block->id == MEM_ID);
	if (block->id != MEM_ID) {
		Log::Debug("WARNING: free invalid allocated block");
		return;
	}

	void *rawData = block->rawData;
	{
		std::lock_guard<std::mutex> lock(MemoryMutex());
		if (block == g_memList) {
			g_memList = block->next;
		} else if (block->prev) {
			block->prev->next = block->next;
		}
		if (block->next) {
			block->next->prev = block->prev;
		}

		g_heapSize -= block->realSize;
		g_userSize -= block->allocationSize;
		block->id = 0;
	}

	RecordBlockFreed();
	std::free(rawData);
}
}

void *mem_malloc(size_t size, const char *file, int line) {
	return AllocateTracked(size, alignof(std::max_align_t), file, line);
}

void *mem_realloc(void *data, size_t size, const char *file, int line) {
	if (!data) {
		return mem_malloc(size, file, line);
	}

	auto *oldBlock = reinterpret_cast<memblock_t *>(data) - 1;
	assert(oldBlock->id == MEM_ID);
	const size_t copySize = std::min(size, oldBlock->allocationSize);
	void *newData = mem_malloc(size, file, line);
	std::memcpy(newData, data, copySize);
	mem_free(data);
	return newData;
}

void mem_free(void *data) {
	FreeTracked(data);
}

namespace {
void *mem_aligned_malloc(size_t size, size_t alignment) {
	return AllocateTracked(size, alignment, "unknown file", 0);
}

void mem_aligned_free(void *data) {
	FreeTracked(data);
}
}

#else

void *mem_malloc(size_t size, const char *, int) {
	void *data = AllocateRaw(size);
	RecordBlockAllocated();
	return data;
}

void *mem_realloc(void *data, size_t size, const char *, int) {
	if (!data) {
		return mem_malloc(size, "unknown file", 0);
	}

	const size_t allocationSize = NormalizeSize(size);
	void *newData = AllocateWithNewHandler([data, allocationSize]() {
		return std::realloc(data, allocationSize);
	});
	RecordAllocationEvent();
	return newData;
}

void mem_free(void *data) {
	if (data) {
		RecordBlockFreed();
		std::free(data);
	}
}

namespace {
void *mem_aligned_malloc(size_t size, size_t alignment) {
	void *data = AllocateAlignedRaw(size, alignment);
	RecordBlockAllocated();
	return data;
}

void mem_aligned_free(void *data) {
	if (data) {
		RecordBlockFreed();
		FreeAlignedRaw(data);
	}
}
}

#endif

void *operator new(size_t size) {
	return mem_malloc(size, "unknown file", 0);
}

void *operator new[](size_t size) {
	return mem_malloc(size, "unknown file", 0);
}

void *operator new(size_t size, std::align_val_t alignment) {
	return mem_aligned_malloc(size, static_cast<size_t>(alignment));
}

void *operator new[](size_t size, std::align_val_t alignment) {
	return mem_aligned_malloc(size, static_cast<size_t>(alignment));
}

void *operator new(size_t size, const std::nothrow_t &) noexcept {
	try {
		return ::operator new(size);
	} catch (...) {
		return nullptr;
	}
}

void *operator new[](size_t size, const std::nothrow_t &) noexcept {
	try {
		return ::operator new[](size);
	} catch (...) {
		return nullptr;
	}
}

void *operator new(size_t size, std::align_val_t alignment, const std::nothrow_t &) noexcept {
	try {
		return ::operator new(size, alignment);
	} catch (...) {
		return nullptr;
	}
}

void *operator new[](size_t size, std::align_val_t alignment, const std::nothrow_t &) noexcept {
	try {
		return ::operator new[](size, alignment);
	} catch (...) {
		return nullptr;
	}
}

void operator delete(void *data) noexcept {
	mem_free(data);
}

void operator delete[](void *data) noexcept {
	mem_free(data);
}

void operator delete(void *data, size_t) noexcept {
	mem_free(data);
}

void operator delete[](void *data, size_t) noexcept {
	mem_free(data);
}

void operator delete(void *data, std::align_val_t) noexcept {
	mem_aligned_free(data);
}

void operator delete[](void *data, std::align_val_t) noexcept {
	mem_aligned_free(data);
}

void operator delete(void *data, size_t, std::align_val_t) noexcept {
	mem_aligned_free(data);
}

void operator delete[](void *data, size_t, std::align_val_t) noexcept {
	mem_aligned_free(data);
}

void operator delete(void *data, const std::nothrow_t &) noexcept {
	mem_free(data);
}

void operator delete[](void *data, const std::nothrow_t &) noexcept {
	mem_free(data);
}

void operator delete(void *data, std::align_val_t, const std::nothrow_t &) noexcept {
	mem_aligned_free(data);
}

void operator delete[](void *data, std::align_val_t, const std::nothrow_t &) noexcept {
	mem_aligned_free(data);
}

void nsMemory::BeginLoop() {
	g_inLoop = true;
}

void nsMemory::EndLoop() {
	g_inLoop = false;
	if (g_loopAllocations) {
		Log::Warning("Memory allocations in game loop: %i", g_loopAllocations);
	}
	g_loopAllocations = 0;
}

void nsMemory::PushLoopAllocationsScope() {
	g_loopAllocationScope++;
}

void nsMemory::PopLoopAllocationsScope() {
	assert(g_loopAllocationScope > 0);
	g_loopAllocationScope--;
}

int nsMemory::LiveAllocations() {
	return g_allocatedBlocks.load(std::memory_order_relaxed);
}

char *mem_strdup(const char *str, const char *file, int line) {
	if (!str) {
		assert(!"Invalid param!");
		return nullptr;
	}

	auto *res = static_cast<char *>(mem_malloc(std::strlen(str) + 1, file, line));
	std::strcpy(res, str);
	return res;
}

void mem_report(size_t &userAlloc, size_t &heapAlloc) {
#ifdef _TRACK_MEMORY_
	std::lock_guard<std::mutex> lock(MemoryMutex());
	userAlloc = g_userSize;
	heapAlloc = g_heapSize;
#else
	userAlloc = 0;
	heapAlloc = 0;
#endif
}

void mem_report() {
#ifdef _TRACK_MEMORY_
	bool leaksDetected = false;
	{
		std::lock_guard<std::mutex> lock(MemoryMutex());
		leaksDetected = g_memList != nullptr;
		if (leaksDetected) {
			FILE *fp = std::fopen("mem_leaks.txt", "w");
			if (fp) {
				std::fprintf(fp, "memory allocated: %zu\n", g_heapSize);
				for (memblock_t *block = g_memList; block; block = block->next) {
					std::fprintf(fp, "file: [%s], line: [%i], size: %zu (%zu)\n",
					             block->file, block->line, block->allocationSize, block->realSize);
				}
				std::fclose(fp);
			}
		}
	}
	if (leaksDetected) {
		Sys_Message("Memory leaks detected!");
	}
#endif

	const int allocatedBlocks = nsMemory::LiveAllocations();
	if (allocatedBlocks != 0) {
		std::printf("WARNING: Live mem blocks at report time: %i\n", allocatedBlocks);
	}
}
