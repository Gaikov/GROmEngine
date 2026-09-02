//
// Created by Roman Gaikov on 2019-07-23.
//
#include "UnitCommon.h"
#include "nsLib/StrTools.h"

#include <cstring>
#include <string>

TEST(String, ToLower) {

    nsString str = "ABS123!@#";
    nsString str1 = "abs123!@#";

    str.ToLower();

    ASSERT_TRUE(str == str1);
}

TEST(String, Format) {
    nsString str;

    str.Format("value: %i", 42);

    ASSERT_TRUE(str == "value: 42");
}

TEST(String, FormatTruncatesAndTerminates) {
    const std::string source(nsString::MAX_SIZE + 32, 'x');
    nsString str;

    str.Format("%s", source.c_str());

    ASSERT_EQ(nsString::MAX_SIZE - 1, str.Length());
    ASSERT_EQ('\0', str.AsChar()[nsString::MAX_SIZE - 1]);
}

TEST(String, StrPrintfTruncatesAndTerminates) {
    const std::string source(1200, 'x');

    const char *result = StrPrintf("%s", source.c_str());

    ASSERT_EQ(1023U, std::strlen(result));
    ASSERT_EQ('\0', result[1023]);
}

