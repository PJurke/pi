#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>



#include "../include/Codegen.h"
#include "../include/Lexer.h"
#include "../include/Logger.h"
#include "../include/ScopedLogger.h"
#include "../include/Parser.h"



/// @brief Helper function to read a file into a string
/// @param filename The name of the file to read
/// @return The content of the file as a string
std::string readFile(const std::string &filename) {
    std::ifstream file(filename);

    if (!file)
        throw std::runtime_error("Cannot open file " + filename);
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// The Pi file is given by the arguments    
int main(int argc, char **argv) {

    // Logger configuration
    Logger::getInstance().setLogLevel(LogLevel::DEBUG);
    Logger::getInstance().enableFileLogging("pi_compiler.log");

    LOG_INFO("PICC starting");

    if (argc < 2) {
        LOG_ERROR("Insufficient command line arguments");
        std::cerr << "Usage: " << argv[0] << " <pi_file_path>" << std::endl;
        return 1;
    }
    
    // Reads the path to the Pi file from the command line
    std::string filePath = argv[1];
    std::string source;
    try {
        source = readFile(filePath);
    } catch (const std::runtime_error &e) {
        std::cerr << "Error reading the file: " << e.what() << std::endl;
        return 1;
    }

    // Lexical analysis: Tokenize the read pi language code
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    // Parsing: Build an AST from the tokens
    Parser parser(tokens);
    std::unique_ptr<FuncNode> funcAST;
    try {
        funcAST = parser.parseFunction();
    } catch (const std::runtime_error &e) {
        std::cerr << "Parsing error: " << e.what() << "\n";
        return 1;
    }

    // Code generation via the outsourced module
    Codegen codegen;
    {
        LOG_SCOPE("Code Generation");
        codegen.generateCode(funcAST.get());
    }

    // Create the main function that calls the generated function
    {
        LOG_SCOPE("LLVM IR Construction (Main)");
        codegen.createMainWrapper(funcAST->name);
    }

    // Output of the generated LLVM-IR
    codegen.printModule();
    
    Logger::getInstance().printPerformanceSummary();
    return 0;
    
}