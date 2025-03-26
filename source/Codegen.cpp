#include "Codegen.h"
#include "Token.h"

using namespace llvm;

Codegen::Codegen() : module(std::make_unique<Module>("MyLangModule", context)), builder(context) {

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

void Codegen::generateCode(const FunctionNode* funcAST) {

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

            Value* strVal = builder.CreateGlobalStringPtr(printNode->text, "str");
            builder.CreateCall(putsFunc, strVal);

        } else if (auto constNode = dynamic_cast<const ConstNode*>(stmt.get())) {

            // Determine the corresponding LLVM type for the constant
            llvm::Type* varType = getReturnType(constNode->type);

            // Create a local variable (Alloca)
            llvm::AllocaInst* alloc = builder.CreateAlloca(varType, nullptr, constNode->name);
            
            // Write the constant value to the variable
            builder.CreateStore(llvm::ConstantInt::get(varType, constNode->value), alloc);

        }
    }

    // Return: 0 as default value
    builder.CreateRet(ConstantInt::get(retType, 0));
    verifyFunction(*func);
    
}

void Codegen::printModule() const {
    module->print(outs(), nullptr);
}

std::unique_ptr<llvm::Module>& Codegen::getModule() {
    return module;
}
