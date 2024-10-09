//
// Created by Roman Gaikov on 2019-07-23.
//
#include "UnitCommon.h"
#include "nsLib/StrTools.h"

TEST(String, ToLower) {

    nsString str = "ABS123!@#";
    nsString str1 = "abs123!@#";

    str.ToLower();

    ASSERT_TRUE(str == str1);
}


