#include <gtest/gtest.h>
#include "codegen.h"
#include "lexer.h"
#include "parser.h"

using namespace NKaleidoscope;
using namespace NKaleidoscope::NAst;

namespace {

std::string Print(auto* element) {
    std::string str;
    llvm::raw_string_ostream rso{str};
    element->print(rso);
    return str;
}

} // namespace

TEST(CodegenTest, NumberExpr) {
    TCodegenVisitor codegen;
    const TNumberExpr asstNode{/* value = */ 1.2};
    asstNode.Accept(codegen);

    EXPECT_EQ(Print(codegen.GetValue()), "double 1.200000e+00");
}

TEST(CodegenTest, BinaryExprFromSource) {
    TCodegenVisitor codegen;

    auto source = TSource::FromString("4+5;");
    auto tokens = LexTokens(source);
    auto parser = TParser{std::move(tokens)};
    for (auto&& astNode : parser.ParseChunk()) {
        astNode->Accept(codegen);
    }

    EXPECT_EQ(Print(codegen.GetValue()), "double 9.000000e+00");
}

TEST(CodegenTest, SimpleFunc) {
    TCodegenVisitor codegen;

    auto source = TSource::FromString("def foo(a b) a*a + 2*a*b + b*b;");
    auto tokens = LexTokens(source);
    auto parser = TParser{std::move(tokens)};
    for (auto&& astNode : parser.ParseChunk()) {
        astNode->Accept(codegen);
    }

    EXPECT_EQ("\n" + Print(codegen.GetFunction()), R"(
define double @foo(double %a, double %b) {
entry:
  %multmp = fmul double %a, %a
  %multmp1 = fmul double %a, 2.000000e+00
  %multmp2 = fmul double %multmp1, %b
  %addtmp = fadd double %multmp, %multmp2
  %multmp3 = fmul double %b, %b
  %addtmp4 = fadd double %multmp3, %addtmp
  ret double %addtmp4
}
)");
}

TEST(CodegenTest, RecursiveFunc) {
    TCodegenVisitor codegen;

    auto source = TSource::FromString(R"(
def foo(a b) a*a + 2*a*b + b*b;
def bar(a) foo(a, 4.0) + bar(31337);
)");
    auto tokens = LexTokens(source);
    auto parser = TParser{std::move(tokens)};
    for (auto&& astNode : parser.ParseChunk()) {
        astNode->Accept(codegen);
    }

    EXPECT_EQ("\n" + Print(codegen.GetFunction()), R"(
define double @bar(double %a) {
entry:
  %calltmp = call double @foo(double %a, double 4.000000e+00)
  %calltmp1 = call double @bar(double 3.133700e+04)
  %addtmp = fadd double %calltmp, %calltmp1
  ret double %addtmp
}
)");
}

TEST(CodegenTest, ExternFunc) {
    TCodegenVisitor codegen;

    auto source = TSource::FromString(R"(
extern cos(x);
)");
    auto tokens = LexTokens(source);
    auto parser = TParser{std::move(tokens)};
    for (auto&& astNode : parser.ParseChunk()) {
        astNode->Accept(codegen);
    }

    EXPECT_EQ("\n" + Print(codegen.GetFunction()), R"(
declare double @cos(double)
)");
}

TEST(CodegenTest, OptimizedFunc) {
    TCodegenVisitor codegen;

    auto source = TSource::FromString(R"(
def test(x) (1+2+x)*(x+(1+2))
)");
    auto tokens = LexTokens(source);
    auto parser = TParser{std::move(tokens)};
    for (auto&& astNode : parser.ParseChunk()) {
        astNode->Accept(codegen);
    }

    EXPECT_EQ("\n" + Print(codegen.GetFunction()), R"(
define double @test(double %x) {
entry:
  %addtmp = fadd double %x, 3.000000e+00
  %multmp = fmul double %addtmp, %addtmp
  ret double %multmp
}
)");
}
