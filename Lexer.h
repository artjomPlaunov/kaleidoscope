#ifndef KALEIDOSCOPE_LEXER_H
#define KALEIDOSCOPE_LEXER_H

#include <string>

enum Token {
    tok_eof = -1,
    tok_def = -2,
    tok_extern = -3,
    tok_identifier = -4,
    tok_number = -5
};

class Lexer {
public:
    Lexer() : LastChar(' ') {}
    int getNextToken();
    const std::string& getIdentifierStr() const { return IdentifierStr; }
    double getNumVal() const { return NumVal; }
    int getCurrentToken() const { return CurTok; }

private:
    std::string IdentifierStr;
    double NumVal;
    int LastChar;
    int CurTok;
    int gettok();
};

#endif 