#pragma once

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
    Plus,       // +
    Minus,      // -
    Less,       // <
    Greater,    // >
};

// description of every token
struct TToken {
    ETokenKind Kind;
    TSourceRange SourceRange;
};

struct ITokenVisitor {
    virtual void Visit(TToken token) = 0;
};

void ParseTokens(const TSource& source, ITokenVisitor& visitor);

} // namespace NKaleidoscope
