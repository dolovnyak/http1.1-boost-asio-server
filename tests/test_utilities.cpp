#include <gtest/gtest.h>

#include "utilities.h"
#include "Http.h"

TEST(Utilities, Split_String_Basic) {
    std::vector<std::string> tokens = SplitString("   \tAA \v Hello    World\t!   \t a ", DELIMITERS);
    ASSERT_EQ(5, tokens.size());
    ASSERT_EQ("AA", tokens[0]);
    ASSERT_EQ("Hello", tokens[1]);
    ASSERT_EQ("World", tokens[2]);
    ASSERT_EQ("!", tokens[3]);
    ASSERT_EQ("a", tokens[4]);

    tokens = SplitString("   World\t\t  ", DELIMITERS);
    ASSERT_EQ(1, tokens.size());
    ASSERT_EQ("World", tokens[0]);
}

TEST(Utilities, Split_String_Empty) {
    std::vector<std::string> tokens = SplitString("    \t\v  ", DELIMITERS);
    ASSERT_EQ(0, tokens.size());


    tokens = SplitString("", DELIMITERS);
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

TEST(Utilities, Find_In_Range_Basic) {
    size_t pos = FindInRange("AA \t Hello    World!\v\t", "Hello", 0, std::string::npos);
    ASSERT_EQ(5, pos);
    pos = FindInRange("AA \t Hello    World!\v\t", "Hello", 5, 9);
    ASSERT_EQ(5, pos);
    pos = FindInRange("AA \t Hello    World!\v\t", "Hello", 5, 8);
    ASSERT_EQ(std::string::npos, pos);
    pos = FindInRange("A q1241352453245345", "A", 0, 0);
    ASSERT_EQ(0, pos);
    pos = FindInRange("AA q1241352453245345", "AA", 0, 0);
    ASSERT_EQ(std::string::npos, pos);
    pos = FindInRange("AA q1241352453245345", "AA", 0, 1);
    ASSERT_EQ(0, pos);
}

TEST(Utilities, Find_In_Range_Empty) {
    size_t pos = FindInRange("", "", 0, 0);
    ASSERT_EQ(std::string::npos, pos);
    pos = FindInRange("", "", 0, std::string::npos);
    ASSERT_EQ(std::string::npos, pos);
}

TEST(Utilities, To_Lower_Basic) {
    std::string str = ToLower("AA \t Hello    World!\v\t");
    ASSERT_EQ("aa \t hello    world!\v\t", str);
}

TEST(Utilities, To_Lower_Empty) {
    std::string str = ToLower("");
    ASSERT_EQ("", str);
}

TEST(Utilities, Is_Ip_V4_Address) {
    std::string str = "1.2.3.4";
    ASSERT_TRUE(IsIpv4(str));

    str = "111.22.255.1";
    ASSERT_TRUE(IsIpv4(str));

    str = "111.256.1.2";
    ASSERT_FALSE(IsIpv4(str));

    str = "111.225.1.";
    ASSERT_FALSE(IsIpv4(str));

    str = ".111.225.1";
    ASSERT_FALSE(IsIpv4(str));

    str = "";
    ASSERT_FALSE(IsIpv4(str));

    str = "...";
    ASSERT_FALSE(IsIpv4(str));
}
