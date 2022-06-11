#include "dump.h"

#include <sstream>

using namespace NKaleidoscope::NAst;

namespace NKaleidoscope {

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

void TDumpVisitor::Visit(const TNumberExpr& numberExpr) {
    std::stringstream ss;
    ss << "NumberExpr: " << numberExpr.GetValue() << "\n";
    Dump_ = ss.str();
}

void TDumpVisitor::Visit(const TVariableExpr& variableExpr) {
    std::stringstream ss;
    ss << "VariableExpr: \"" << variableExpr.GetName().AsStringView() << "\"\n";
    Dump_ = ss.str();
}

void TDumpVisitor::Visit(const TBinaryExpr& binaryExpr) {
    std::stringstream ss;
    ss << "BinaryExpr: \"" << BinaryExprOpToChar(binaryExpr.GetOp()) << "\"\n";
    ss << Indent(Dump(binaryExpr.GetLhs()));
    ss << Indent(Dump(binaryExpr.GetRhs()));
    Dump_ = ss.str();
}

void TDumpVisitor::Visit(const NAst::TIfExpr& ifExpr) {
    std::stringstream ss;
    ss << "IfExpr:\n";
    ss << "Cond:\n";
    ss << Indent(Dump(ifExpr.GetCond()));
    ss << "Then:\n";
    ss << Indent(Dump(ifExpr.GetThen()));
    ss << "Else:\n";
    ss << Indent(Dump(ifExpr.GetElse()));
    Dump_ = ss.str();
}

void TDumpVisitor::Visit(const TCallExpr& callExpr) {
    std::stringstream ss;
    ss << "CallExpr: \"" << callExpr.GetCallee().AsStringView() << "\"\n";
    for (const auto& arg : callExpr.GetArgs()) {
        ss << Indent(Dump(*arg));
    }
    Dump_ = ss.str();
}

void TDumpVisitor::Visit(const TPrototype& prototype) {
    std::stringstream ss;
    ss << "Prototype: \"" << prototype.GetName().AsStringView() << "\", args: ";
    const auto& args = prototype.GetArgs();
    for (std::size_t i = 0; i < args.size(); ++i) {
        ss << "\"" << args[i].AsStringView() << "\"";
        if (i != args.size() - 1) {
            ss << ", ";
        } else {
            ss << "\n";
        }
    }
    Dump_ = ss.str();
}

void TDumpVisitor::Visit(const TFunction& function) {
    std::stringstream ss;
    ss << "Function definition: \n";
    ss << Indent(Dump(function.GetPrototype()));
    ss << Indent(Dump(function.GetBody()));
    Dump_ = ss.str();
}

const std::string& TDumpVisitor::GetDump() const {
    return Dump_;
}

std::string Dump(const TNode& node) {
    TDumpVisitor visitor;
    node.Accept(visitor);
    return visitor.GetDump();
}

} // namespace NKaleidoscope
