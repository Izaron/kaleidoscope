#include "ast.h"

namespace NKaleidoscope::NAst {

// TNumberExpr
TNumberExpr::TNumberExpr(double value)
    : Value_{value}
{}

double TNumberExpr::GetValue() const {
    return Value_;
}

// TVariableExpr
TVariableExpr::TVariableExpr(TSourceRange name)
    : Name_{name}
{}

const TSourceRange& TVariableExpr::GetName() const {
    return Name_;
}

// TBinaryExpr
TBinaryExpr::TBinaryExpr(EOp op, std::unique_ptr<TExpr> lhs, std::unique_ptr<TExpr> rhs)
    : Op_{op}
    , Lhs_{std::move(lhs)}
    , Rhs_{std::move(rhs)}
{}

TBinaryExpr::EOp TBinaryExpr::GetOp() const {
    return Op_;
}

const TExpr& TBinaryExpr::GetLhs() const {
    return *Lhs_;
}

const TExpr& TBinaryExpr::GetRhs() const {
    return *Rhs_;
}

// TCallExpr
TCallExpr::TCallExpr(TSourceRange callee, std::vector<std::unique_ptr<TExpr>> args)
    : Callee_{callee}
    , Args_{std::move(args)}
{}

const TSourceRange& TCallExpr::GetCallee() const {
    return Callee_;
}

const std::vector<std::unique_ptr<TExpr>>& TCallExpr::GetArgs() const {
    return Args_;
}

// TPrototype
TPrototype::TPrototype(TSourceRange name, std::vector<TSourceRange> args)
    : Name_{name}
    , Args_{std::move(args)}
{}

const TSourceRange& TPrototype::GetName() const {
    return Name_;
}

const std::vector<TSourceRange>& TPrototype::GetArgs() const {
    return Args_;
}

// TFunction
TFunction::TFunction(std::unique_ptr<TPrototype> prototype, std::unique_ptr<TExpr> body)
    : Prototype_{std::move(prototype)}
    , Body_{std::move(body)}
{}

const TPrototype& TFunction::GetPrototype() const {
    return *Prototype_;
}

const TExpr& TFunction::GetBody() const {
    return *Body_;
}

} // namespace NKaleidoscope::NAst
