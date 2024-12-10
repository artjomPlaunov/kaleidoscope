#ifndef KALEIDOSCOPE_DRIVER_H
#define KALEIDOSCOPE_DRIVER_H

#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"

class KaleidoscopeDriver {
public:
    KaleidoscopeDriver();
    void initialize();
    void mainLoop();
    void handleDefinition();
    void handleExtern();
    void handleTopLevelExpression();

private:
    Lexer lexer;
    CodeGen codeGen;
    Parser parser;
    std::map<char, int> binopPrecedence;
};

#endif 