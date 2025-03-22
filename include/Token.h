#ifndef TOKEN_H
#define TOKEN_H

#include <string>

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

    TOKEN_EOF,      // end of file
    TOKEN_UNKNOWN   // something unexpected appeared
};

std::string TokenTypeToString(TokenType type);

#endif