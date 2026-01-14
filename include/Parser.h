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


/// @brief AST node for numbers
struct NumberNode : public ASTNode {
    int value;
};

/// @brief AST node for characters
struct CharNode : public ASTNode {
    char value;
};

/// @brief AST node for binary operations
struct BinaryOpNode : public ASTNode {
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    std::string op;
};

/// @brief AST node for functions
struct FuncNode : public ASTNode {
    std::string name;
    std::string returnType;
    std::vector<std::unique_ptr<ASTNode>> body;     // Series of statements
};

/// @brief AST node for constants
struct ConstNode : public ASTNode {
    std::string name;
    std::string type;
    std::unique_ptr<ASTNode> value;
};

/// @brief AST node for return statements
struct ReturnNode : public ASTNode {
    std::unique_ptr<ASTNode> returnValue; // Optional return value
};

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<FuncNode> parseFunction();
    std::unique_ptr<ASTNode> parseStatement();
    
    /// @brief Check if parser reached end of file
    bool isAtEOF();
    
    // Expression parsing
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseFactor();

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