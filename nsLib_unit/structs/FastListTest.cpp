//
// Created by Roman on 7/26/2024.
//

#include "UnitCommon.h"
#include "nsLib/structs/FastList.h"

class TestItem : public nsListItem<TestItem> {

};

TEST(FastList, Add) {
    nsFastList<TestItem>    list;
    TestItem i1, i2, i3;

    ASSERT_TRUE(list.IsEmpty());
    list.Add(&i1);
    ASSERT_TRUE(!list.IsEmpty());
    ASSERT_TRUE(list.GetHead() == &i1);
    ASSERT_TRUE(list.HasItem(&i1));
    ASSERT_TRUE(list.ComputeAmount() == 1);

    list.Add(&i2);
    ASSERT_TRUE(list.GetHead() == &i2);
    ASSERT_TRUE(list.HasItem(&i2));
    ASSERT_TRUE(list.ComputeAmount() == 2);

    list.Add(&i3);
    ASSERT_TRUE(list.GetHead() == &i3);
    ASSERT_TRUE(list.HasItem(&i3));
    ASSERT_TRUE(list.ComputeAmount() == 3);


    list.Remove(&i3);
    ASSERT_TRUE(list.GetHead() != &i3);
    ASSERT_TRUE(!list.HasItem(&i3));
    ASSERT_TRUE(list.ComputeAmount() == 2);

    list.Remove(&i1);
    ASSERT_TRUE(list.GetHead() != &i1);
    ASSERT_TRUE(!list.HasItem(&i1));
    ASSERT_TRUE(list.ComputeAmount() == 1);

    list.Remove(&i2);
    ASSERT_TRUE(list.GetHead() != &i2);
    ASSERT_TRUE(!list.HasItem(&i2));
    ASSERT_TRUE(list.ComputeAmount() == 0);
    ASSERT_TRUE(list.IsEmpty());


}
