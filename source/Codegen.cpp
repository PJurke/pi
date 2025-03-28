#include "../include/Codegen.h"
#include "../include/Logger.h"
#include "../include/Token.h"

using namespace llvm;

Codegen::Codegen() : module(std::make_unique<Module>("MyLangModule", context)), builder(context) {

    LOG_INFO("Initializing CodeGen with new LLVM module");

    // Declare the C function puts
    std::vector<Type*> putsArgs { Type::getInt8Ty(context)->getPointerTo() };
    FunctionType *putsType = FunctionType::get(builder.getInt32Ty(), putsArgs, false);
    putsFunc = module->getOrInsertFunction("puts", putsType);

}

llvm::Type* Codegen::getReturnType(const std::string &retTypeStr) {

    if(retTypeStr == "char8" || retTypeStr == "int8")
        return builder.getInt8Ty();
    else if(retTypeStr == "char16" || retTypeStr == "int16")
        return builder.getInt16Ty();
    else if(retTypeStr == "char32" || retTypeStr == "int32")
        return builder.getInt32Ty();
    else if(retTypeStr == "int64")
        return builder.getInt64Ty();
    
    throw std::runtime_error("Unsupported return type: " + retTypeStr);

}

void Codegen::generateCode(const FuncNode* funcAST) {

    // Determine the LLVM type for the return type of the function
    llvm::Type* retType = getReturnType(funcAST->returnType);

    // Create the function signature
    FunctionType* funcType = FunctionType::get(retType, false);
    Function* func = Function::Create(funcType, Function::ExternalLinkage, funcAST->name, module.get());
    BasicBlock* funcBB = BasicBlock::Create(context, "entry", func);
    builder.SetInsertPoint(funcBB);

    // Here we treat a series of print statements as a function body
    for (const auto& stmt : funcAST->body) {
        if (auto printNode = dynamic_cast<const PrintNode*>(stmt.get())) {
            generatePrint(printNode);
        } else if (auto constNode = dynamic_cast<const ConstNode*>(stmt.get())) {
            generateConst(constNode);
        }
    }

    // Return: 0 as default value
    builder.CreateRet(ConstantInt::get(retType, 0));
    verifyFunction(*func);
    
}

void Codegen::generateConst(const ConstNode* constNode) {

    // Determine the corresponding LLVM type for the constant
    llvm::Type* llvmType = getReturnType(constNode->type);

    // Create a local variable (allocaInst)
    llvm::AllocaInst* allocaInst = builder.CreateAlloca(llvmType, nullptr, constNode->name);
    
    // Write the constant value to the variable
    builder.CreateStore(llvm::ConstantInt::get(llvmType, constNode->value), allocaInst);

}

void Codegen::generatePrint(const PrintNode* printNode) {
    Value* strVal = builder.CreateGlobalStringPtr(printNode->text, "str");
    builder.CreateCall(putsFunc, strVal);
}

void Codegen::printModule() const {
    module->print(outs(), nullptr);
}

std::unique_ptr<llvm::Module>& Codegen::getModule() {
    return module;
}
