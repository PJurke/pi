#include "../include/Lexer.h"
#include <cctype>

Lexer::Lexer(const std::string &source) : source(source), index(0) {}

char Lexer::currentChar() {
    if (index < source.size())
        return source[index];
    return '\0';
}

void Lexer::advance() {
    index++;
}

std::vector<Token> Lexer::tokenize() {

    std::vector<Token> tokens;

    while (index < source.size()) {

        char c = currentChar();

        if (isspace(c)) {
            advance();
            continue;
        }

        if (isalpha(c)) {
            std::string word;
            while (isalnum(currentChar())) {
                word.push_back(currentChar());
                advance();
            }
            if (word == "func")
                tokens.push_back({TOKEN_FUNC, word});
            else if (word == "start")
                tokens.push_back({TOKEN_START, word});
            else if (word == "print")
                tokens.push_back({TOKEN_PRINT, word});
            else if (word == "int32")
                tokens.push_back({TOKEN_INT32, word});
            else
                tokens.push_back({TOKEN_IDENT, word});
            continue;
        }

        if (c == '-' && (index + 1 < source.size()) && source[index + 1] == '>') {
            tokens.push_back({TOKEN_ARROW, "->"});
            advance(); // skip '-'
            advance(); // skip '>'
            continue;
        }

        if (c == '(') {
            tokens.push_back({TOKEN_LPAREN, "("});
            advance();
            continue;
        }

        if (c == ')') {
            tokens.push_back({TOKEN_RPAREN, ")"});
            advance();
            continue;
        }

        if (c == '{') {
            tokens.push_back({TOKEN_LBRACE, "{"});
            advance();
            continue;
        }

        if (c == '}') {
            tokens.push_back({TOKEN_RBRACE, "}"});
            advance();
            continue;
        }

        if (c == '"') {
            std::string str;
            advance(); // öffnendes Anführungszeichen überspringen
            while (currentChar() != '"' && currentChar() != '\0') {
                str.push_back(currentChar());
                advance();
            }
            advance(); // schließendes Anführungszeichen überspringen
            tokens.push_back({TOKEN_STRING, str});
            continue;
        }

        // Unbekannte Zeichen behandeln
        tokens.push_back({TOKEN_UNKNOWN, std::string(1, c)});
        advance();
    }

    tokens.push_back({TOKEN_EOF, ""});
    return tokens;
}