#pragma once

#include <memory>
#include <vector>

#include "source.h"

namespace NKaleidoscope::NAst {

// forward declarations for visitor pattern
class TNumberExpr;
class TVariableExpr;
class TBinaryExpr;
class TCallExpr;
class TPrototype;
class TFunction;

class IVisitor {
public:
    virtual ~IVisitor() = default;
    virtual void Visit(const TNumberExpr&) = 0;
    virtual void Visit(const TVariableExpr&) = 0;
    virtual void Visit(const TBinaryExpr&) = 0;
    virtual void Visit(const TCallExpr&) = 0;
    virtual void Visit(const TPrototype&) = 0;
    virtual void Visit(const TFunction&) = 0;
};

// base class for all nodes
class TNode {
public:
    virtual ~TNode() = default;
    virtual void Accept(IVisitor& v) const = 0;
};

// base class for all expression nodes
class TExpr : public TNode {};

// numeric literals like "1.0"
class TNumberExpr : public TExpr {
public:
    TNumberExpr(double value);
    double GetValue() const;

    void Accept(IVisitor& v) const override { v.Visit(*this); }

private:
    double Value_;
};

// referencing a variable like "a"
class TVariableExpr : public TExpr {
public:
    TVariableExpr(TSourceRange name);
    const TSourceRange& GetName() const;

    void Accept(IVisitor& v) const override { v.Visit(*this); }

private:
    TSourceRange Name_;
};

// a binary operator
class TBinaryExpr : public TExpr {
public:
    enum struct EOp {
        Less,
        Plus,
        Minus,
        Multiply,
    };

public:
    TBinaryExpr(EOp op, std::unique_ptr<TExpr> lhs, std::unique_ptr<TExpr> rhs);
    EOp GetOp() const;
    const TExpr& GetLhs() const;
    const TExpr& GetRhs() const;

    void Accept(IVisitor& v) const override { v.Visit(*this); }

private:
    EOp Op_;
    std::unique_ptr<TExpr> Lhs_;
    std::unique_ptr<TExpr> Rhs_;
};

// function call
class TCallExpr : public TExpr {
public:
    TCallExpr(TSourceRange callee, std::vector<std::unique_ptr<TExpr>> args);
    const TSourceRange& GetCallee() const;
    const std::vector<std::unique_ptr<TExpr>>& GetArgs() const;

    void Accept(IVisitor& v) const override { v.Visit(*this); }

private:
    TSourceRange Callee_;
    std::vector<std::unique_ptr<TExpr>> Args_;
};

// "prototype" of a function (declaration)
class TPrototype : public TNode {
public:
    TPrototype(TSourceRange name, std::vector<TSourceRange> args);
    const TSourceRange& GetName() const;
    const std::vector<TSourceRange>& GetArgs() const;

    void Accept(IVisitor& v) const override { v.Visit(*this); }

private:
    TSourceRange Name_;
    std::vector<TSourceRange> Args_;
};

// a function definition (at the same time it is a prototype)
class TFunction : public TNode {
public:
    TFunction(std::unique_ptr<TPrototype> prototype, std::unique_ptr<TExpr> body);
    const TPrototype& GetPrototype() const;
    const TExpr& GetBody() const;

    void Accept(IVisitor& v) const override { v.Visit(*this); }

private:
    std::unique_ptr<TPrototype> Prototype_;
    std::unique_ptr<TExpr> Body_;
};

} // namespace NKaleidoscope::NAst
