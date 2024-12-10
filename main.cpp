#include "KaleidoscopeDriver.h"
#include "CodeGen.h"
#include "llvm/Support/raw_ostream.h"

static void HandleDefinition(Parser &parser, CodeGen &codeGen) {
    if (auto FnAST = parser.ParseDefinition()) {
        if (auto *FnIR = FnAST->codegen(codeGen)) {
            fprintf(stderr, "Read function definition:");
            FnIR->print(llvm::errs());
            fprintf(stderr, "\n");
        }
    }
}

static void HandleExtern(Parser &parser, CodeGen &codeGen) {
    if (auto ProtoAST = parser.ParseExtern()) {
        if (auto *FnIR = ProtoAST->codegen(codeGen)) {
            fprintf(stderr, "Read extern: ");
            FnIR->print(llvm::errs());
            fprintf(stderr, "\n");
        }
    }
}

static void HandleTopLevelExpression(Parser &parser, CodeGen &codeGen) {
    if (auto FnAST = parser.ParseTopLevelExpr()) {
        if (auto *FnIR = FnAST->codegen(codeGen)) {
            fprintf(stderr, "Read top-level expression:");
            FnIR->print(llvm::errs());
            fprintf(stderr, "\n");
            FnIR->eraseFromParent();
        }
    }
}

int main() {
    // Initialize operator precedence
    Parser::BinopPrecedence['<'] = 10;
    Parser::BinopPrecedence['+'] = 20;
    Parser::BinopPrecedence['-'] = 20;
    Parser::BinopPrecedence['*'] = 40;

    // Initialize lexer and parser
    Lexer lexer;
    Parser parser(lexer);
    
    // Initialize code generator
    CodeGen codeGen;
    codeGen.initialize();

    fprintf(stderr, "ready> ");
    lexer.getNextToken();

    // Main REPL loop
    while (true) {
        fprintf(stderr, "ready> ");
        switch (lexer.getCurrentToken()) {
            case tok_eof: 
                return 0;
            case ';':
                lexer.getNextToken();
                break;
            case tok_def:
                HandleDefinition(parser, codeGen);
                break;
            case tok_extern:
                HandleExtern(parser, codeGen);
                break;
            default:
                HandleTopLevelExpression(parser, codeGen);
                break;
        }
    }

    return 0;
} 