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
void TNumberExpr::Dump(std::stringstream& os) {
    os << "NumberExpr: " << Value_;
}

// TVariableExpr
void TVariableExpr::Dump(std::stringstream& os) {
    os << "VariableExpr: \"" << Name_.AsStringView() << "\"";
}

// TBinaryExpr
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

// TPrototype

// TFunction

} // namespace NKaleidoscope::NAst
