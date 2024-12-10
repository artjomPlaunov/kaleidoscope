#include "KaleidoscopeDriver.h"
#include "llvm/Support/raw_ostream.h"

KaleidoscopeDriver::KaleidoscopeDriver() 
    : parser(lexer, codeGen) {
    // Initialize operator precedence
    binopPrecedence['<'] = 10;
    binopPrecedence['+'] = 20;
    binopPrecedence['-'] = 20;
    binopPrecedence['*'] = 40;
}

void KaleidoscopeDriver::initialize() {
    codeGen.initialize();
    lexer.getNextToken(); // Prime the first token
}

void KaleidoscopeDriver::handleDefinition() {
    if (auto FnAST = parser.parseDefinition()) {
        if (auto *FnIR = FnAST->codegen(codeGen)) {
            fprintf(stderr, "Read function definition:");
            FnIR->print(llvm::errs());
            fprintf(stderr, "\n");
        }
    } else {
        lexer.getNextToken();
    }
}

// Implement handleExtern() and handleTopLevelExpression() similarly...

void KaleidoscopeDriver::mainLoop() {
    while (true) {
        fprintf(stderr, "ready> ");
        switch (lexer.getCurrentToken()) {
            case tok_eof: return;
            case ';': 
                lexer.getNextToken(); 
                break;
            case tok_def:
                handleDefinition();
                break;
            case tok_extern:
                handleExtern();
                break;
            default:
                handleTopLevelExpression();
                break;
        }
    }
} 