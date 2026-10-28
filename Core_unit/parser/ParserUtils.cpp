//
// Created by Roman Gaikov on 2019-07-23.
//
#include "UnitCommon.h"
#include "Core/ParserUtils.h"
#include "Core/Parser.h"
#include <limits>

TEST(ParserUtils, Float) {

    char source[] = "$value";
    char source1[] = "$value 10";

    auto ss = ps_begin(source);
    auto value = ParseFloat(ss, "value", 0);
    ps_end(ss);
    ASSERT_TRUE(value == 0);

    ss = ps_begin(source1);
    value = ParseFloat(ss, "value", 0);
    ps_end(ss);
    ASSERT_TRUE(value == 10);
}

TEST( Color, NormalizeClampsChannelsIndependently ) {
    nsColor color( 2, -1, 0.5f, 4 );
    color.Normalize();
    EXPECT_FLOAT_EQ( 1, color.r );
    EXPECT_FLOAT_EQ( 0, color.g );
    EXPECT_FLOAT_EQ( 0.5f, color.b );
    EXPECT_FLOAT_EQ( 1, color.a );
    color.Normalize();
    EXPECT_FLOAT_EQ( 0.5f, color.b );
    EXPECT_EQ( nsColor( 1, 0, 0.5f, 1 ).ToARGB(), color.ToARGB() );
}

TEST( Color, NormalizeMakesNonFiniteChannelsSafeToPack ) {
    const auto infinity = std::numeric_limits<float>::infinity();
    nsColor color( infinity, -infinity, std::numeric_limits<float>::quiet_NaN(), 0.25f );
    color.Normalize();
    EXPECT_FLOAT_EQ( 1, color.r );
    EXPECT_FLOAT_EQ( 0, color.g );
    EXPECT_FLOAT_EQ( 0, color.b );
    EXPECT_FLOAT_EQ( 0.25f, color.a );
}

TEST( ParserUtils, ColorPreservesRangeByDefault ) {
    char source[] = "$color 2 -1 0.5 4";
    const auto state = ps_begin( source );
    ASSERT_NE( nullptr, state );
    nsColor color;
    EXPECT_TRUE( ParseColorExt( state, "color", color ) );
    EXPECT_FLOAT_EQ( 2, color.r );
    EXPECT_FLOAT_EQ( -1, color.g );
    EXPECT_FLOAT_EQ( 0.5f, color.b );
    EXPECT_FLOAT_EQ( 4, color.a );
    ps_end( state );
}

TEST( ParserUtils, ColorNormalizationWorksForBothOverloads ) {
    for ( const auto named : { false, true } ) {
        char source[] = "$color 2 -1 0.5 4";
        const auto state = ps_begin( source );
        ASSERT_NE( nullptr, state );
        nsColor color;
        if ( named ) {
            EXPECT_TRUE( ParseColorExt( state, "color", color, true ) );
        } else {
            ASSERT_TRUE( ps_var_begin( state, "color" ) );
            EXPECT_TRUE( ParseColorExt( state, color, true ) );
        }
        EXPECT_FLOAT_EQ( 1, color.r );
        EXPECT_FLOAT_EQ( 0, color.g );
        EXPECT_FLOAT_EQ( 0.5f, color.b );
        EXPECT_FLOAT_EQ( 1, color.a );
        ps_end( state );
    }
}

TEST( ParserUtils, NormalizedColorKeepsHexRgba ) {
    for ( const auto normalize : { false, true } ) {
        char source[] = "$color \"FF804020\"";
        const auto state = ps_begin( source );
        ASSERT_NE( nullptr, state );
        nsColor color;
        EXPECT_TRUE( ParseColorExt( state, "color", color, normalize ) );
        EXPECT_FLOAT_EQ( 1, color.r );
        EXPECT_FLOAT_EQ( 128.0f / 255, color.g );
        EXPECT_FLOAT_EQ( 64.0f / 255, color.b );
        EXPECT_FLOAT_EQ( 32.0f / 255, color.a );
        ps_end( state );
    }
}

TEST( ParserUtils, FailedColorParsePreservesDestination ) {
    char source[] = "$color \"invalid\"";
    const auto state = ps_begin( source );
    ASSERT_NE( nullptr, state );
    nsColor color( 2, -1, 0.5f, 4 );
    EXPECT_FALSE( ParseColorExt( state, "color", color, true ) );
    EXPECT_FALSE( ParseColorExt( state, "missing", color, true ) );
    EXPECT_FLOAT_EQ( 2, color.r );
    EXPECT_FLOAT_EQ( -1, color.g );
    EXPECT_FLOAT_EQ( 0.5f, color.b );
    EXPECT_FLOAT_EQ( 4, color.a );
    ps_end( state );
}


