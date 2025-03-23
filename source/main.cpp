#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <memory>
#include <vector>

#include "../include/Lexer.h"
#include "../include/Parser.h"

using namespace llvm;

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

    if (argc < 2) {
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

    // Initialize LLVM
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();

    // Lexical analysis: Tokenize the read pi language code
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    // Parsing: Build an AST from the tokens
    Parser parser(tokens);
    std::unique_ptr<FunctionNode> funcAST;
    try {
        funcAST = parser.parseFunction();
    } catch (const std::runtime_error &e) {
        errs() << "Parsing error: " << e.what() << "\n";
        return 1;
    }

    // IR-Generation
    LLVMContext Context;
    auto TheModule = std::make_unique<Module>("MyLangModule", Context);
    IRBuilder<> Builder(Context);

    Type* retType = nullptr;
    if (funcAST->returnType == "char8")
        retType = Builder.getInt8Ty();
    else if (funcAST->returnType == "char16")
        retType = Builder.getInt16Ty();
    else if (funcAST->returnType == "char32")
        retType = Builder.getInt32Ty();
    else if (funcAST->returnType == "int8")
        retType = Builder.getInt8Ty();
    else if (funcAST->returnType == "int16")
        retType = Builder.getInt16Ty();
    else if (funcAST->returnType == "int32")
        retType = Builder.getInt32Ty();
    else if (funcAST->returnType == "int64")
        retType = Builder.getInt64Ty();
    else
        throw std::runtime_error("Unsupported return type: " + funcAST->returnType);

    // Declare C function: int puts(const char*);
    std::vector<Type*> putsArgs { Type::getInt8Ty(Context)->getPointerTo() };
    FunctionType *putsType = FunctionType::get(Builder.getInt32Ty(), putsArgs, false);
    FunctionCallee putsFunc = TheModule->getOrInsertFunction("puts", putsType);

    // Create "start" funktion: void start()
    FunctionType *funcType = FunctionType::get(retType, false);
    Function *func = Function::Create(funcType, Function::ExternalLinkage, funcAST->name, TheModule.get());
    BasicBlock *funcBB = BasicBlock::Create(Context, "entry", func);
    Builder.SetInsertPoint(funcBB);

    // Expect body is PrintNode
    PrintNode *printNode = dynamic_cast<PrintNode*>(funcAST->body.get());
    if (printNode) {
        Value *helloWorld = Builder.CreateGlobalStringPtr(printNode->text, "helloWorld");
        Builder.CreateCall(putsFunc, helloWorld);
    }

    // For int32 return functions we return 0
    Builder.CreateRet(ConstantInt::get(retType, 0));
    verifyFunction(*func);

    // Create the “main” function: int main()
    FunctionType *mainType = FunctionType::get(Builder.getInt32Ty(), false);
    Function *mainFunc = Function::Create(mainType, Function::ExternalLinkage, "main", TheModule.get());
    BasicBlock *mainBB = BasicBlock::Create(Context, "entry", mainFunc);
    Builder.SetInsertPoint(mainBB);
    Builder.CreateCall(func);
    Builder.CreateRet(ConstantInt::get(Builder.getInt32Ty(), 0));
    verifyFunction(*mainFunc);

    // Output of the generated LLVM-IR to stdout
    TheModule->print(outs(), nullptr);
    return 0;
    
}