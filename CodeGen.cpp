#include "CodeGen.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

using namespace llvm;



CodeGen::CodeGen() {}

void CodeGen::initialize() {
    TheContext = std::make_unique<LLVMContext>();
    TheModule = std::make_unique<Module>("my cool jit", *TheContext);
    Builder = std::make_unique<IRBuilder<>>(*TheContext);
}

Value* CodeGen::LogErrorV(const char *Str) {
    fprintf(stderr, "Error: %s\n", Str);
    return nullptr;
}

// Implementation of AST codegen methods
Value *NumberExprAST::codegen(CodeGen &CG) {
    return ConstantFP::get(CG.getContext(), APFloat(Val));
}

Value *VariableExprAST::codegen(CodeGen &CG) {
    Value *V = CG.getNamedValues()[Name];
    if (!V)
        return CG.LogErrorV("Unknown variable name");
    return V;
}

Value *BinaryExprAST::codegen(CodeGen &CG) {
    Value *L = LHS->codegen(CG);
    Value *R = RHS->codegen(CG);
    if (!L || !R)
        return nullptr;

    switch (Op) {
    case '+':
        return CG.getBuilder()->CreateFAdd(L, R, "addtmp");
    case '-':
        return CG.getBuilder()->CreateFSub(L, R, "subtmp");
    case '*':
        return CG.getBuilder()->CreateFMul(L, R, "multmp");
    case '<':
        L = CG.getBuilder()->CreateFCmpULT(L, R, "cmptmp");
        // Convert bool 0/1 to double 0.0 or 1.0
        return CG.getBuilder()->CreateUIToFP(L, Type::getDoubleTy(CG.getContext()),
                                                    "booltmp");
    default:
        return CG.LogErrorV("invalid binary operator");
    }
}

Value *CallExprAST::codegen(CodeGen &CG) {
    // Look up the name in the global module table.
    Function *CalleeF = CG.getModule()->getFunction(Callee);
    if (!CalleeF)
        return CG.LogErrorV("Unknown function referenced");

    // If argument mismatch error.
    if (CalleeF->arg_size() != Args.size())
        return CG.LogErrorV("Incorrect # arguments passed");

    std::vector<Value *> ArgsV;
    for (unsigned i = 0, e = Args.size(); i != e; ++i) {
        ArgsV.push_back(Args[i]->codegen(CG));
        if (!ArgsV.back())
            return nullptr;
    }

    return CG.getBuilder()->CreateCall(CalleeF, ArgsV, "calltmp");
}

Function *PrototypeAST::codegen(CodeGen &CG) {
    // Make the function type:  double(double,double) etc.
    std::vector<Type*> Doubles(Args.size(),
                              Type::getDoubleTy(CG.getContext()));
    FunctionType *FT =
        FunctionType::get(Type::getDoubleTy(CG.getContext()), Doubles, false);

    Function *F =
        Function::Create(FT, Function::ExternalLinkage, Name, CG.getModule());

    // Set names for all arguments.
    unsigned Idx = 0;
    for (auto &Arg : F->args())
        Arg.setName(Args[Idx++]);

    return F;
}

Function *FunctionAST::codegen(CodeGen &CG) {
    // First, check for an existing function from a previous 'extern' declaration.
    Function *TheFunction = CG.getModule()->getFunction(Proto->getName());

    if (!TheFunction)
        TheFunction = Proto->codegen(CG);

    if (!TheFunction)
        return nullptr;

    if (!TheFunction->empty())
        return (Function*)CG.LogErrorV("Function cannot be redefined.");

    // Create a new basic block to start insertion into.
    BasicBlock *BB = BasicBlock::Create(CG.getContext(), "entry", TheFunction);
    CG.getBuilder()->SetInsertPoint(BB);

    // Record the function arguments in the NamedValues map.
    CG.getNamedValues().clear();
    for (auto &Arg : TheFunction->args())
        CG.getNamedValues()[std::string(Arg.getName())] = &Arg;

    if (Value *RetVal = Body->codegen(CG)) {
        // Finish off the function.
        CG.getBuilder()->CreateRet(RetVal);

        // Validate the generated code, checking for consistency.
        verifyFunction(*TheFunction);

        return TheFunction;
    }

    // Error reading body, remove function.
    TheFunction->eraseFromParent();
    return nullptr;
} 