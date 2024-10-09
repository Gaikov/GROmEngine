//
// Created by Roman Gaikov on 2019-07-23.
//
#include "UnitCommon.h"
#include "Core/ParserUtils.h"
#include "Core/Parser.h"

TEST(ParserUtils, Float) {

    char *source = "$value";
    char *source1 = "$value 10";

    auto ss = ps_begin(source);
    auto value = ParseFloat(ss, "value", 0);
    ps_end(ss);
    ASSERT_TRUE(value == 0);

    ss = ps_begin(source1);
    value = ParseFloat(ss, "value", 0);
    ps_end(ss);
    ASSERT_TRUE(value == 10);
}


