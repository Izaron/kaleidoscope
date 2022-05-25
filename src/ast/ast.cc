#include "ast.h"

namespace NKaleidoscope::NAst {

namespace {

constexpr std::string_view DUMP_CHILD_INDENT = "  ";

char BinaryExprOpToChar(TBinaryExpr::EOp op) {
    using enum TBinaryExpr::EOp;
    switch (op) {
        case Plus: return '+';
        case Minus: return '-';
        default: __builtin_unreachable();
    }
}

} // namespace

// TNumberExpr
TNumberExpr::TNumberExpr(double value)
    : Value_{value}
{}

void TNumberExpr::Dump(std::stringstream& os) {
    os << "NumberExpr: " << Value_;
}

// TVariableExpr
TVariableExpr::TVariableExpr(TSourceRange name)
    : Name_{name}
{}

void TVariableExpr::Dump(std::stringstream& os) {
    os << "VariableExpr: \"" << Name_.AsStringView() << "\"";
}

// TBinaryExpr
TBinaryExpr::TBinaryExpr(EOp op, std::unique_ptr<TExpr> lhs, std::unique_ptr<TExpr> rhs)
    : Op_{op}
    , Lhs_{std::move(lhs)}
    , Rhs_{std::move(rhs)}
{}

void TBinaryExpr::Dump(std::stringstream& os) {
    os << "BinaryExpr: \"" << BinaryExprOpToChar(Op_) << "\"\n";

    std::stringstream lhsOs;
    Lhs_->Dump(lhsOs);
    os << DUMP_CHILD_INDENT << lhsOs.view() << "\n";

    std::stringstream rhsOs;
    Rhs_->Dump(rhsOs);
    os << DUMP_CHILD_INDENT << rhsOs.view();
}

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
