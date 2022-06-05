#include <gtest/gtest.h>
#include "parser.h"

using namespace NKaleidoscope;

TEST(ParserTest, Def1) {
    std::string buffer = "def foo(x y) x+foo(y, 4.0);";
    auto source = TSource::FromString(std::move(buffer));

    TParser parser{LexTokens(source)};
    auto definition = parser.ParseDefinition();
    constexpr std::string_view expectedDump = R"(
Function definition: 
  Prototype: "foo", args: "x", "y"
  BinaryExpr: "+"
    VariableExpr: "x"
    CallExpr: "foo"
      VariableExpr: "y"
      NumberExpr: 4
)";
    EXPECT_EQ("\n" + definition->Dump(), expectedDump);
}

TEST(ParserTest, Def2) {
    std::string buffer = "def foo(x y) x+y y;";
    auto source = TSource::FromString(std::move(buffer));

    TParser parser{LexTokens(source)};
    auto definition = parser.ParseDefinition();
    constexpr std::string_view expectedDump = R"(
Function definition: 
  Prototype: "foo", args: "x", "y"
  BinaryExpr: "+"
    VariableExpr: "x"
    VariableExpr: "y"
)";
    EXPECT_EQ("\n" + definition->Dump(), expectedDump);
}

TEST(ParserTest, Extern1) {
    std::string buffer = "extern sin(a);";
    auto source = TSource::FromString(std::move(buffer));

    TParser parser{LexTokens(source)};
    auto definition = parser.ParseExtern();
    constexpr std::string_view expectedDump = R"(
Prototype: "sin", args: "a"
)";
    EXPECT_EQ("\n" + definition->Dump(), expectedDump);
}
