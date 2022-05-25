#include <gtest/gtest.h>
#include <list>
#include "ast.h"

using namespace NKaleidoscope;
using namespace NKaleidoscope::NAst;

TEST(AstTest, DumpNumberExpr) {
    EXPECT_EQ(TNumberExpr{/* value = */ 1.2}.Dump(), "NumberExpr: 1.2\n");
}

TEST(AstTest, DumpVariableExpr) {
    auto source = TSource::FromString("foo");
    auto sourceRange = TSourceRange{&source, 0, 3};

    constexpr std::string_view expectedDump =
R"(VariableExpr: "foo"
)";
    EXPECT_EQ(TVariableExpr{/* name = */ sourceRange}.Dump(), expectedDump);
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

    constexpr std::string_view expectedDump =
R"(BinaryExpr: "+"
  VariableExpr: "foo"
  NumberExpr: 1.2
)";
    EXPECT_EQ(binaryExpr.Dump(), expectedDump);
}

TEST(AstTest, DumpCallExpr) {
    auto firstArgSource = TSource::FromString("foo");
    auto firstArgSourceRange = TSourceRange{&firstArgSource, 0, 3};
    auto firstArg = std::make_unique<TVariableExpr>(firstArgSourceRange);

    auto secondArg = std::make_unique<TNumberExpr>(1.2);
    auto thirdArg = std::make_unique<TNumberExpr>(3.4);

    auto calleeSource = TSource::FromString("func");
    auto calleeSourceRange = TSourceRange{&calleeSource, 0, 4};

    std::vector<std::unique_ptr<TExpr>> args;
    args.emplace_back(std::move(firstArg));
    args.emplace_back(std::move(secondArg));
    args.emplace_back(std::move(thirdArg));

    TCallExpr callExpr{calleeSourceRange, std::move(args)};
    constexpr std::string_view expectedDump =
R"(CallExpr: "func"
  VariableExpr: "foo"
  NumberExpr: 1.2
  NumberExpr: 3.4
)";
    EXPECT_EQ(callExpr.Dump(), expectedDump);
}

TEST(AstTest, DumpPrototype) {
    auto nameSource = TSource::FromString("func");
    auto nameSourceRange = TSourceRange{&nameSource, 0, 4};

    auto arg0Source = TSource::FromString("arg0");
    auto arg0SourceRange = TSourceRange{&arg0Source, 0, 4};

    auto arg1Source = TSource::FromString("arg1");
    auto arg1SourceRange = TSourceRange{&arg1Source, 0, 4};

    TPrototype prototype{nameSourceRange, std::vector<TSourceRange>{arg0SourceRange, arg1SourceRange}};
    constexpr std::string_view expectedDump =
R"(Prototype: "func", args: "arg0", "arg1"
)";
    EXPECT_EQ(prototype.Dump(), expectedDump);
}
