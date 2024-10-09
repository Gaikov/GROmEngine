//
// Created by Roman Gaikov on 2019-09-09.
//

#include "UnitCommon.h"
#include "nsLib/Lock.h"

TEST(Lock, IsLocked) {
    nsLock lock;

    ASSERT_TRUE(!lock.IsLocked());

    lock.AddLock("test1");
    ASSERT_TRUE(lock.HasLock("test1"));
    ASSERT_TRUE(lock.IsLocked());

    lock.AddLock("test2");
    ASSERT_TRUE(lock.HasLock("test2"));
    ASSERT_TRUE(lock.IsLocked());

    lock.RemoveLock("test2");
    ASSERT_TRUE(!lock.HasLock("test2"));
    ASSERT_TRUE(lock.IsLocked());

    lock.RemoveLock("test1");
    ASSERT_TRUE(!lock.HasLock("test1"));
    ASSERT_TRUE(!lock.IsLocked());
}
    