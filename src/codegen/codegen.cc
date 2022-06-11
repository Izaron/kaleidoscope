#include "codegen.h"

#include <map>
#include <stack>

#include <llvm/Pass.h>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>

namespace NKaleidoscope {

namespace {

llvm::legacy::FunctionPassManager ConstructFunctionPassManager(llvm::Module& module,
                                                               EOptimizationLevel optimizationLevel)
{
    llvm::legacy::FunctionPassManager manager{&module};

    if (optimizationLevel == EOptimizationLevel::High) {
        // simple optimizations
        manager.add(llvm::createInstructionCombiningPass());

        // reassociate expressions
        manager.add(llvm::createReassociatePass());

        // eliminate common subexpressions
        manager.add(llvm::createNewGVNPass());

        // simplify the control flow graph
        manager.add(llvm::createCFGSimplificationPass());
    }

    manager.doInitialization();
    return manager;
}

} // namespace

// TCodegenVisitor::TImpl
class TCodegenVisitor::TImpl {
public:
    TImpl(TCodegenVisitor& visitor, EOptimizationLevel optimizationLevel)
        : Visitor_{visitor}
        , Builder_{Context_}
        , Module_{"cool_module", Context_}
        , FunctionPassManager_{ConstructFunctionPassManager(Module_, optimizationLevel)}
    {
    }

    void Visit(const NAst::TNumberExpr& numberExpr) {
        const llvm::APFloat val{numberExpr.GetValue()};
        Value_ = llvm::ConstantFP::get(Context_, val);
    }

    void Visit(const NAst::TVariableExpr& variableExpr) {
        const std::string_view name = variableExpr.GetName().AsStringView();
        auto iter = NamedValues_.find(name);
        if (iter == NamedValues_.end()) {
            throw std::runtime_error("Expected known named value, found \"" + std::string{name} + "\"");
        }
        Value_ = iter->second;
    }

    void Visit(const NAst::TBinaryExpr& binaryExpr) {
        binaryExpr.GetLhs().Accept(Visitor_);
        llvm::Value* lhsValue = Value_;

        binaryExpr.GetRhs().Accept(Visitor_);
        llvm::Value* rhsValue = Value_;

        using enum NAst::TBinaryExpr::EOp;
        switch (binaryExpr.GetOp()) {
            case Less:
                Value_ = Builder_.CreateFCmpULT(lhsValue, rhsValue, "cmptmp");
                Value_ = Builder_.CreateUIToFP(Value_, llvm::Type::getDoubleTy(Context_));
                break;
            case Plus:
                Value_ = Builder_.CreateFAdd(lhsValue, rhsValue, "addtmp");
                break;
            case Minus:
                Value_ = Builder_.CreateFSub(lhsValue, rhsValue, "subtmp");
                break;
            case Multiply:
                Value_ = Builder_.CreateFMul(lhsValue, rhsValue, "multmp");
                break;
        }
    }

    void Visit(const NAst::TIfExpr& ifExpr) {
        ifExpr.GetCond().Accept(Visitor_);
        llvm::Value* condValue = Value_;

        // convert condition to a bool by comparing non-equal to 0.0
        condValue = Builder_.CreateFCmpONE(condValue,
                                           llvm::ConstantFP::get(Context_, llvm::APFloat{0.0}),
                                           "ifcond");

        // create blocks for then/else cases
        llvm::Function* func = Builder_.GetInsertBlock()->getParent();
        llvm::BasicBlock* thenBlock = llvm::BasicBlock::Create(Context_, "then", func);
        llvm::BasicBlock* elseBlock = llvm::BasicBlock::Create(Context_, "else");
        llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(Context_, "ifcont");

        Builder_.CreateCondBr(condValue, thenBlock, elseBlock);

        // emit 'then' block
        Builder_.SetInsertPoint(thenBlock);
        ifExpr.GetThen().Accept(Visitor_);
        llvm::Value* thenValue = Value_;
        Builder_.CreateBr(mergeBlock); // unconditional branch
        thenBlock = Builder_.GetInsertBlock();

        // emit 'else' block
        func->getBasicBlockList().push_back(elseBlock);
        Builder_.SetInsertPoint(elseBlock);
        ifExpr.GetElse().Accept(Visitor_);
        llvm::Value* elseValue = Value_;
        Builder_.CreateBr(mergeBlock); // unconditional branch
        elseBlock = Builder_.GetInsertBlock();

        // emit merge block
        func->getBasicBlockList().push_back(mergeBlock);
        Builder_.SetInsertPoint(mergeBlock);
        llvm::PHINode* phiNode = Builder_.CreatePHI(llvm::Type::getDoubleTy(Context_), 2, "iftmp");
        phiNode->addIncoming(thenValue, thenBlock);
        phiNode->addIncoming(elseValue, elseBlock);

        Value_ = phiNode;
    }

    void Visit(const NAst::TCallExpr& callExpr) {
        // lookup callee function in the module
        const std::string_view calleeName = callExpr.GetCallee().AsStringView();
        llvm::Function* calleeFunction = Module_.getFunction(calleeName);
        if (!calleeFunction) {
            throw std::runtime_error("Unknown function \"" + std::string{calleeName} + "\"");
        }

        // check arguments count
        const auto& args = callExpr.GetArgs();
        if (calleeFunction->arg_size() != args.size()) {
            throw std::runtime_error("Incorrect number of arguments");
        }

        // build call
        std::vector<llvm::Value*> argsValues;
        for (const auto& arg : args) {
            arg->Accept(Visitor_);
            argsValues.emplace_back(Value_);
        }
        Value_ = Builder_.CreateCall(calleeFunction, argsValues, "calltmp");
    }

    void Visit(const NAst::TPrototype& prototype) {
        const std::string_view name = prototype.GetName().AsStringView();
        const auto& prototypeArgs = prototype.GetArgs();

        std::vector<llvm::Type*> doubles{prototypeArgs.size(), llvm::Type::getDoubleTy(Context_)};
        llvm::FunctionType* functionType = llvm::FunctionType::get(
            llvm::Type::getDoubleTy(Context_), doubles, /* isVarArg = */ false);

        Function_ = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, name, Module_);

        // set names for all arguments
        std::size_t idx = 0;
        for (auto& arg : Function_->args()) {
            arg.setName(prototypeArgs[idx++].AsStringView());
        }
    }

    void Visit(const NAst::TFunction& function) {
        const std::string_view funcName = function.GetPrototype().GetName().AsStringView();
        llvm::Function* func = Module_.getFunction(funcName);
        if (!func) {
            function.GetPrototype().Accept(Visitor_);
            func = Function_;
        }
        if (!func->empty()) {
            throw std::runtime_error("Can't redefine function \"" + std::string{funcName} + "\"");
        }

        // create BBs for body
        llvm::BasicBlock* basicBlock = llvm::BasicBlock::Create(Context_, "entry", func);
        Builder_.SetInsertPoint(basicBlock);

        // record function arguments' names
        NamedValues_.clear();
        for (auto& arg : func->args()) {
            NamedValues_[arg.getName()] = &arg;
        }

        // return expression value
        function.GetBody().Accept(Visitor_);
        Builder_.CreateRet(Value_);
        llvm::verifyFunction(*func);
        FunctionPassManager_.run(*func);
    }

    const llvm::Value* GetValue() const { return Value_; }
    const llvm::Function* GetFunction() const { return Function_; }

    llvm::Module& GetModule() { return Module_; }

private:
    // base visitor
    TCodegenVisitor& Visitor_;

    // llvm classes
    llvm::LLVMContext Context_;
    llvm::IRBuilder<> Builder_;
    llvm::Module Module_;
    llvm::legacy::FunctionPassManager FunctionPassManager_;
    std::map<std::string_view, llvm::Value*, std::less<>> NamedValues_;

    // visitor's values
    llvm::Value* Value_;
    llvm::Function* Function_;
};

// TCodegenVisitor
TCodegenVisitor::TCodegenVisitor(EOptimizationLevel optimizationLevel)
    : Impl_{std::make_unique<TImpl>(*this, optimizationLevel)}
{
}

TCodegenVisitor::~TCodegenVisitor()
{
}

void TCodegenVisitor::Visit(const NAst::TNumberExpr& numberExpr) { Impl_->Visit(numberExpr); }
void TCodegenVisitor::Visit(const NAst::TVariableExpr& variableExpr) { Impl_->Visit(variableExpr); }
void TCodegenVisitor::Visit(const NAst::TBinaryExpr& binaryExpr) { Impl_->Visit(binaryExpr); }
void TCodegenVisitor::Visit(const NAst::TIfExpr& ifExpr) { Impl_->Visit(ifExpr); }
void TCodegenVisitor::Visit(const NAst::TCallExpr& callExpr) { Impl_->Visit(callExpr); }
void TCodegenVisitor::Visit(const NAst::TPrototype& prototype) { Impl_->Visit(prototype); }
void TCodegenVisitor::Visit(const NAst::TFunction& function) { Impl_->Visit(function); }

const llvm::Value* TCodegenVisitor::GetValue() const { return Impl_->GetValue(); }
const llvm::Function* TCodegenVisitor::GetFunction() const { return Impl_->GetFunction(); }

llvm::Module& TCodegenVisitor::GetModule() { return Impl_->GetModule(); }

} // namespace NKaleidoscope
