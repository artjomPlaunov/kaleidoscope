#include "Parser.h"
#include <cctype>

std::map<char, int> Parser::BinopPrecedence;

std::unique_ptr<ExprAST> Parser::LogError(const char *Str) {
    fprintf(stderr, "Error: %s\n", Str);
    return nullptr;
}

std::unique_ptr<PrototypeAST> Parser::LogErrorP(const char *Str) {
    LogError(Str);
    return nullptr;
}

std::unique_ptr<ExprAST> Parser::ParseNumberExpr() {
    auto Result = std::make_unique<NumberExprAST>(lexer.getNumVal());
    lexer.getNextToken();
    return std::move(Result);
}

std::unique_ptr<ExprAST> Parser::ParseParenExpr() {
    lexer.getNextToken(); // eat (.
    auto V = ParseExpression();
    if (!V)
        return nullptr;

    if (lexer.getCurrentToken() != ')')
        return LogError("expected ')'");
    lexer.getNextToken(); // eat ).
    return V;
}

std::unique_ptr<ExprAST> Parser::ParseIdentifierExpr() {
    std::string IdName = lexer.getIdentifierStr();

    lexer.getNextToken(); // eat identifier.

    if (lexer.getCurrentToken() != '(') // Simple variable ref.
        return std::make_unique<VariableExprAST>(IdName);

    // Call.
    lexer.getNextToken(); // eat (
    std::vector<std::unique_ptr<ExprAST>> Args;
    if (lexer.getCurrentToken() != ')') {
        while (true) {
            if (auto Arg = ParseExpression())
                Args.push_back(std::move(Arg));
            else
                return nullptr;

            if (lexer.getCurrentToken() == ')')
                break;

            if (lexer.getCurrentToken() != ',')
                return LogError("Expected ')' or ',' in argument list");
            lexer.getNextToken();
        }
    }

    // Eat the ')'.
    lexer.getNextToken();

    return std::make_unique<CallExprAST>(IdName, std::move(Args));
}

std::unique_ptr<ExprAST> Parser::ParsePrimary() {
    switch (lexer.getCurrentToken()) {
    default:
        return LogError("unknown token when expecting an expression");
    case tok_identifier:
        return ParseIdentifierExpr();
    case tok_number:
        return ParseNumberExpr();
    case '(':
        return ParseParenExpr();
    }
}

std::unique_ptr<ExprAST> Parser::ParseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> LHS) {
    while (true) {
        int TokPrec = GetTokPrecedence();

        if (TokPrec < ExprPrec)
            return LHS;

        int BinOp = lexer.getCurrentToken();
        lexer.getNextToken(); // eat binop

        auto RHS = ParsePrimary();
        if (!RHS)
            return nullptr;

        int NextPrec = GetTokPrecedence();
        if (TokPrec < NextPrec) {
            RHS = ParseBinOpRHS(TokPrec + 1, std::move(RHS));
            if (!RHS)
                return nullptr;
        }

        LHS = std::make_unique<BinaryExprAST>(BinOp, std::move(LHS), std::move(RHS));
    }
}

std::unique_ptr<ExprAST> Parser::ParseExpression() {
    auto LHS = ParsePrimary();
    if (!LHS)
        return nullptr;

    return ParseBinOpRHS(0, std::move(LHS));
}

std::unique_ptr<PrototypeAST> Parser::ParsePrototype() {
    if (lexer.getCurrentToken() != tok_identifier)
        return LogErrorP("Expected function name in prototype");

    std::string FnName = lexer.getIdentifierStr();
    lexer.getNextToken();

    if (lexer.getCurrentToken() != '(')
        return LogErrorP("Expected '(' in prototype");

    std::vector<std::string> ArgNames;
    while (lexer.getNextToken() == tok_identifier)
        ArgNames.push_back(lexer.getIdentifierStr());
    
    if (lexer.getCurrentToken() != ')')
        return LogErrorP("Expected ')' in prototype");

    lexer.getNextToken(); // eat ).

    return std::make_unique<PrototypeAST>(FnName, std::move(ArgNames));
}

std::unique_ptr<FunctionAST> Parser::ParseDefinition() {
    lexer.getNextToken(); // eat def.
    auto Proto = ParsePrototype();
    if (!Proto)
        return nullptr;

    if (auto E = ParseExpression())
        return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
    return nullptr;
}

std::unique_ptr<FunctionAST> Parser::ParseTopLevelExpr() {
    if (auto E = ParseExpression()) {
        auto Proto = std::make_unique<PrototypeAST>("__anon_expr",
                                                   std::vector<std::string>());
        return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
    }
    return nullptr;
}

std::unique_ptr<PrototypeAST> Parser::ParseExtern() {
    lexer.getNextToken(); // eat extern.
    return ParsePrototype();
}

int Parser::GetTokPrecedence() {
    if (!isascii(lexer.getCurrentToken()))
        return -1;

    int TokPrec = BinopPrecedence[lexer.getCurrentToken()];
    if (TokPrec <= 0)
        return -1;
    return TokPrec;
}