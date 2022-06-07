#pragma once

#include "ast.h"

namespace NKaleidoscope {

class TCodegenVisitor : public NAst::IVisitor {
public:
    TCodegenVisitor();

    void Visit(const NAst::TNumberExpr&) override;
    void Visit(const NAst::TVariableExpr&) override;
    void Visit(const NAst::TBinaryExpr&) override;
    void Visit(const NAst::TCallExpr&) override;
    void Visit(const NAst::TPrototype&) override;
    void Visit(const NAst::TFunction&) override;

private:
    class TImpl;
    std::unique_ptr<TImpl> Impl_;
};

} // namespace NKaleidoscope
