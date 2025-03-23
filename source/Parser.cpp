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
    if (t.type == TOKEN_INT8 || t.type == TOKEN_INT16 || t.type == TOKEN_INT32 || t.type == TOKEN_INT64) {
        retType = t.lexeme;
        advance();
    } else {
        throw std::runtime_error("Expected type (e.g. int8, int16, int32, int64) after '->'");
    }

    // Function body
    expect(TOKEN_LBRACE, "Expected '{' to start function body");

    // For now, we only accept a single print statement
    expect(TOKEN_PRINT, "Expected 'print' in function body");
    expect(TOKEN_LPAREN, "Expected '(' after 'print'");

    std::string printText;
    if (currentToken().type == TOKEN_STRING) {
        printText = currentToken().lexeme;
        advance();
    } else {
        throw std::runtime_error("Expected string literal in print statement");
    }

    expect(TOKEN_RPAREN, "Expected ')' after string literal");
    expect(TOKEN_RBRACE, "Expected '}' to close function body");

    // Create AST node
    auto printNode = std::make_unique<PrintNode>();
    printNode->text = printText;

    auto funcNode = std::make_unique<FunctionNode>();
    funcNode->name = funcName;
    funcNode->returnType = retType;
    funcNode->body = std::move(printNode);

    return funcNode;
}