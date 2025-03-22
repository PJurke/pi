#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include "Token.h"

struct Token {
    TokenType type;
    std::string lexeme;
};

class Lexer {
public:
    Lexer(const std::string &source);
    std::vector<Token> tokenize();
private:
    std::string source;
    size_t index;
    char currentChar();
    void advance();
};

#endif