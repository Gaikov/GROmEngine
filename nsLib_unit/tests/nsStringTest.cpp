//
// Created by Roman Gaikov on 2019-07-23.
//
#include "UnitCommon.h"
#include "nsLib/StrTools.h"

#include <cstring>
#include <string>
#include <unordered_map>

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

TEST(String, HashSupportsUnorderedMap) {
    const nsString first = "levels/level_orange.txt";
    const nsString equal = "levels/level_orange.txt";
    const nsString other = "levels/level_1.txt";

    ASSERT_EQ(first.Hash(), equal.Hash());

    std::unordered_map<nsString, int> values;
    values.emplace(first, 42);

    ASSERT_EQ(42, values.at(equal));
    ASSERT_EQ(values.end(), values.find(other));
}
