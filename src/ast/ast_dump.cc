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
std::string TNumberExpr::Dump() const {
    std::stringstream ss;
    ss << "NumberExpr: " << Value_ << "\n";
    return ss.str();
}

// TVariableExpr
std::string TVariableExpr::Dump() const {
    std::stringstream ss;
    ss << "VariableExpr: \"" << Name_.AsStringView() << "\"\n";
    return ss.str();
}

// TBinaryExpr
std::string TBinaryExpr::Dump() const {
    std::stringstream ss;
    ss << "BinaryExpr: \"" << BinaryExprOpToChar(Op_) << "\"\n";
    ss << DUMP_CHILD_INDENT << Lhs_->Dump();
    ss << DUMP_CHILD_INDENT << Rhs_->Dump();
    return ss.str();
}

// TCallExpr
std::string TCallExpr::Dump() const {
    std::stringstream ss;
    ss << "CallExpr: \"" << Callee_.AsStringView() << "\"\n";
    for (const auto& arg : Args_) {
        ss << DUMP_CHILD_INDENT << arg->Dump();
    }
    return ss.str();
}

// TPrototype
std::string TPrototype::Dump() const {
    std::stringstream ss;
    ss << "Prototype: \"" << Name_.AsStringView() << "\", args: ";
    for (std::size_t i = 0; i < Args_.size(); ++i) {
        ss << "\"" << Args_[i].AsStringView() << "\"";
        if (i != Args_.size() - 1) {
            ss << ", ";
        } else {
            ss << "\n";
        }
    }
    return ss.str();
}

// TFunction
std::string TFunction::Dump() const {
    std::stringstream ss;
    ss << "Function definition: \n";
    ss << DUMP_CHILD_INDENT << Prototype_->Dump();
    ss << DUMP_CHILD_INDENT << Body_->Dump();
    return ss.str();
}

} // namespace NKaleidoscope::NAst
