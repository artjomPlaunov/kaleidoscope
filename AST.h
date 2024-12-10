#ifndef KALEIDOSCOPE_AST_H
#define KALEIDOSCOPE_AST_H

#include "llvm/IR/Value.h"
#include <memory>
#include <string>
#include <vector>

class CodeGen;

class ExprAST {
public:
    virtual ~ExprAST() = default;
    virtual llvm::Value *codegen(CodeGen &CG) = 0;
};

class NumberExprAST : public ExprAST {
    double Val;
public:
    NumberExprAST(double Val) : Val(Val) {}
    llvm::Value *codegen(CodeGen &CG) override;
};

class VariableExprAST : public ExprAST {
    std::string Name;
public:
    VariableExprAST(const std::string &Name) : Name(Name) {}
    llvm::Value *codegen(CodeGen &CG) override;
};

class BinaryExprAST : public ExprAST {
    char Op;
    std::unique_ptr<ExprAST> LHS, RHS;
public:
    BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
                  std::unique_ptr<ExprAST> RHS)
        : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
    llvm::Value *codegen(CodeGen &CG) override;
};

class CallExprAST : public ExprAST {
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;
public:
    CallExprAST(const std::string &Callee,
                std::vector<std::unique_ptr<ExprAST>> Args)
        : Callee(Callee), Args(std::move(Args)) {}
    llvm::Value *codegen(CodeGen &CG) override;
};

class PrototypeAST {
    std::string Name;
    std::vector<std::string> Args;
public:
    PrototypeAST(const std::string &Name, std::vector<std::string> Args)
        : Name(Name), Args(std::move(Args)) {}
    llvm::Function *codegen(CodeGen &CG);
    const std::string &getName() const { return Name; }
};

class FunctionAST {
    std::unique_ptr<PrototypeAST> Proto;
    std::unique_ptr<ExprAST> Body;
public:
    FunctionAST(std::unique_ptr<PrototypeAST> Proto,
                std::unique_ptr<ExprAST> Body)
        : Proto(std::move(Proto)), Body(std::move(Body)) {}
    llvm::Function *codegen(CodeGen &CG);
};

#endif 