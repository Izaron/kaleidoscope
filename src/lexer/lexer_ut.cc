#include <gtest/gtest.h>
#include "lexer.h"

using namespace NKaleidoscope;
using enum ETokenKind;

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

const std::vector<ETokenKind> TOKEN_KINDS = {
    // def fib(x)
    Def, Identifier, LBracket, Identifier, RBracket,
    // if x < 3 then
    Identifier, Identifier, Less, Number, Identifier,
    // 1
    Number,
    // else
    Identifier,
    // fib(x-1)+fib(x-2)
    Identifier, LBracket, Identifier, Minus, Number, RBracket, Plus,
    Identifier, LBracket, Identifier, Minus, Number, RBracket,
    // fib(40)
    Identifier, LBracket, Number, RBracket,
    // <eof>
    Eof,
};

const std::vector<double> TOKEN_NUMBERS = {
    // x < 3
    3,
    // 1
    1,
    // fib(x-1)+fib(x-2)
    1, 2,
    // fib(40)
    40,
};

} // namespace

TEST(LexerTest, Smoke) {
    const TSource source = TSource::FromString(SMOKE_PROGRAM);
    TMockTokenVisitor tokenVisitor;
    ParseTokens(source, tokenVisitor);

    const auto& tokens = tokenVisitor.Tokens;
    EXPECT_EQ(tokens.size(), 30);

    std::size_t numbersIndex = 0;
    for (std::size_t i = 0; i < TOKEN_KINDS.size(); ++i) {
        const auto kind = tokens[i].Kind;
        EXPECT_EQ(kind, TOKEN_KINDS[i]);
        if (kind == Number) {
            EXPECT_EQ(tokens[i].SourceRange.AsDouble(), TOKEN_NUMBERS[numbersIndex]);
            ++numbersIndex;
        }
    }
}

TEST(LexerTest, TokenListSmoke) {
    const TSource source = TSource::FromString(SMOKE_PROGRAM);
    TTokenList tokenList = ParseTokens(source);

    for (std::size_t i = 0; i < TOKEN_KINDS.size(); ++i) {
        EXPECT_EQ(tokenList.Current().Kind, TOKEN_KINDS[i]);
        if (i + 1 < TOKEN_KINDS.size()) {
            EXPECT_TRUE(tokenList.SkipToken());
        } else {
            EXPECT_FALSE(tokenList.SkipToken());
        }
    }
}
