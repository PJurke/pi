#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <string>
#include <vector>

#include "Lexer.h"

// Base class for AST nodes
struct ASTNode {
    virtual ~ASTNode() = default;
};

// AST node for a print command
struct PrintNode : public ASTNode {
    std::string text;
};

// New AST node for functions
struct FunctionNode : public ASTNode {
    std::string name;
    std::string returnType; // Here we can later represent a more complex type
    std::vector<std::unique_ptr<ASTNode>> body; // We currently allow multiple print statements
};

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<FunctionNode> parseFunction();
    
private:
    const std::vector<Token>& tokens;
    size_t index;
    Token currentToken();
    void advance();
    
    // Auxiliary functions, e.g. to check expected tokens:
    void expect(TokenType type, const std::string& errMsg);
};

#endif