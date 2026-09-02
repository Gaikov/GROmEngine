#include "UnitCommon.h"
#include "Core/buffer/StringWriter.h"

#include <string>

TEST(StringWriter, PrintfWritesFormattedText) {
    nsStringWriter writer;

    ASSERT_TRUE(writer.Printf("value: %i", 42));
    ASSERT_EQ("value: 42", writer.GetBuffer());
}

TEST(StringWriter, PrintfTruncatesLongText) {
    nsStringWriter writer;
    const std::string source(1200, 'x');

    ASSERT_TRUE(writer.Printf("%s", source.c_str()));
    ASSERT_EQ(1023U, writer.GetBuffer().size());
    ASSERT_EQ(std::string(1023, 'x'), writer.GetBuffer());
}
