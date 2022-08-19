#include <gtest/gtest.h>

#include "utilities.h"
#include "common.h"

TEST(Utilities, Split_String_Basic) {
    std::vector<std::string> tokens = SplitString("   \tAA \v Hello    World\t!   \t a ", SPACE_DELIMITERS);
    ASSERT_EQ(5, tokens.size());
    ASSERT_EQ("AA", tokens[0]);
    ASSERT_EQ("Hello", tokens[1]);
    ASSERT_EQ("World", tokens[2]);
    ASSERT_EQ("!", tokens[3]);
    ASSERT_EQ("a", tokens[4]);
}

TEST(Utilities, Split_String_Empty) {
    std::vector<std::string> tokens = SplitString("    \t\v  ", SPACE_DELIMITERS);
    ASSERT_EQ(0, tokens.size());


    tokens = SplitString("", SPACE_DELIMITERS);
    ASSERT_EQ(0, tokens.size());
}

TEST(Utilities, Strip_String_Basic) {
    std::string str = StripString("  \v \tAA \t Hello    World!\v\t");
    ASSERT_EQ("AA \t Hello    World!", str);

    str = StripString("AA \t Hello    World!");
    ASSERT_EQ("AA \t Hello    World!", str);
}

TEST(Utilities, Strip_String_Empty) {
    std::string str = StripString("");
    ASSERT_EQ("", str);

    str = StripString("   \t\v  ");
    ASSERT_EQ("", str);
}