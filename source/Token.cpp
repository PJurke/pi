#include "../include/Token.h"

std::string TokenTypeToString(TokenType type) {

    switch (type) {
        case TokenType::TOKEN_FUNC:     return "func";

        case TokenType::TOKEN_START:    return "start function";
        case TokenType::TOKEN_IDENT:    return "identifier";
        case TokenType::TOKEN_PRINT:    return "print";

        case TokenType::TOKEN_LPAREN:   return "(";
        case TokenType::TOKEN_RPAREN:   return ")";

        case TokenType::TOKEN_LBRACE:   return "{";
        case TokenType::TOKEN_RBRACE:   return "}";

        case TokenType::TOKEN_ARROW:    return "->";
        
        case TokenType::TOKEN_INT32:    return "int32";
        case TokenType::TOKEN_STRING:   return "string";

        case TokenType::TOKEN_EOF:      return "end of file";
        case TokenType::TOKEN_UNKNOWN:  return "unknown";
    }

}