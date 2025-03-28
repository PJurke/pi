#include <stdexcept>

#include "../include/Parser.h"
#include "../include/Token.h"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), index(0) {}

Token Parser::currentToken() {
    if (index < tokens.size())
        return tokens[index];
    return {TOKEN_EOF, ""};
}

void Parser::advance() {
    if (index < tokens.size())
        index++;
}

void Parser::expect(TokenType type, const std::string& errMsg) {

    Token token = currentToken();

    if (token.type != type) {
        std::string fullErr = "Syntax Error\n" +
            errMsg + "\n" +
            "Line " + std::to_string(token.line) + ", column " + std::to_string(token.column) + "\n" +
            "Encountered: \"" + token.lexeme + "\"\n";

        throw std::runtime_error(fullErr);
    }

    advance();
}

std::unique_ptr<FunctionNode> Parser::parseFunction() {

    // Expected syntax
    // func <name> () -> <type> { <body> }
    expect(TOKEN_FUNC, "Expected 'func' at beginning of function definition");
    
    // Function name
    // Two possibilities: TOKEN_START (entry function) or TOKEN_IDENT (user-defined function)
    std::string funcName;

    if (currentToken().type == TOKEN_START || currentToken().type == TOKEN_IDENT) {
        funcName = currentToken().lexeme;
        advance();
    } else {
        throw std::runtime_error("Expected function name after 'func'");
    }

    // Parameter list (empty for now)
    expect(TOKEN_LPAREN, "Expected '(' after function name");
    expect(TOKEN_RPAREN, "Expected ')' after '(' in function definition");

    // Return type
    expect(TOKEN_ARROW, "Expected '->' after parameter list");
    std::string retType;

    Token t = currentToken();
    if (t.type == TOKEN_CHAR8 || t.type == TOKEN_CHAR16 || t.type == TOKEN_CHAR32 ||
        t.type == TOKEN_INT8 || t.type == TOKEN_INT16 || t.type == TOKEN_INT32 || t.type == TOKEN_INT64) {
        retType = t.lexeme;
        advance();
    } else {
        throw std::runtime_error("Expected type (e.g. char8, char16, char32, int8, int16, int32, int64) after '->'");
    }

    // Function body
    expect(TOKEN_LBRACE, "Expected '{' to start function body");

    // We allow multiple statements in a function body
    std::vector<std::unique_ptr<ASTNode>> bodyStatements;

    while (currentToken().type != TOKEN_RBRACE) {

        // Parse statement function processes print and const statements
        bodyStatements.push_back(parseStatement());
        
    }
    
    expect(TOKEN_RBRACE, "Expected '}' to close function body");

    auto funcNode = std::make_unique<FunctionNode>();
    funcNode->name = funcName;
    funcNode->returnType = retType;
    funcNode->body = std::move(bodyStatements);

    return funcNode;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (currentToken().type == TOKEN_PRINT) {

        advance(); // skip 'print'
        expect(TOKEN_LPAREN, "Expected '(' after 'print'");

        std::string printText;

        if (currentToken().type == TOKEN_STRING) {
            printText = currentToken().lexeme;
            advance();
        } else {
            throw std::runtime_error("Expected string literal in print statement");
        }

        expect(TOKEN_RPAREN, "Expected ')' after string literal");

        auto node = std::make_unique<PrintNode>();
        node->text = printText;
        return node;
    }
    else if (currentToken().type == TOKEN_CONST) {

        advance(); // skip 'const'

        if (currentToken().type != TOKEN_IDENT)
            throw std::runtime_error("Expected identifier after 'const'");

        std::string name = currentToken().lexeme;
        advance();

        expect(TOKEN_COLON, "Expected ':' after identifier");

        std::string type;

        Token t = currentToken();
        if (t.type == TOKEN_CHAR8 || t.type == TOKEN_CHAR16 || t.type == TOKEN_CHAR32 ||
            t.type == TOKEN_INT8 || t.type == TOKEN_INT16 || t.type == TOKEN_INT32 || t.type == TOKEN_INT64) {
            type = t.lexeme;
            advance();
        } else {
            throw std::runtime_error("Expected type after ':'");
        }

        expect(TOKEN_ASSIGN, "Expected '=' after type");

        int value = 0;
        if (currentToken().type == TOKEN_NUMBER) {

            // A number literal is permitted for int types only
            if (type != "int8" && type != "int16" && type != "int32" && type != "int64") {

                std::string fullErr = std::string("Type Mismatch Error\n") +
                    "Expected char literal for constant of type " + type + "\n" +
                    "Line " + std::to_string(currentToken().line) + ", column " + std::to_string(currentToken().column) + "\n";

                throw std::runtime_error(fullErr);

            }

            value = std::stoi(currentToken().lexeme);
            advance();

        } else if (currentToken().type == TOKEN_CHAR) {

            // A character literal is permitted for char types only
            if (type != "char8" && type != "char16" && type != "char32") {

                std::string fullErr = std::string("Type Mismatch Error\n") +
                    "Expected number literal for constant of type " + type + "\n" +
                    "Line " + std::to_string(currentToken().line) + ", column " + std::to_string(currentToken().column) + "\n";

                throw std::runtime_error(fullErr);

            }

            value = static_cast<int>(currentToken().lexeme[0]);
            advance();

        } else {
            throw std::runtime_error("Expected number or char literal after '=' in const statement");
        }

        auto node = std::make_unique<ConstNode>();
        node->name = name;
        node->type = type;
        node->value = value;
        return node;
    }

    throw std::runtime_error("Expected statement (print or const)");
}
