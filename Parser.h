#ifndef KALEIDOSCOPE_PARSER_H
#define KALEIDOSCOPE_PARSER_H

#include "AST.h"
#include "Lexer.h"
#include <map>
#include <memory>

class Parser {
public:
    static std::map<char, int> BinopPrecedence;
    
    Parser(Lexer &lex) : lexer(lex) {}

    // Public parsing interface
    std::unique_ptr<ExprAST> ParseExpression();
    std::unique_ptr<PrototypeAST> ParsePrototype();
    std::unique_ptr<FunctionAST> ParseDefinition();
    std::unique_ptr<PrototypeAST> ParseExtern();
    std::unique_ptr<FunctionAST> ParseTopLevelExpr();

private:
    Lexer &lexer;

    // Error handling helpers
    std::unique_ptr<ExprAST> LogError(const char *Str);
    std::unique_ptr<PrototypeAST> LogErrorP(const char *Str);

    // Parsing helpers
    std::unique_ptr<ExprAST> ParseNumberExpr();
    std::unique_ptr<ExprAST> ParseUnaryExpr();
    std::unique_ptr<ExprAST> ParseParenExpr();
    std::unique_ptr<ExprAST> ParseIdentifierExpr();
    std::unique_ptr<ExprAST> ParsePrimary();
    std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> LHS);
    int GetTokPrecedence();
};

#endif 