#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

// Token types for pi language
enum TokenType {
    TOKEN_FUNC,     // function

    TOKEN_START,    // start function
    TOKEN_IDENT,    // function identifier
    TOKEN_PRINT,    // print function (provisional)

    TOKEN_LPAREN,   // (
    TOKEN_RPAREN,   // )

    TOKEN_LBRACE,   // {
    TOKEN_RBRACE,   // }

    TOKEN_ARROW,    // result type of a function

    TOKEN_INT32,    // data type integer, 32 bit
    TOKEN_STRING,   // data type string

    TOKEN_EOF,
    TOKEN_UNKNOWN
};

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