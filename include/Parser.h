#ifndef PARSER_H
#define PARSER_H

#include "Lexer.h"
#include <vector>
#include <string>
#include <memory>

// Base class for AST nodes
struct ASTNode {
    virtual ~ASTNode() = default;
};

// AST-Knoten für einen Print-Befehl
struct PrintNode : public ASTNode {
    std::string text;
};

// Neuer AST-Knoten für Funktionen
struct FunctionNode : public ASTNode {
    std::string name;
    std::string returnType; // Hier können wir später einen komplexeren Typ repräsentieren
    std::unique_ptr<ASTNode> body; // Aktuell gehen wir von einem einzelnen PrintNode aus
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
    
    // Hilfsfunktionen, z.B. um erwartete Token zu überprüfen:
    void expect(TokenType type, const std::string& errMsg);
};

#endif