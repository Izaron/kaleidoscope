#include "parser.h"

#include <unordered_map>

namespace NKaleidoscope {

namespace {

const std::unordered_map<ETokenKind, int> BINOP_PRECEDENCE = {
    {ETokenKind::Less, 10},
    {ETokenKind::Plus, 20},
    {ETokenKind::Minus, 20},
    {ETokenKind::Multiply, 40},
};

const std::unordered_map<ETokenKind, NAst::TBinaryExpr::EOp> TOKEN_KIND_TO_BINOP = {
    {ETokenKind::Less, NAst::TBinaryExpr::EOp::Less},
    {ETokenKind::Plus, NAst::TBinaryExpr::EOp::Plus},
    {ETokenKind::Minus, NAst::TBinaryExpr::EOp::Minus},
    {ETokenKind::Multiply, NAst::TBinaryExpr::EOp::Multiply},
};

} // namespace

TParser::TParser(TTokenList&& tokens)
    : Tokens_{std::move(tokens)}
{}

std::unique_ptr<NAst::TExpr> TParser::ParseNumberExpr() {
    const double number = Tokens_.Current().SourceRange.AsDouble();
    Tokens_.SkipToken();
    return std::make_unique<NAst::TNumberExpr>(number);
}

std::unique_ptr<NAst::TExpr> TParser::ParseParenExpr() {
    Tokens_.SkipToken(); // eat '('

    auto expr = ParseExpr(); // parse expression inside brackets

    if (Tokens_.Current().Kind != ETokenKind::RBracket) {
        throw std::runtime_error("Expected ')' symbol");
    }
    Tokens_.SkipToken(); // eat ')'

    return expr;
}

std::unique_ptr<NAst::TExpr> TParser::ParseIdentifierExpr() {
    const TSourceRange idSourceRange = Tokens_.Current().SourceRange;
    Tokens_.SkipToken(); // eat identifier

    if (Tokens_.Current().Kind != ETokenKind::LBracket) {
        // simple variable reference
        return std::make_unique<NAst::TVariableExpr>(idSourceRange);
    }

    // function call
    Tokens_.SkipToken(); // eat '('
    std::vector<std::unique_ptr<NAst::TExpr>> args;
    if (Tokens_.Current().Kind != ETokenKind::RBracket) {
        // there is a non-empty list of arguments
        while (true) {
            args.emplace_back(ParseExpr());

            const auto kind = Tokens_.Current().Kind;
            if (kind == ETokenKind::RBracket) {
                // there are no more arguments
                break;
            }

            // there should be a ','
            if (kind != ETokenKind::Comma) {
                throw std::runtime_error("Expected ',' symbol");
            }
            Tokens_.SkipToken(); // eat ','
        }
    }
    Tokens_.SkipToken(); // eat ')'

    return std::make_unique<NAst::TCallExpr>(idSourceRange, std::move(args));
}

std::unique_ptr<NAst::TExpr> TParser::ParsePrimaryExpr() {
    switch (Tokens_.Current().Kind) {
    case ETokenKind::Identifier:
        return ParseIdentifierExpr();
    case ETokenKind::Number:
        return ParseNumberExpr();
    case ETokenKind::LBracket:
        return ParseParenExpr();
    default:
        throw std::runtime_error("unknown token when expecting an expression");
    }
}

std::unique_ptr<NAst::TExpr> TParser::ParseExpr() {
    auto lhs = ParsePrimaryExpr();
    return ParseBinopRhs(0, std::move(lhs));
}

std::unique_ptr<NAst::TExpr> TParser::ParseBinopRhs(int exprPrec, std::unique_ptr<NAst::TExpr> lhs) {
    while (true) {
        // find the binop's precedence
        int tokPrec = GetTokenPrecedence();

        // if this is a binop that is at least as harder as the current binop,
        // consume it, otherwise we are done
        if (tokPrec < exprPrec) {
            return lhs;
        }

        const ETokenKind binopTokenKind = Tokens_.Current().Kind;
        Tokens_.SkipToken(); // eat binop

        // parse primary expr after the binop
        auto rhs = ParsePrimaryExpr();

        // if this is a binop that is harder than the current binop,
        // read its group as a whole
        int nextPrec = GetTokenPrecedence();
        if (tokPrec < nextPrec) {
            rhs = ParseBinopRhs(tokPrec + 1, std::move(rhs));
        }

        // merge lhs/rhs
        lhs = std::make_unique<NAst::TBinaryExpr>(
            TOKEN_KIND_TO_BINOP.at(binopTokenKind),
            std::move(lhs),
            std::move(rhs)
        );
    }
}

std::unique_ptr<NAst::TPrototype> TParser::ParsePrototype() {
    if (Tokens_.Current().Kind != ETokenKind::Identifier) {
        throw std::runtime_error("Expected function name in prototype");
    }

    const TSourceRange nameSourceRange = Tokens_.Current().SourceRange;
    Tokens_.SkipToken(); // eat the identifier

    if (Tokens_.Current().Kind != ETokenKind::LBracket) {
        throw std::runtime_error("Expected '(' in prototype");
    }
    Tokens_.SkipToken(); // eat '('

    std::vector<TSourceRange> args;
    while (Tokens_.Current().Kind == ETokenKind::Identifier) {
        args.emplace_back(Tokens_.Current().SourceRange);
        Tokens_.SkipToken(); // eat the identifier
    }

    if (Tokens_.Current().Kind != ETokenKind::RBracket) {
        throw std::runtime_error("Expected ')' in prototype");
    }
    Tokens_.SkipToken(); // eat ')'

    return std::make_unique<NAst::TPrototype>(nameSourceRange, std::move(args));
}

std::unique_ptr<NAst::TFunction> TParser::ParseDefinition() {
    Tokens_.SkipToken(); // eat 'def'
    auto prototype = ParsePrototype();
    auto expr = ParseExpr();
    return std::make_unique<NAst::TFunction>(std::move(prototype), std::move(expr));
}

std::unique_ptr<NAst::TPrototype> TParser::ParseExtern() {
    Tokens_.SkipToken(); // eat 'extern'
    return ParsePrototype();
}

int TParser::GetTokenPrecedence() const {
    const auto kind = Tokens_.Current().Kind;
    if (auto iter = BINOP_PRECEDENCE.find(kind); iter != BINOP_PRECEDENCE.end()) {
        return iter->second;
    }
    return -1;
}

} // namespace NKaleidoscope
