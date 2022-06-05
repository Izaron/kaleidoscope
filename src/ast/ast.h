#pragma once

#include <memory>
#include <sstream>
#include <vector>

#include "source.h"

namespace NKaleidoscope::NAst {

// base class for all expression nodes
class TExpr {
public:
    virtual ~TExpr() = default;
    virtual std::string Dump() const = 0;
};

// numeric literals like "1.0"
class TNumberExpr : public TExpr {
public:
    TNumberExpr(double value);
    std::string Dump() const override;

private:
    double Value_;
};

// referencing a variable like "a"
class TVariableExpr : public TExpr {
public:
    TVariableExpr(TSourceRange name);
    std::string Dump() const override;

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
    std::string Dump() const override;

private:
    EOp Op_;
    std::unique_ptr<TExpr> Lhs_;
    std::unique_ptr<TExpr> Rhs_;
};

// function call
class TCallExpr : public TExpr {
public:
    TCallExpr(TSourceRange callee, std::vector<std::unique_ptr<TExpr>> args);
    std::string Dump() const override;

private:
    TSourceRange Callee_;
    std::vector<std::unique_ptr<TExpr>> Args_;
};

// "prototype" of a function (declaration)
class TPrototype {
public:
    TPrototype(TSourceRange name, std::vector<TSourceRange> args);
    std::string Dump() const;
    const TSourceRange& GetName() const;

private:
    TSourceRange Name_;
    std::vector<TSourceRange> Args_;
};

// a function definition (at the same time it is a prototype)
class TFunction {
public:
    TFunction(std::unique_ptr<TPrototype> prototype, std::unique_ptr<TExpr> body);
    std::string Dump() const;

private:
    std::unique_ptr<TPrototype> Prototype_;
    std::unique_ptr<TExpr> Body_;
};

} // namespace NKaleidoscope::NAst
