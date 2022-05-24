#include <gtest/gtest.h>
#include "lexer.h"

using namespace NKaleidoscope;

namespace {

struct TMockTokenVisitor : ITokenVisitor {
    void Visit(TToken token) override {
        Tokens.emplace_back(std::move(token));
    }

    std::vector<TToken> Tokens;
};

const std::string SMOKE_PROGRAM = R"(
# Compute the x'th fibonacci number.
def fib(x)
  if x < 3 then
    1
  else
    fib(x-1)+fib(x-2)

# This expression will compute the 40th number.
fib(40)
)";

} // namespace

TEST(LexerTest, Smoke) {
    using enum ETokenKind;

    const TSource source = TSource::FromString(SMOKE_PROGRAM);
    TMockTokenVisitor tokenVisitor;
    ParseTokens(source, tokenVisitor);

    const auto& tokens = tokenVisitor.Tokens;
    EXPECT_EQ(tokens.size(), 30);

    std::vector<ETokenKind> tokenKinds = {
        // def fib(x)
        Def, Identifier, Invalid, Identifier, Invalid,
        // if x < 3 then
        Identifier, Identifier, Less, Number, Identifier,
        // 1
        Number,
        // else
        Identifier,
        // fib(x-1)+fib(x-2)
        Identifier, Invalid, Identifier, Minus, Number, Invalid, Plus,
        Identifier, Invalid, Identifier, Minus, Number, Invalid,
        // fib(40)
        Identifier, Invalid, Number, Invalid,
    };

    for (std::size_t i = 0; i < tokenKinds.size(); ++i) {
        EXPECT_EQ(tokens[i].Kind, tokenKinds[i]);
    }
}
