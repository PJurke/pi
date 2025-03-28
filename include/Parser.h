#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <string>
#include <vector>

#include "Lexer.h"

/// @brief Base class for AST nodes
struct ASTNode {
    virtual ~ASTNode() = default;
};

/// @brief AST node for a print command
struct PrintNode : public ASTNode {
    std::string text;
};

/// @brief AST node for functions
struct FunctionNode : public ASTNode {
    std::string name;
    std::string returnType; // Here we can later represent a more complex type
    std::vector<std::unique_ptr<ASTNode>> body; // We currently allow multiple print statements
};

/// @brief AST node for constants
struct ConstNode : public ASTNode {
    std::string name;
    std::string type;
    int value;
};

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<FunctionNode> parseFunction();
    std::unique_ptr<ASTNode> parseStatement();
    
private:
    const std::vector<Token>& tokens;
    size_t index;

    /// @brief Get the current token from the token vector 
    Token currentToken();

    /// @brief Advance the index to the next token
    void advance();
    
    // Auxiliary functions, e.g. to check expected tokens:
    void expect(TokenType type, const std::string& errorMessage);

};

#endif