#include <stdexcept>

#include "../include/Logger.h"
#include "../include/ScopedLogger.h"
#include "../include/Parser.h"
#include "../include/Token.h"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), index(0) {
    // Explicitly ensure an EOF token exists at the end
    if (this->tokens.empty() || this->tokens.back().type != TOKEN_EOF) {
        // Use last token's line info if available, else placeholders
        int line = -1;
        int col = -1;
        if (!this->tokens.empty()) {
            line = this->tokens.back().line;
            col = this->tokens.back().column; // technically + length but this is fine
        }
        this->tokens.push_back({TOKEN_EOF, "", line, col});
    }
    LOG_INFO("Initializing Parser with " + std::to_string(this->tokens.size()) + " tokens");
}

const Token& Parser::currentToken() const {
    return tokens[index];
}

bool Parser::isAtEOF() const {
    return currentToken().type == TOKEN_EOF;
}

void Parser::advance() {
    if (!isAtEOF()) {
        index++;
    }
}

const Token& Parser::peek() const {
    return tokens[index];
}

const Token& Parser::previous() const {
    if (index > 0) return tokens[index - 1];
    return tokens[0];
}

bool Parser::check(TokenType type) const {
    if (isAtEOF()) return false;
    return currentToken().type == type;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

const Token& Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        advance();
        return previous();
    }

    const Token& token = currentToken();
    std::string fullError = "Syntax Error\n" +
        message + "\n" +
        "Line " + std::to_string(token.line) + ", column " + std::to_string(token.column) + "\n" +
        "Encountered: \"" + token.lexeme + "\"\n";

    throw std::runtime_error(fullError);
}

std::string Parser::parseType() {
    if (match({TOKEN_CHAR8, TOKEN_CHAR16, TOKEN_CHAR32,
               TOKEN_INT8, TOKEN_INT16, TOKEN_INT32, TOKEN_INT64,
               TOKEN_UINT8, TOKEN_UINT16, TOKEN_UINT32, TOKEN_UINT64,
               TOKEN_VOID})) {
        return previous().lexeme;
    }
    throw std::runtime_error("Expected type (e.g. void, char8, char16, char32, int8, int16, int32, int64) after symbol");
}

std::unique_ptr<FuncNode> Parser::parseFunction() {

    // Expected syntax
    // func <name> () -> <type> { <body> }
    consume(TOKEN_FUNC, "Expected 'func' at beginning of function definition");
    
    // Function name
    std::string functionName;
    Token funcNameToken;

    if (match({TOKEN_START, TOKEN_IDENT})) {
        funcNameToken = previous();
        functionName = funcNameToken.lexeme;
    } else {
        throw std::runtime_error("Expected function name after 'func'");
    }

    LOG_INFO("Parsing Function '" + functionName + "'");
    LOG_SCOPE("Parsing");

    // Parameter list (empty for now)
    consume(TOKEN_LPAREN, "Expected '(' after function name");
    consume(TOKEN_RPAREN, "Expected ')' after '(' in function definition");

    // Return type
    consume(TOKEN_ARROW, "Expected '->' after parameter list");
    std::string returnType = parseType();

    // Function body
    consume(TOKEN_LBRACE, "Expected '{' to start function body");

    // We allow multiple statements in a function body
    std::vector<std::unique_ptr<ASTNode>> bodyStatements;

    while (!check(TOKEN_RBRACE) && !isAtEOF()) {

        // Parse statement function processes print and const statements
        bodyStatements.push_back(parseStatement());
        
    }
    
    consume(TOKEN_RBRACE, "Expected '}' to close function body");

    auto funcNode = std::make_unique<FuncNode>();
    funcNode->token = funcNameToken;
    funcNode->name = functionName;
    funcNode->returnType = returnType;
    funcNode->body = std::move(bodyStatements);

    return funcNode;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    LOG_SCOPE("Parsing Statement");

    if (match({TOKEN_PRINT})) {
        Token printToken = previous();
        consume(TOKEN_LPAREN, "Expected '(' after 'print'");

        std::string printText;
        if (match({TOKEN_STRING})) {
            printText = previous().lexeme;
        } else {
            throw std::runtime_error("Expected string literal in print statement");
        }

        consume(TOKEN_RPAREN, "Expected ')' after string literal");

        auto printNode = std::make_unique<PrintNode>();
        printNode->token = printToken;
        printNode->text = printText;
        return printNode;
    }
    else if (match({TOKEN_CONST})) {
        Token constToken = previous();

        Token identToken = consume(TOKEN_IDENT, "Expected identifier after 'const'");
        std::string name = identToken.lexeme;

        consume(TOKEN_COLON, "Expected ':' after identifier");

        std::string declaredType = parseType();

        consume(TOKEN_ASSIGN, "Expected '=' after type");

        auto expr = parseExpression();

        auto node = std::make_unique<ConstNode>();
        node->token = constToken;
        node->name = name;
        node->type = declaredType;
        node->value = std::move(expr);
        return node;
    }
    else if (match({TOKEN_RETURN})) {
        Token returnToken = previous();
        
        std::unique_ptr<ASTNode> returnVal = nullptr;

        // Check lookahead for expression starters
        if (check(TOKEN_NUMBER) || check(TOKEN_CHAR) || check(TOKEN_LPAREN) || check(TOKEN_IDENT)) {
            returnVal = parseExpression();
        }

        auto returnNode = std::make_unique<ReturnNode>();
        returnNode->token = returnToken;
        returnNode->returnValue = std::move(returnVal);
        return returnNode;
    }

    Token t = currentToken();
    throw std::runtime_error("Expected statement (print, const, or return) but found '" + t.lexeme + "'");
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
    // Expression ::= Term { ("+" | "-") Term }
    auto left = parseTerm();

    while (match({TOKEN_PLUS, TOKEN_MINUS})) {
        Token opToken = previous();
        std::string op = opToken.lexeme;
        auto right = parseTerm();
        
        auto binaryNode = std::make_unique<BinaryOpNode>();
        binaryNode->token = opToken;
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

    while (match({TOKEN_STAR, TOKEN_SLASH})) {
        Token opToken = previous();
        std::string op = opToken.lexeme;
        auto right = parseFactor();

        auto binaryNode = std::make_unique<BinaryOpNode>();
        binaryNode->token = opToken;
        binaryNode->left = std::move(left);
        binaryNode->right = std::move(right);
        binaryNode->op = op;
        left = std::move(binaryNode);
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parseFactor() {
    // Factor ::= NumberLiteral | "(" Expression ")"
    
    if (match({TOKEN_MINUS})) {
        Token opToken = previous();
        auto operand = parseFactor(); // Recursion for "- - 5" Support

        if (auto num = dynamic_cast<NumberNode*>(operand.get())) {
            num->value = -num->value;
            num->token = opToken; // Update Token location to the minus sign
            return operand;
        }
        throw std::runtime_error("Syntax Error\nLine " + std::to_string(opToken.line) + ": Only integer literals can be negated currently.");
    }

    if (match({TOKEN_NUMBER})) {
        Token numToken = previous();
        int64_t val = std::stoll(numToken.lexeme);
        auto node = std::make_unique<NumberNode>();
        node->token = numToken;
        node->value = val;
        return node;
    }
    else if (match({TOKEN_CHAR})) {
        Token charToken = previous();
        char val = charToken.lexeme[0];
        auto node = std::make_unique<CharNode>();
        node->token = charToken;
        node->value = val;
        return node;
    }
    else if (match({TOKEN_IDENT})) {
        Token varToken = previous();
        std::string name = varToken.lexeme;
        auto node = std::make_unique<VariableNode>();
        node->token = varToken;
        node->name = name;
        return node;
    }
    else if (match({TOKEN_LPAREN})) {
        auto expr = parseExpression();
        consume(TOKEN_RPAREN, "Expected ')' after expression");
        return expr;
    }

    throw std::runtime_error("Unexpected token in expression: " + currentToken().lexeme);
}