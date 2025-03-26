#include "../include/Token.h"

std::string TokenTypeToString(TokenType type) {

    switch (type) {
        case TokenType::TOKEN_FUNC:     return "func";

        case TokenType::TOKEN_START:    return "start function";
        case TokenType::TOKEN_IDENT:    return "identifier";
        case TokenType::TOKEN_PRINT:    return "print";
        case TokenType::TOKEN_CONST:    return "const";

        case TokenType::TOKEN_LPAREN:   return "(";
        case TokenType::TOKEN_RPAREN:   return ")";

        case TokenType::TOKEN_LBRACE:   return "{";
        case TokenType::TOKEN_RBRACE:   return "}";

        case TokenType::TOKEN_COLON:    return ":";
        case TokenType::TOKEN_ASSIGN:   return "=";
        case TokenType::TOKEN_ARROW:    return "->";
        
        // CHARACTER TYPES

        case TokenType::TOKEN_CHAR8:    return "char8";
        case TokenType::TOKEN_CHAR16:   return "char16";
        case TokenType::TOKEN_CHAR32:   return "char32";

        // INTEGER TYPES

        case TokenType::TOKEN_INT8:     return "int8";
        case TokenType::TOKEN_INT16:    return "int16";
        case TokenType::TOKEN_INT32:    return "int32";
        case TokenType::TOKEN_INT64:    return "int64";

        case TokenType::TOKEN_STRING:   return "string";
        case TokenType::TOKEN_NUMBER:    return "number";

        case TokenType::TOKEN_EOF:      return "end of file";
        case TokenType::TOKEN_UNKNOWN:  return "unknown";
    }

}