#pragma once

#include <vector>

#include "source.h"

namespace NKaleidoscope {

// the list of all possible tokens
enum struct ETokenKind {
    Invalid,

    // end-of-file
    Eof,

    // commands
    Def,
    Extern,
    
    // primary
    Identifier,
    Number,

    // one-symbol tokens
    LBracket,   // (
    RBracket,   // )
    Comma,      // ,
    Plus,       // +
    Minus,      // -
    Multiply,   // *
    Less,       // <
    Greater,    // >
    Semicolon,  // ;
};

// description of every token
struct TToken {
    ETokenKind Kind;
    TSourceRange SourceRange;
};

// token parsers
class TTokenList {
public:
    TTokenList();

    void AddToken(TToken token);
    const TToken& Current() const;
    bool SkipToken();

private:
    std::vector<TToken> Tokens_;
    std::size_t Index_;
};

struct ITokenVisitor {
    virtual void Visit(TToken token) = 0;
};

void LexTokens(const TSource& source, ITokenVisitor& visitor);
TTokenList LexTokens(const TSource& source);

} // namespace NKaleidoscope
