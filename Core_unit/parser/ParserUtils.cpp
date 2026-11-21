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

TEST(ParserUtils, QuotedBracesDoNotChangeBlockBoundaries) {
    char source[] = R"($first {
        $text "Press } to open { menu" "$10 { continued"
        $nested { $text "}" }
    }
    $second { $value 42 })";
    const auto parser = ps_begin(source);
    ASSERT_NE(nullptr, parser);
    ASSERT_TRUE(ps_block_begin(parser, "first"));
    ASSERT_TRUE(ps_var_begin(parser, "text"));
    EXPECT_STREQ("Press } to open { menu", ps_var_str(parser));
    ASSERT_TRUE(ps_block_begin(parser, "nested"));
    ASSERT_TRUE(ps_var_begin(parser, "text"));
    EXPECT_STREQ("}", ps_var_str(parser));
    ps_block_end(parser);
    ps_block_end(parser);
    ASSERT_TRUE(ps_block_begin(parser, "second"));
    EXPECT_FLOAT_EQ(42, ps_get_f(parser, "value", 0));
    ps_block_end(parser);
    // Finding a sibling directly also exercises skipping the entire first block.
    ASSERT_TRUE(ps_block_begin(parser, "second"));
    EXPECT_FLOAT_EQ(42, ps_get_f(parser, "value", 0));
    ps_end(parser);
}

TEST(ParserUtils, FieldAndBlockSearchIgnoreQuotedPrefixes) {
    char source[] = R"($message "$10 { ghost } $value 99 $phantom { }"
        $value 7 $real { $value 11 })";
    const auto parser = ps_begin(source);
    ASSERT_NE(nullptr, parser);
    EXPECT_FLOAT_EQ(7, ps_get_f(parser, "value", 0));
    EXPECT_FALSE(ps_block_begin(parser, "phantom"));
    ASSERT_TRUE(ps_block_begin(parser, "real"));
    EXPECT_FLOAT_EQ(11, ps_get_f(parser, "value", 0));
    ps_end(parser);
}

TEST(ParserUtils, RepeatedBlocksAndVariablesSkipQuotedBraces) {
    char source[] = R"($items { $item { $text "}" } $item { $text "{" } }
        $value "} $value {" $value "last")";
    const auto parser = ps_begin(source);
    ASSERT_NE(nullptr, parser);
    ASSERT_TRUE(ps_block_begin(parser, "items"));
    ASSERT_TRUE(ps_block_begin(parser, "item"));
    EXPECT_STREQ("}", ps_get_str(parser, "text", ""));
    ASSERT_TRUE(ps_block_next(parser));
    EXPECT_STREQ("{", ps_get_str(parser, "text", ""));
    EXPECT_FALSE(ps_block_next(parser));
    ps_block_end(parser);
    ps_block_end(parser);
    ASSERT_TRUE(ps_var_begin(parser, "value"));
    ASSERT_TRUE(ps_var_next(parser));
    EXPECT_STREQ("last", ps_var_str(parser));
    EXPECT_FALSE(ps_var_next(parser));
    ps_end(parser);
}

TEST(ParserUtils, QuotedClosingBraceDoesNotTerminateUnclosedBlock) {
    char source[] = "$item { $text \"}\"";
    const auto parser = ps_begin(source);
    ASSERT_NE(nullptr, parser);
    EXPECT_FALSE(ps_block_begin(parser, "item"));
    ps_end(parser);
}

TEST(ParserUtils, CommentsWithLiteralGlyphsDoNotHideFollowingBlocks) {
    char source[] = R"($textures { $tex "atlas.png" }
        $chars_data {
            /*;char: " { // (22)*/
            $char { $code 34 }
            // Unmatched quote " and brace }
            $char { $code 65 }
            ; Unmatched quote " and brace {
            $char { $code 123 }
            /*;char: } (7D)*/
            $char { $code 125 }
        }
        $next { $value 42 } // trailing " comment without newline)";
    const auto parser = ps_begin(source);
    ASSERT_NE(nullptr, parser);
    ASSERT_TRUE(ps_block_begin(parser, "chars_data"));
    ASSERT_TRUE(ps_block_begin(parser, "char"));
    for (const auto code : {34, 65, 123, 125}) {
        EXPECT_FLOAT_EQ(code, ps_get_f(parser, "code", 0));
        EXPECT_EQ(code != 125, ps_block_next(parser));
    }
    ps_block_end(parser);
    ps_block_end(parser);
    ASSERT_TRUE(ps_block_begin(parser, "next"));
    EXPECT_FLOAT_EQ(42, ps_get_f(parser, "value", 0));
    ps_end(parser);
}

TEST(ParserUtils, CommentDelimitersInsideQuotedValuesArePreserved) {
    char source[] = R"($text "https://example.com;/* literal */ { }"
        /* comment with a literal " */ $value 7)";
    const auto parser = ps_begin(source);
    ASSERT_NE(nullptr, parser);
    EXPECT_STREQ("https://example.com;/* literal */ { }", ps_get_str(parser, "text", ""));
    EXPECT_FLOAT_EQ(7, ps_get_f(parser, "value", 0));
    ps_end(parser);
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


