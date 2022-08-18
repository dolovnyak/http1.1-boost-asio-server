#include <gtest/gtest.h>

#include "utilities.h"

TEST(Utilities, Split_String_Basic) {
    std::vector<std::string> tokens = SplitString("   AA  Hello    World  !    a ", ' ');
    ASSERT_EQ(5, tokens.size());
    ASSERT_EQ("AA", tokens[0]);
    ASSERT_EQ("Hello", tokens[1]);
    ASSERT_EQ("World", tokens[2]);
    ASSERT_EQ("!", tokens[3]);
    ASSERT_EQ("a", tokens[4]);
}

TEST(Utilities, Split_String_Empty) {
    std::vector<std::string> tokens = SplitString("      ", ' ');
    ASSERT_EQ(0, tokens.size());


    tokens = SplitString("", ' ');
    ASSERT_EQ(0, tokens.size());
}
