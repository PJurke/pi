#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

#include "Token.h"

struct Token {
    TokenType type;
    std::string lexeme;
    int line;               // Line number where the token was found
    int column;             // Column number where the token was found
};

class Lexer {
public:
    Lexer(const std::string &source);
    std::vector<Token> tokenize();
private:
    std::string source;
    size_t index;
    
    int line;   // Current line number
    int column; // Current column number

    char currentChar();
    void advance();
};

#endif