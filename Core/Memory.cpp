#include "Memory.h"
#include "nsLib/log.h"
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <limits>

#if defined(_MSC_VER)
#include <intrin.h>
#include <windows.h>
#define MEMORY_CALLER_ADDRESS _ReturnAddress()
#elif defined(__GNUC__) || defined(__clang__)
#define MEMORY_CALLER_ADDRESS __builtin_return_address(0)
#else
#define MEMORY_CALLER_ADDRESS nullptr
#endif

#if !defined(_WIN32) && !defined(__EMSCRIPTEN__)
#include <dlfcn.h>
#endif

namespace {
using TrackingGeneration = uint64_t;

thread_local bool g_inLoop = false;
thread_local int g_loopAllocations = 0;
thread_local int g_loopAllocationScope = 0;

std::atomic<int> g_allocatedBlocks = 0;
std::atomic<TrackingGeneration> g_generationCounter = 0;
std::atomic<TrackingGeneration> g_activeGeneration = 0;
std::atomic<TrackingGeneration> g_reportGeneration = 0;

constexpr uintptr_t ALLOCATION_ID = 0xA11CA7ED;
constexpr size_t CALLER_CACHE_SIZE = 1024;

struct CallerCacheEntry {
	std::atomic<uintptr_t> caller = 0;
	std::atomic<bool> isEngine = false;
};

CallerCacheEntry g_callerCache[CALLER_CACHE_SIZE];

struct AllocationHeader {
	void *rawData;
	size_t allocationSize;
	size_t alignment;
	TrackingGeneration generation;
	uintptr_t id;
};

void RecordAllocationEvent() {
	if (g_inLoop && g_loopAllocationScope == 0) {
		g_loopAllocations++;
	}
}

void RecordBlockAllocated(TrackingGeneration generation) {
	RecordAllocationEvent();
	if (generation != 0 &&
	    generation == g_reportGeneration.load(std::memory_order_acquire)) {
		g_allocatedBlocks.fetch_add(1, std::memory_order_relaxed);
	}
}

void RecordBlockFreed(TrackingGeneration generation) {
	if (generation != 0 &&
	    generation == g_reportGeneration.load(std::memory_order_acquire)) {
		g_allocatedBlocks.fetch_sub(1, std::memory_order_relaxed);
	}
}

size_t NormalizeSize(size_t size) {
	return size == 0 ? 1 : size;
}

size_t NormalizeAlignment(size_t alignment) {
	if (alignment == 0 || (alignment & (alignment - 1)) != 0) {
		throw std::bad_alloc();
	}
	return std::max(alignment, alignof(AllocationHeader));
}

bool ResolveEngineAllocationCaller(void *caller) {
#if defined(__EMSCRIPTEN__)
	return true;
#elif defined(_WIN32)
	HMODULE callerModule = nullptr;
	const DWORD flags =
			GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
			GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;
	const BOOL callerFound = GetModuleHandleExA(
			flags, reinterpret_cast<LPCSTR>(caller), &callerModule);
	static HMODULE engineModule = []() {
		HMODULE module = nullptr;
		GetModuleHandleExA(
				GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
				GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
				reinterpret_cast<LPCSTR>(&ResolveEngineAllocationCaller), &module);
		return module;
	}();
	return callerFound && engineModule && callerModule == engineModule;
#else
	Dl_info callerInfo{};
	static void *engineBase = []() {
		Dl_info info{};
		return dladdr(reinterpret_cast<void *>(&ResolveEngineAllocationCaller), &info) != 0
		       ? info.dli_fbase
		       : nullptr;
	}();
	return engineBase &&
	       dladdr(caller, &callerInfo) != 0 &&
	       callerInfo.dli_fbase == engineBase;
#endif
}

bool IsEngineAllocationCaller(void *caller) {
	const uintptr_t address = reinterpret_cast<uintptr_t>(caller);
	if (address == 0) {
		return false;
	}

	CallerCacheEntry &entry =
			g_callerCache[(address >> 4) & (CALLER_CACHE_SIZE - 1)];
	uintptr_t cachedCaller = entry.caller.load(std::memory_order_acquire);
	if (cachedCaller == address) {
		return entry.isEngine.load(std::memory_order_relaxed);
	}

	const bool isEngine = ResolveEngineAllocationCaller(caller);
	if (cachedCaller == 0 &&
	    entry.caller.compare_exchange_strong(
			    cachedCaller, 1, std::memory_order_acq_rel, std::memory_order_acquire)) {
		entry.isEngine.store(isEngine, std::memory_order_relaxed);
		entry.caller.store(address, std::memory_order_release);
	}
	return isEngine;
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

void *AllocateBlock(size_t size, size_t requestedAlignment, void *caller) {
	const size_t allocationSize = NormalizeSize(size);
	const size_t alignment = NormalizeAlignment(requestedAlignment);
	constexpr size_t headerSize = sizeof(AllocationHeader);

	if (allocationSize > std::numeric_limits<size_t>::max() - headerSize - (alignment - 1)) {
		throw std::bad_alloc();
	}

	const size_t realSize = allocationSize + headerSize + alignment - 1;
	void *rawData = AllocateWithNewHandler([realSize]() {
		return std::malloc(realSize);
	});

	const uintptr_t begin = reinterpret_cast<uintptr_t>(rawData) + headerSize;
	const uintptr_t aligned = (begin + alignment - 1) & ~(alignment - 1);
	auto *block = reinterpret_cast<AllocationHeader *>(aligned) - 1;
	block->rawData = rawData;
	block->allocationSize = size;
	block->alignment = alignment;
	const TrackingGeneration activeGeneration =
			g_activeGeneration.load(std::memory_order_acquire);
	block->generation =
			activeGeneration != 0 && IsEngineAllocationCaller(caller)
			? activeGeneration
			: 0;
	block->id = ALLOCATION_ID;

	void *data = reinterpret_cast<void *>(aligned);
	std::memset(data, 0, allocationSize);
	RecordBlockAllocated(block->generation);
	return data;
}

AllocationHeader *GetAllocationHeader(void *data) {
	return reinterpret_cast<AllocationHeader *>(data) - 1;
}

void FreeBlock(void *data) {
	if (!data) {
		return;
	}

	AllocationHeader *block = GetAllocationHeader(data);
	assert(block->id == ALLOCATION_ID);
	if (block->id != ALLOCATION_ID) {
		return;
	}

	void *rawData = block->rawData;
	const TrackingGeneration generation = block->generation;

	block->id = 0;
	RecordBlockFreed(generation);
	std::free(rawData);
}

void *mem_aligned_malloc(size_t size, size_t alignment, void *caller) {
	return AllocateBlock(size, alignment, caller);
}

void mem_aligned_free(void *data) {
	FreeBlock(data);
}
}

void *mem_malloc(size_t size, const char *, int) {
	return AllocateBlock(size, alignof(std::max_align_t), MEMORY_CALLER_ADDRESS);
}

void *mem_realloc(void *data, size_t size, const char *, int) {
	if (!data) {
		return AllocateBlock(size, alignof(std::max_align_t), MEMORY_CALLER_ADDRESS);
	}

	AllocationHeader *oldBlock = GetAllocationHeader(data);
	assert(oldBlock->id == ALLOCATION_ID);
	if (oldBlock->id != ALLOCATION_ID) {
		return nullptr;
	}

	const size_t copySize = std::min(size, oldBlock->allocationSize);
	void *newData = AllocateBlock(size, oldBlock->alignment, MEMORY_CALLER_ADDRESS);
	std::memcpy(newData, data, copySize);
	FreeBlock(data);
	return newData;
}

void mem_free(void *data) {
	FreeBlock(data);
}

void *operator new(size_t size) {
	return AllocateBlock(size, alignof(std::max_align_t), MEMORY_CALLER_ADDRESS);
}

void *operator new[](size_t size) {
	return AllocateBlock(size, alignof(std::max_align_t), MEMORY_CALLER_ADDRESS);
}

void *operator new(size_t size, std::align_val_t alignment) {
	return mem_aligned_malloc(size, static_cast<size_t>(alignment),
	                          MEMORY_CALLER_ADDRESS);
}

void *operator new[](size_t size, std::align_val_t alignment) {
	return mem_aligned_malloc(size, static_cast<size_t>(alignment),
	                          MEMORY_CALLER_ADDRESS);
}

void *operator new(size_t size, const std::nothrow_t &) noexcept {
	try {
		return AllocateBlock(size, alignof(std::max_align_t), MEMORY_CALLER_ADDRESS);
	} catch (...) {
		return nullptr;
	}
}

void *operator new[](size_t size, const std::nothrow_t &) noexcept {
	try {
		return AllocateBlock(size, alignof(std::max_align_t), MEMORY_CALLER_ADDRESS);
	} catch (...) {
		return nullptr;
	}
}

void *operator new(size_t size, std::align_val_t alignment, const std::nothrow_t &) noexcept {
	try {
		return AllocateBlock(size, static_cast<size_t>(alignment), MEMORY_CALLER_ADDRESS);
	} catch (...) {
		return nullptr;
	}
}

void *operator new[](size_t size, std::align_val_t alignment, const std::nothrow_t &) noexcept {
	try {
		return AllocateBlock(size, static_cast<size_t>(alignment), MEMORY_CALLER_ADDRESS);
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

void nsMemory::StartTracking() {
	if (g_activeGeneration.load(std::memory_order_acquire) != 0) {
		return;
	}

	TrackingGeneration generation = g_generationCounter.fetch_add(1, std::memory_order_relaxed) + 1;
	if (generation == 0) {
		g_generationCounter.store(1, std::memory_order_relaxed);
		generation = 1;
	}

	g_allocatedBlocks.store(0, std::memory_order_relaxed);
	g_reportGeneration.store(generation, std::memory_order_release);

	g_activeGeneration.store(generation, std::memory_order_release);
}

void nsMemory::StopTracking() {
	g_activeGeneration.store(0, std::memory_order_release);
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

void mem_report() {
	const int allocatedBlocks = nsMemory::LiveAllocations();
	if (allocatedBlocks != 0) {
		std::printf("WARNING: Live engine-session mem blocks at report time: %i\n",
		            allocatedBlocks);
	}
}
