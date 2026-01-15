#include <stdexcept>

#include "../include/Logger.h"
#include "../include/ScopedLogger.h"
#include "../include/Parser.h"
#include "../include/Token.h"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), index(0) {
    LOG_INFO("Initializing Parser with " + std::to_string(tokens.size()) + " tokens");
}

Token Parser::currentToken() {
    if (index < tokens.size())
        return tokens[index];

    return {TOKEN_EOF, "", -1, -1};
}

bool Parser::isAtEOF() {
    return currentToken().type == TOKEN_EOF;
}

void Parser::advance() {
    if (index < tokens.size())
        index++;
}

void Parser::expect(TokenType type, const std::string& errorMessage) {

    Token token = currentToken();

    if (token.type != type) {
        std::string fullError = "Syntax Error\n" +
            errorMessage + "\n" +
            "Line " + std::to_string(token.line) + ", column " + std::to_string(token.column) + "\n" +
            "Encountered: \"" + token.lexeme + "\"\n";

        throw std::runtime_error(fullError);
    }

    advance();
}

std::unique_ptr<FuncNode> Parser::parseFunction() {

    // Expected syntax
    // func <name> () -> <type> { <body> }
    expect(TOKEN_FUNC, "Expected 'func' at beginning of function definition");
    
    // Function name
    // Two possibilities: TOKEN_START (entry function) or TOKEN_IDENT (user-defined function)
    std::string functionName;

    if (currentToken().type == TOKEN_START || currentToken().type == TOKEN_IDENT) {
        functionName = currentToken().lexeme;
        advance();
    } else {
        throw std::runtime_error("Expected function name after 'func'");
    }

    LOG_INFO("Parsing Function '" + functionName + "'");
    LOG_SCOPE("Parsing");

    // Parameter list (empty for now)
    expect(TOKEN_LPAREN, "Expected '(' after function name");
    expect(TOKEN_RPAREN, "Expected ')' after '(' in function definition");

    // Return type
    expect(TOKEN_ARROW, "Expected '->' after parameter list");
    std::string returnType;

    Token t = currentToken();
    if (t.type == TOKEN_CHAR8 || t.type == TOKEN_CHAR16 || t.type == TOKEN_CHAR32 ||
        t.type == TOKEN_INT8 || t.type == TOKEN_INT16 || t.type == TOKEN_INT32 || t.type == TOKEN_INT64 ||
        t.type == TOKEN_UINT8 || t.type == TOKEN_UINT16 || t.type == TOKEN_UINT32 || t.type == TOKEN_UINT64 ||
        t.type == TOKEN_VOID) {
        returnType = t.lexeme;
        advance();
    } else {
        throw std::runtime_error("Expected type (e.g. void, char8, char16, char32, int8, int16, int32, int64) after '->'");
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

    auto funcNode = std::make_unique<FuncNode>();
    funcNode->name = functionName;
    funcNode->returnType = returnType;
    funcNode->body = std::move(bodyStatements);

    return funcNode;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    LOG_SCOPE("Parsing Statement");

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

        auto printNode = std::make_unique<PrintNode>();
        printNode->text = printText;
        return printNode;
    }
    else if (currentToken().type == TOKEN_CONST) {

        advance(); // skip 'const'

        if (currentToken().type != TOKEN_IDENT)
            throw std::runtime_error("Expected identifier after 'const'");

        std::string name = currentToken().lexeme;
        advance();

        expect(TOKEN_COLON, "Expected ':' after identifier");

        std::string declaredType;

        Token t = currentToken();
        if (t.type == TOKEN_CHAR8 || t.type == TOKEN_CHAR16 || t.type == TOKEN_CHAR32 ||
            t.type == TOKEN_INT8 || t.type == TOKEN_INT16 || t.type == TOKEN_INT32 || t.type == TOKEN_INT64 ||
            t.type == TOKEN_UINT8 || t.type == TOKEN_UINT16 || t.type == TOKEN_UINT32 || t.type == TOKEN_UINT64) {
            declaredType = t.lexeme;
            advance();
        } else {
            throw std::runtime_error("Expected type after ':'");
        }

        expect(TOKEN_ASSIGN, "Expected '=' after type");

        // Parse expression for the value
        auto expr = parseExpression();

        auto node = std::make_unique<ConstNode>();
        node->name = name;
        node->type = declaredType;
        node->value = std::move(expr);
        return node;
    }

    else if (currentToken().type == TOKEN_RETURN) {
        advance(); // skip 'return'

        std::unique_ptr<ASTNode> returnVal = nullptr;

        Token t = currentToken();
        // Check if the next token starts an expression
        if (t.type == TOKEN_NUMBER || t.type == TOKEN_CHAR || t.type == TOKEN_LPAREN) {
            returnVal = parseExpression();
        }

        auto returnNode = std::make_unique<ReturnNode>();
        returnNode->returnValue = std::move(returnVal);
        return returnNode;
    }

    throw std::runtime_error("Expected statement (print, const, or return)");
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
    // Expression ::= Term { ("+" | "-") Term }
    auto left = parseTerm();

    while (currentToken().type == TOKEN_PLUS || currentToken().type == TOKEN_MINUS) {
        std::string op = currentToken().lexeme;
        advance();
        auto right = parseTerm();
        
        auto binaryNode = std::make_unique<BinaryOpNode>();
        binaryNode->left = std::move(left);
        binaryNode->right = std::move(right);
        binaryNode->op = op;
        left = std::move(binaryNode);
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parseTerm() {
    // Term ::= Factor { ("*" | "/") Factor }
    auto left = parseFactor();

    while (currentToken().type == TOKEN_STAR || currentToken().type == TOKEN_SLASH) {
        std::string op = currentToken().lexeme;
        advance();
        auto right = parseFactor();

        auto binaryNode = std::make_unique<BinaryOpNode>();
        binaryNode->left = std::move(left);
        binaryNode->right = std::move(right);
        binaryNode->op = op;
        left = std::move(binaryNode);
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parseFactor() {
    // Factor ::= NumberLiteral | "(" Expression ")"
    
    if (currentToken().type == TOKEN_NUMBER) {
        int val = std::stoi(currentToken().lexeme);
        advance();
        auto node = std::make_unique<NumberNode>();
        node->value = val;
        return node;
    }
    else if (currentToken().type == TOKEN_CHAR) {
        char val = currentToken().lexeme[0];
        advance();
        auto node = std::make_unique<CharNode>();
        node->value = val;
        return node;
    }
    else if (currentToken().type == TOKEN_LPAREN) {
        advance(); // skip '('
        auto expr = parseExpression();
        expect(TOKEN_RPAREN, "Expected ')' after expression");
        return expr;
    }

    throw std::runtime_error("Unexpected token in expression: " + currentToken().lexeme);
}