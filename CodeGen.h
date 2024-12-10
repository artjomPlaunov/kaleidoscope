#ifndef KALEIDOSCOPE_CODEGEN_H
#define KALEIDOSCOPE_CODEGEN_H

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "AST.h"
#include <map>
#include <memory>

class CodeGen {
public:
    CodeGen();
    void initialize();
    
    llvm::Value* LogErrorV(const char *Str);
    llvm::LLVMContext& getContext() { return *TheContext; }
    llvm::Module* getModule() { return TheModule.get(); }
    llvm::IRBuilder<>* getBuilder() { return Builder.get(); }
    std::map<std::string, llvm::Value*>& getNamedValues() { return NamedValues; }

private:
    std::unique_ptr<llvm::LLVMContext> TheContext;
    std::unique_ptr<llvm::Module> TheModule;
    std::unique_ptr<llvm::IRBuilder<>> Builder;
    std::map<std::string, llvm::Value*> NamedValues;
};

#endif 