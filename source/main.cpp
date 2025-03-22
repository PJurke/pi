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
        throw std::runtime_error("Kann Datei " + filename + " nicht öffnen.");
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {

    // Initialize LLVM
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();

    // Read Pi language file
    std::string source = readFile("code/start.pi");

    // Lexical analysis: Tokenize the read pi language code
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    // Parsing: Build an AST from the tokens
    Parser parser(tokens);
    auto funcAST = parser.parseFunction();

    // IR-Generation
    LLVMContext Context;
    auto TheModule = std::make_unique<Module>("MyLangModule", Context);
    IRBuilder<> Builder(Context);

    // For int32 return type
    Type *retType = Builder.getInt32Ty();

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