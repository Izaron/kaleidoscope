#include "lexer.h"

namespace NKaleidoscope {

namespace {

bool IsDigitDot(auto c) {
    return std::isdigit(c) || c == '.';
}

ETokenKind CharToToken(char c) {
    switch (c) {
    case '(':
        return ETokenKind::LBracket;
    case ')':
        return ETokenKind::RBracket;
    case ',':
        return ETokenKind::Comma;
    case '+':
        return ETokenKind::Plus;
    case '-':
        return ETokenKind::Minus;
    case '*':
        return ETokenKind::Multiply;
    case '<':
        return ETokenKind::Less;
    case '>':
        return ETokenKind::Greater;
    case ';':
        return ETokenKind::Semicolon;
    default:
        return ETokenKind::Invalid;
    }
}

TToken LexToken(const TSource& source, std::size_t& offset) {
    std::string_view buffer = source.GetBuffer();
    while (offset < buffer.length() && std::isspace(buffer[offset])) {
        ++offset;
    }

    // check if we have reached EOF
    if (offset >= buffer.length()) {
        return TToken{
            .Kind = ETokenKind::Eof,
            .SourceRange = TSourceRange{.Source = &source, .Offset = buffer.length(), .Length = 0},
        };
    }

    const std::size_t beginOffset = offset;

    // identifier: [a-zA-Z][a-zA-Z0-9]*
    if (std::isalpha(buffer[offset])) {
        while (offset < buffer.length() && std::isalnum(buffer[offset])) {
            ++offset;
        }
        const std::size_t tokenLength = offset - beginOffset;

        // calculate token kind
        const std::string_view identifierStr = buffer.substr(beginOffset, tokenLength);
        ETokenKind kind = ETokenKind::Identifier;
        if (identifierStr == "def") {
            kind = ETokenKind::Def;
        } else if (identifierStr == "extern") {
            kind = ETokenKind::Extern;
        } else if (identifierStr == "if") {
            kind = ETokenKind::If;
        } else if (identifierStr == "then") {
            kind = ETokenKind::Then;
        } else if (identifierStr == "else") {
            kind = ETokenKind::Else;
        }

        // return token
        return TToken{
            .Kind = kind,
            .SourceRange = TSourceRange{.Source = &source, .Offset = beginOffset, .Length = tokenLength},
        };
    }

    // number: [0-9.]+
    if (IsDigitDot(buffer[offset])) {
        while (offset < buffer.length() && IsDigitDot(buffer[offset])) {
            ++offset;
        }
        const std::size_t tokenLength = offset - beginOffset;

        // return token
        return TToken{
            .Kind = ETokenKind::Number,
            .SourceRange = TSourceRange{.Source = &source, .Offset = beginOffset, .Length = tokenLength},
        };
    }

    // comment: '#' and until end of line
    if (buffer[offset] == '#') {
        do {
            ++offset;
        } while (offset < buffer.length() && buffer[offset] != '\n' && buffer[offset] != '\n');

        return LexToken(source, offset);
    }

    // map the current character to the token
    char ch = buffer[offset++];
    return TToken{
        .Kind = CharToToken(ch),
        .SourceRange = TSourceRange{.Source = &source, .Offset = beginOffset, .Length = 1},
    };
}

} // namespace

void LexTokens(const TSource& source, ITokenVisitor& visitor) {
    std::size_t offset = 0;
    while (true) {
        TToken token = LexToken(source, offset);
        bool isEof = token.Kind == ETokenKind::Eof;
        visitor.Visit(std::move(token));
        if (isEof) {
            break;
        }
    }
}

// token parsers
TTokenList::TTokenList()
    : Index_{0}
{
}

void TTokenList::AddToken(TToken token) {
    Tokens_.emplace_back(std::move(token));
}

const TToken& TTokenList::Current() const {
    return Tokens_[Index_];
}

bool TTokenList::SkipToken() {
    if (Index_ + 1 < Tokens_.size()) {
        ++Index_;
        return true;
    }
    return false;
}

TTokenList LexTokens(const TSource& source) {
    TTokenList tokenList;

    struct TTokenVisitor final : ITokenVisitor {
        TTokenList* TokenList;
        TTokenVisitor(TTokenList* tokenList) : TokenList{tokenList} {}

        void Visit(TToken token) override {
            TokenList->AddToken(std::move(token));
        }
    } visitor{&tokenList};
    LexTokens(source, visitor);

    return tokenList;
}

} // namespace NKaleidoscope
