#pragma once

#include "ast.h"

namespace NKaleidoscope {

class TDumpVisitor : public NAst::IVisitor {
public:
    void Visit(const NAst::TNumberExpr&) override;
    void Visit(const NAst::TVariableExpr&) override;
    void Visit(const NAst::TBinaryExpr&) override;
    void Visit(const NAst::TIfExpr&) override;
    void Visit(const NAst::TCallExpr&) override;
    void Visit(const NAst::TPrototype&) override;
    void Visit(const NAst::TFunction&) override;

    const std::string& GetDump() const;

private:
    std::string Dump_;
};

std::string Dump(const NAst::TNode& node);

} // namespace NKaleidoscope
