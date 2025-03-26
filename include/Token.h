#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum TokenType {
    TOKEN_FUNC,     // function

    TOKEN_START,    // start function
    TOKEN_IDENT,    // function identifier
    TOKEN_PRINT,    // print function (provisional)
    TOKEN_CONST,    // constant

    TOKEN_LPAREN,   // (
    TOKEN_RPAREN,   // )

    TOKEN_LBRACE,   // {
    TOKEN_RBRACE,   // }

    TOKEN_COLON,    // :
    TOKEN_ASSIGN,   // =
    TOKEN_ARROW,    // indicate a return type of a function

    // CHARACTER TYPES
    TOKEN_CHAR8,
    TOKEN_CHAR16,
    TOKEN_CHAR32,

    // INTEGER TYPES
    TOKEN_INT8,
    TOKEN_INT16,
    TOKEN_INT32,
    TOKEN_INT64,
    
    TOKEN_CHAR,     // char data type
    TOKEN_STRING,   // data type string
    TOKEN_NUMBER,   // data type number

    TOKEN_EOF,      // end of file
    TOKEN_UNKNOWN   // something unexpected appeared
};

std::string TokenTypeToString(TokenType type);

#endif