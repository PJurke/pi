#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

#include "Token.h"

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