#pragma once

#include "ast.h"

#include <llvm/IR/Function.h>

namespace NKaleidoscope {

enum struct EOptimizationLevel {
    Zero,
    High,
};

class TCodegenVisitor : public NAst::IVisitor {
public:
    TCodegenVisitor(EOptimizationLevel optimizationLevel = EOptimizationLevel::High);
    ~TCodegenVisitor();

    void Visit(const NAst::TNumberExpr&) override;
    void Visit(const NAst::TVariableExpr&) override;
    void Visit(const NAst::TBinaryExpr&) override;
    void Visit(const NAst::TIfExpr&) override;
    void Visit(const NAst::TCallExpr&) override;
    void Visit(const NAst::TPrototype&) override;
    void Visit(const NAst::TFunction&) override;

    const llvm::Value* GetValue() const;
    const llvm::Function* GetFunction() const;

private:
    class TImpl;
    std::unique_ptr<TImpl> Impl_;
};

} // namespace NKaleidoscope
