#include "ast.h"

namespace NKaleidoscope::NAst {

namespace {

constexpr std::string_view DUMP_CHILD_INDENT = "  ";

char BinaryExprOpToChar(TBinaryExpr::EOp op) {
    using enum TBinaryExpr::EOp;
    switch (op) {
        case Less: return '<';
        case Plus: return '+';
        case Minus: return '-';
        case Multiply: return '*';
        default: __builtin_unreachable();
    }
}

std::string Indent(std::string s) {
    std::stringstream result;
    result << DUMP_CHILD_INDENT;
    for (std::size_t i = 0; i < s.size(); ++i) {
        result << s[i];
        if (s[i] == '\n' && i != s.size() - 1) {
            result << DUMP_CHILD_INDENT;
        }
    }
    return result.str();
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
    ss << Indent(Lhs_->Dump());
    ss << Indent(Rhs_->Dump());
    return ss.str();
}

// TCallExpr
std::string TCallExpr::Dump() const {
    std::stringstream ss;
    ss << "CallExpr: \"" << Callee_.AsStringView() << "\"\n";
    for (const auto& arg : Args_) {
        ss << Indent(arg->Dump());
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
    ss << Indent(Prototype_->Dump());
    ss << Indent(Body_->Dump());
    return ss.str();
}

} // namespace NKaleidoscope::NAst
