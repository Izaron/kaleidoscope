#pragma once

#include <memory>

#include "ast.h"
#include "lexer.h"

namespace NKaleidoscope {

class TParser {
public:
    TParser(TTokenList&& tokens);

    // numberexpr ::= number
    std::unique_ptr<NAst::TExpr> ParseNumberExpr();

    // parenexpr ::= '(' expression ')'
    std::unique_ptr<NAst::TExpr> ParseParenExpr();

    // identifierexpr
    //   ::= identifier
    //   ::= identifier '(' expression* ')'
    std::unique_ptr<NAst::TExpr> ParseIdentifierExpr();

    // primary
    //   ::= identifierexpr
    //   ::= numberexpr
    //   ::= parenexpr
    std::unique_ptr<NAst::TExpr> ParsePrimaryExpr();

    // expr ::= primary binoprhs
    std::unique_ptr<NAst::TExpr> ParseExpr();

    // binop ::= '<'|'+'|'-'|'*'
    // binoprhs ::= (binop primary)*
    std::unique_ptr<NAst::TExpr> ParseBinopRhs(int exprPrec, std::unique_ptr<NAst::TExpr> lhs);

    // prototype ::= id '(' id* ')'
    std::unique_ptr<NAst::TPrototype> ParsePrototype();

    // definition ::= 'def' prototype expression
    std::unique_ptr<NAst::TFunction> ParseDefinition();

    // external ::= 'extern' prototype
    std::unique_ptr<NAst::TPrototype> ParseExtern();

private:
    // helper methods
    int GetTokenPrecedence() const;

private:
    TTokenList Tokens_;
};

} // namespace NKaleidoscope
