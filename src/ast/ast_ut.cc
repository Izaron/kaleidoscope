#include <gtest/gtest.h>
#include "ast.h"

using namespace NKaleidoscope;
using namespace NKaleidoscope::NAst;

TEST(AstTest, DumpNumberExpr) {
    std::stringstream ss;
    TNumberExpr{/* value = */ 1.2}.Dump(ss);
    EXPECT_EQ(ss.view(), "NumberExpr: 1.2");
}

TEST(AstTest, DumpVariableExpr) {
    auto source = TSource::FromString("foo");
    auto sourceRange = TSourceRange{&source, 0, 3};

    std::stringstream ss;
    TVariableExpr{/* name = */ sourceRange}.Dump(ss);
    constexpr std::string_view expectedDump = R"(VariableExpr: "foo")";
    EXPECT_EQ(ss.view(), expectedDump);
}

TEST(AstTest, DumpBinaryExpr) {
    auto source = TSource::FromString("foo");
    auto sourceRange = TSourceRange{&source, 0, 3};

    auto lhsExpr = std::make_unique<TVariableExpr>(sourceRange);
    auto rhsExpr = std::make_unique<TNumberExpr>(1.2);
    TBinaryExpr binaryExpr{
        TBinaryExpr::EOp::Plus,
        std::move(lhsExpr),
        std::move(rhsExpr)
    };

    std::stringstream ss;
    binaryExpr.Dump(ss);
    constexpr std::string_view expectedDump =
R"(BinaryExpr: "+"
  VariableExpr: "foo"
  NumberExpr: 1.2)";
    EXPECT_EQ(ss.view(), expectedDump);
}
