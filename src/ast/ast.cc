#include "ast.h"

namespace NKaleidoscope::NAst {

// TNumberExpr
TNumberExpr::TNumberExpr(double value)
    : Value_{value}
{}

// TVariableExpr
TVariableExpr::TVariableExpr(TSourceRange name)
    : Name_{name}
{}

// TBinaryExpr
TBinaryExpr::TBinaryExpr(EOp op, std::unique_ptr<TExpr> lhs, std::unique_ptr<TExpr> rhs)
    : Op_{op}
    , Lhs_{std::move(lhs)}
    , Rhs_{std::move(rhs)}
{}

// TCallExpr
TCallExpr::TCallExpr(TSourceRange callee, std::vector<std::unique_ptr<TExpr>> args)
    : Callee_{callee}
    , Args_{std::move(args)}
{}

// TPrototype
TPrototype::TPrototype(TSourceRange name, std::vector<TSourceRange> args)
    : Name_{name}
    , Args_{std::move(args)}
{}

const TSourceRange& TPrototype::GetName() const {
    return Name_;
}

// TFunction
TFunction::TFunction(std::unique_ptr<TPrototype> prototype, std::unique_ptr<TExpr> body)
    : Prototype_{std::move(prototype)}
    , Body_{std::move(body)}
{}

} // namespace NKaleidoscope::NAst
