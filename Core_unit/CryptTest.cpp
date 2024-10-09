//
// Created by Roman on 5/8/2024.
//

#include "UnitCommon.h"
#include "Core/Crypt.h"
#include "nsLib/StrTools.h"

#define CRYPT_DATA_TEST "Hello World from GROm!"

TEST(Crypt, XorEncode) {
    nsString data = CRYPT_DATA_TEST;
    nsString key = "grom";

    auto length = data.Length();

    nsCrypt::XorEncode(data.AsChar(), length, key.AsChar());
    printf("crypted: %s\n", data.AsChar());
    ASSERT_TRUE(strcmp(data, CRYPT_DATA_TEST) != 0);

    nsCrypt::XorEncode(data.AsChar(), length, key.AsChar());
    ASSERT_TRUE(strcmp(data, CRYPT_DATA_TEST) == 0);
}