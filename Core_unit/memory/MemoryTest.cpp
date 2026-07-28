#include "Core/Memory.h"
#include "nsLib/log.h"
#include <gtest/gtest.h>
#include <atomic>
#include <cstdio>
#include <cstdint>
#include <new>
#include <thread>

namespace {
bool IsZeroed(const void *data, size_t size) {
	const auto *bytes = static_cast<const unsigned char *>(data);
	for (size_t i = 0; i < size; ++i) {
		if (bytes[i] != 0) {
			return false;
		}
	}
	return true;
}

struct ThrowingObject {
	ThrowingObject() {
		throw 1;
	}
};

struct alignas(256) ThrowingAlignedObject {
	ThrowingAlignedObject() {
		throw 1;
	}
};

class AllocationWarningListener final : public ILogListener {
public:
	void LogPrint(LogLevel level, const char *str) override {
		if (level == PRN_WARNING) {
			std::sscanf(str, "WARNING: Memory allocations in game loop: %i", &allocations);
		}
	}

	int allocations = -1;
};
}

TEST(MemoryTest, TracksOrdinaryScalarAndArrayAllocations) {
	const int scalarBefore = nsMemory::LiveAllocations();
	void *scalar = ::operator new(37);
	const int scalarAllocated = nsMemory::LiveAllocations();
	const bool scalarZeroed = IsZeroed(scalar, 37);
	::operator delete(scalar);
	const int scalarFreed = nsMemory::LiveAllocations();

	const int arrayBefore = nsMemory::LiveAllocations();
	void *array = ::operator new[](73);
	const int arrayAllocated = nsMemory::LiveAllocations();
	const bool arrayZeroed = IsZeroed(array, 73);
	::operator delete[](array);
	const int arrayFreed = nsMemory::LiveAllocations();

	EXPECT_EQ(scalarAllocated, scalarBefore + 1);
	EXPECT_EQ(scalarFreed, scalarBefore);
	EXPECT_TRUE(scalarZeroed);
	EXPECT_EQ(arrayAllocated, arrayBefore + 1);
	EXPECT_EQ(arrayFreed, arrayBefore);
	EXPECT_TRUE(arrayZeroed);
}

TEST(MemoryTest, TracksNothrowAllocations) {
	const int scalarBefore = nsMemory::LiveAllocations();
	void *scalar = ::operator new(41, std::nothrow);
	const int scalarAllocated = nsMemory::LiveAllocations();
	::operator delete(scalar, std::nothrow);
	const int scalarFreed = nsMemory::LiveAllocations();

	const int arrayBefore = nsMemory::LiveAllocations();
	void *array = ::operator new[](79, std::nothrow);
	const int arrayAllocated = nsMemory::LiveAllocations();
	::operator delete[](array, std::nothrow);
	const int arrayFreed = nsMemory::LiveAllocations();

	ASSERT_NE(scalar, nullptr);
	ASSERT_NE(array, nullptr);
	EXPECT_EQ(scalarAllocated, scalarBefore + 1);
	EXPECT_EQ(scalarFreed, scalarBefore);
	EXPECT_EQ(arrayAllocated, arrayBefore + 1);
	EXPECT_EQ(arrayFreed, arrayBefore);
}

TEST(MemoryTest, TracksAlignedAndSizedAlignedAllocations) {
	const auto alignment64 = std::align_val_t(64);
	const int scalarBefore = nsMemory::LiveAllocations();
	void *scalar = ::operator new(117, alignment64);
	const int scalarAllocated = nsMemory::LiveAllocations();
	const bool scalarAligned = reinterpret_cast<uintptr_t>(scalar) % 64 == 0;
	const bool scalarZeroed = IsZeroed(scalar, 117);
	::operator delete(scalar, alignment64);
	const int scalarFreed = nsMemory::LiveAllocations();

	const auto alignment256 = std::align_val_t(256);
	const int arrayBefore = nsMemory::LiveAllocations();
	void *array = ::operator new[](513, alignment256);
	const int arrayAllocated = nsMemory::LiveAllocations();
	const bool arrayAligned = reinterpret_cast<uintptr_t>(array) % 256 == 0;
	const bool arrayZeroed = IsZeroed(array, 513);
	::operator delete[](array, 513, alignment256);
	const int arrayFreed = nsMemory::LiveAllocations();

	EXPECT_TRUE(scalarAligned);
	EXPECT_TRUE(scalarZeroed);
	EXPECT_EQ(scalarAllocated, scalarBefore + 1);
	EXPECT_EQ(scalarFreed, scalarBefore);
	EXPECT_TRUE(arrayAligned);
	EXPECT_TRUE(arrayZeroed);
	EXPECT_EQ(arrayAllocated, arrayBefore + 1);
	EXPECT_EQ(arrayFreed, arrayBefore);
}

TEST(MemoryTest, TracksAlignedNothrowAllocations) {
	const auto alignment = std::align_val_t(128);
	const int before = nsMemory::LiveAllocations();
	void *data = ::operator new(91, alignment, std::nothrow);
	const int allocated = nsMemory::LiveAllocations();
	const bool aligned = data && reinterpret_cast<uintptr_t>(data) % 128 == 0;
	::operator delete(data, alignment, std::nothrow);
	const int freed = nsMemory::LiveAllocations();

	ASSERT_NE(data, nullptr);
	EXPECT_TRUE(aligned);
	EXPECT_EQ(allocated, before + 1);
	EXPECT_EQ(freed, before);
}

TEST(MemoryTest, SupportsZeroSizeAllocations) {
	const int before = nsMemory::LiveAllocations();
	void *data = ::operator new(0);
	const int allocated = nsMemory::LiveAllocations();
	::operator delete(data, size_t(0));
	const int freed = nsMemory::LiveAllocations();

	EXPECT_NE(data, nullptr);
	EXPECT_EQ(allocated, before + 1);
	EXPECT_EQ(freed, before);
}

TEST(MemoryTest, ReleasesMemoryWhenConstructorsThrow) {
	const int scalarBefore = nsMemory::LiveAllocations();
	bool scalarThrew = false;
	try {
		new ThrowingObject();
	} catch (int) {
		scalarThrew = true;
	}
	const int scalarAfter = nsMemory::LiveAllocations();

	const int alignedBefore = nsMemory::LiveAllocations();
	bool alignedThrew = false;
	try {
		new ThrowingAlignedObject();
	} catch (int) {
		alignedThrew = true;
	}
	const int alignedAfter = nsMemory::LiveAllocations();

	EXPECT_TRUE(scalarThrew);
	EXPECT_EQ(scalarAfter, scalarBefore);
	EXPECT_TRUE(alignedThrew);
	EXPECT_EQ(alignedAfter, alignedBefore);
}

TEST(MemoryTest, MaintainsCounterAcrossThreads) {
	{
		std::thread warmup([]() {});
		warmup.join();
	}

	const int before = nsMemory::LiveAllocations();
	std::atomic<bool> valid = true;
	std::thread threads[4];
	for (auto &thread : threads) {
		thread = std::thread([&valid]() {
			for (int i = 0; i < 1000; ++i) {
				const auto alignment = std::align_val_t(64);
				void *data = ::operator new(127, alignment);
				if (reinterpret_cast<uintptr_t>(data) % 64 != 0 || !IsZeroed(data, 127)) {
					valid.store(false, std::memory_order_relaxed);
				}
				::operator delete(data, alignment);
			}
		});
	}
	for (auto &thread : threads) {
		thread.join();
	}
	const int after = nsMemory::LiveAllocations();

	EXPECT_TRUE(valid.load(std::memory_order_relaxed));
	EXPECT_EQ(after, before);
}

TEST(MemoryTest, CountsOrdinaryAndAlignedFrameAllocations) {
	AllocationWarningListener listener;
	Log::Init();
	Log::Shared()->AddPolicy(&listener);

	nsMemory::BeginLoop();
	void *ordinary = ::operator new(31);
	::operator delete(ordinary);
	const auto alignment = std::align_val_t(64);
	void *aligned = ::operator new(63, alignment);
	::operator delete(aligned, alignment);
	nsMemory::EndLoop();

	Log::Shared()->RemovePolicy(&listener);
	Log::Release();
	EXPECT_EQ(listener.allocations, 2);
}
