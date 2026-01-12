#include <cctype>

#include "../include/Lexer.h"
#include "../include/Logger.h"
#include "../include/ScopedLogger.h"

Lexer::Lexer(const std::string &source) : source(source), index(0), line(1), column(1) {
    LOG_INFO("Initializing Lexer with source code of length: " + std::to_string(source.length()));
}

char Lexer::currentChar() {
    if (index < source.size())
        return source[index];
    return '\0';
}

void Lexer::advance() {
    if (currentChar() == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    index++;
}

std::vector<Token> Lexer::tokenize() {

    LOG_SCOPE("Tokenization");

    std::vector<Token> tokens;

    while (index < source.size()) {

        char c = currentChar();

        if (isspace(c)) {
            advance();
            continue;
        }

        // Save start position for the current token
        int tokenLine = line;
        int tokenColumn = column;

        if (isalpha(c)) {
            std::string word;

            while (isalnum(currentChar())) {
                word.push_back(currentChar());
                advance();
            }

            if (word == "func")
                tokens.push_back({TOKEN_FUNC, word, tokenLine, tokenColumn});
            else if (word == "start")
                tokens.push_back({TOKEN_START, word, tokenLine, tokenColumn});
            else if (word == "print")
                tokens.push_back({TOKEN_PRINT, word, tokenLine, tokenColumn});
            else if (word == "const")
                tokens.push_back({TOKEN_CONST, word, tokenLine, tokenColumn});

            // CHARACTER TYPES
            else if (word == "char8")
                tokens.push_back({TOKEN_CHAR8, word, tokenLine, tokenColumn});
            else if (word == "char16")
                tokens.push_back({TOKEN_CHAR16, word, tokenLine, tokenColumn});
            else if (word == "char32")
                tokens.push_back({TOKEN_CHAR32, word, tokenLine, tokenColumn});

            // INTEGER TYPES
            else if (word == "int8")
                tokens.push_back({TOKEN_INT8, word, tokenLine, tokenColumn});
            else if (word == "int16")
                tokens.push_back({TOKEN_INT16, word, tokenLine, tokenColumn});
            else if (word == "int32")
                tokens.push_back({TOKEN_INT32, word, tokenLine, tokenColumn});
            else if (word == "int64")
                tokens.push_back({TOKEN_INT64, word, tokenLine, tokenColumn});
            
            else
                tokens.push_back({TOKEN_IDENT, word, tokenLine, tokenColumn});
            continue;
        }

        if (isdigit(c)) {

            std::string number;

            while (isdigit(currentChar())) {
                number += currentChar();
                advance();
            }
            
            tokens.push_back({TOKEN_NUMBER, number, tokenLine, tokenColumn});
            continue;
        }

        if (c == '\'') {

            // skip the opening '
            advance();
            
            std::string charLiteral;

            // optional: handling of escape sequences
            if (currentChar() == '\\') {
                advance();
                charLiteral.push_back(currentChar());
                advance();
            } else {
                charLiteral.push_back(currentChar());
                advance();
            }

            if (currentChar() != '\'')
                throw std::runtime_error("Expected closing ' for char literal");

            // skip the closing '
            advance();
            tokens.push_back({TOKEN_CHAR, charLiteral, tokenLine, tokenColumn});
            continue;
        }

        if (c == '-') {
            if (index + 1 < source.size() && source[index + 1] == '>') {
                tokens.push_back({TOKEN_ARROW, "->", tokenLine, tokenColumn});
                advance(); // skip '-'
                advance(); // skip '>'
            } else {
                tokens.push_back({TOKEN_MINUS, "-", tokenLine, tokenColumn});
                advance();
            }
            continue;
        }

        if (c == ':') {
            tokens.push_back({TOKEN_COLON, ":", tokenLine, tokenColumn});
            advance();
            continue;
        }

        if (c == '=') {
            tokens.push_back({TOKEN_ASSIGN, "=", tokenLine, tokenColumn});
            advance();
            continue;
        }

        if (c == '+') {
            tokens.push_back({TOKEN_PLUS, "+", tokenLine, tokenColumn});
            advance();
            continue;
        }

        if (c == '*') {
            tokens.push_back({TOKEN_STAR, "*", tokenLine, tokenColumn});
            advance();
            continue;
        }

        if (c == '/') {
            tokens.push_back({TOKEN_SLASH, "/", tokenLine, tokenColumn});
            advance();
            continue;
        }

        if (c == '(') {
            tokens.push_back({TOKEN_LPAREN, "(", tokenLine, tokenColumn});
            advance();
            continue;
        }

        if (c == ')') {
            tokens.push_back({TOKEN_RPAREN, ")", tokenLine, tokenColumn});
            advance();
            continue;
        }

        if (c == '{') {
            tokens.push_back({TOKEN_LBRACE, "{", tokenLine, tokenColumn});
            advance();
            continue;
        }

        if (c == '}') {
            tokens.push_back({TOKEN_RBRACE, "}", tokenLine, tokenColumn});
            advance();
            continue;
        }

        if (c == '"') {
            std::string str;
            advance(); // skip opening quotation mark

            while (currentChar() != '"' && currentChar() != '\0') {
                str.push_back(currentChar());
                advance();
            }

            advance(); // skip closing quotation mark
            tokens.push_back({TOKEN_STRING, str, tokenLine, tokenColumn});
            continue;
        }

        // Handle unknown characters
        tokens.push_back({TOKEN_UNKNOWN, std::string(1, c), tokenLine, tokenColumn});
        advance();
    }

    tokens.push_back({TOKEN_EOF, "", line, column});

    LOG_INFO("Tokenization completed successfully. Total tokens: " + std::to_string(tokens.size()));

    return tokens;
}