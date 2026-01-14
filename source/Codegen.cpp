#include "../include/Codegen.h"
#include "../include/Logger.h"
#include "../include/Token.h"

#include <llvm/Support/TargetSelect.h>

using namespace llvm;

Codegen::Codegen() : module(std::make_unique<Module>("MyLangModule", context)), builder(context) {

    LOG_INFO("Initializing CodeGen with new LLVM module");

    // Initialize LLVM native targets
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();

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
    else if(retTypeStr == "void")
        return builder.getVoidTy();
    
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
        } else if (auto returnNode = dynamic_cast<const ReturnNode*>(stmt.get())) {
            generateReturn(returnNode, retType);
        }
    }

    // Return: 0 as default value if no return encountered (implicit void return at end)
    // Note: If the last statement was a return, this might be unreachable, but LLVM handles it.
    if (!builder.GetInsertBlock()->getTerminator()) {
        if (retType->isVoidTy()) {
            builder.CreateRetVoid();
        } else {
            builder.CreateRet(ConstantInt::get(retType, 0));
        }
    }
    verifyFunction(*func);
    
}

void Codegen::generateReturn(const ReturnNode* returnNode, llvm::Type* expectedRetType) {
    if (!returnNode->returnValue) {
        if (!expectedRetType->isVoidTy())
            throw std::runtime_error("Function must return a value");
        builder.CreateRetVoid();
        return;
    }

    if (expectedRetType->isVoidTy())
        throw std::runtime_error("Void function cannot return a value");

    llvm::Value* retVal = generateExpression(returnNode->returnValue.get());

    if (retVal->getType() != expectedRetType) {
        // Simple implicit cast attempt
        if (expectedRetType->isIntegerTy() && retVal->getType()->isIntegerTy()) {
            retVal = builder.CreateIntCast(retVal, expectedRetType, true, "casttmp");
        } else {
             throw std::runtime_error("Return type mismatch");
        }
    }

    builder.CreateRet(retVal);
}

llvm::Value* Codegen::generateExpression(const ASTNode* node) {
    if (auto numberNode = dynamic_cast<const NumberNode*>(node)) {
        return llvm::ConstantInt::get(builder.getInt32Ty(), numberNode->value);
    }
    else if (auto charNode = dynamic_cast<const CharNode*>(node)) {
        return llvm::ConstantInt::get(builder.getInt8Ty(), charNode->value);
    }
    else if (auto binaryNode = dynamic_cast<const BinaryOpNode*>(node)) {
        llvm::Value* left = generateExpression(binaryNode->left.get());
        llvm::Value* right = generateExpression(binaryNode->right.get());

        if (binaryNode->op == "+")
            return builder.CreateAdd(left, right, "addtmp");
        if (binaryNode->op == "-")
            return builder.CreateSub(left, right, "subtmp");
        if (binaryNode->op == "*")
            return builder.CreateMul(left, right, "multmp");
        if (binaryNode->op == "/")
            return builder.CreateSDiv(left, right, "divtmp"); // Signed division
        
        throw std::runtime_error("Unknown binary operator: " + binaryNode->op);
    }
    
    throw std::runtime_error("Unknown expression node type");
}

void Codegen::generateConst(const ConstNode* constNode) {

    // Determine the corresponding LLVM type for the constant
    llvm::Type* llvmType = getReturnType(constNode->type);

    // Create a local variable (allocaInst)
    llvm::AllocaInst* allocaInst = builder.CreateAlloca(llvmType, nullptr, constNode->name);
    
    // Evaluate validity of the expression
    llvm::Value* initVal = generateExpression(constNode->value.get());

    // Cast the value to the target type if necessary
    if (initVal->getType() != llvmType) {
        initVal = builder.CreateIntCast(initVal, llvmType, true, "casttmp");
    }

    // Write the constant value to the variable
    builder.CreateStore(initVal, allocaInst);

}

void Codegen::generatePrint(const PrintNode* printNode) {
    Value* strVal = builder.CreateGlobalStringPtr(printNode->text, "str");
    builder.CreateCall(putsFunc, strVal);
}

void Codegen::createMainWrapper(const std::string& targetFuncName) {
    llvm::FunctionType* mainType = llvm::FunctionType::get(builder.getInt32Ty(), false);
    llvm::Function* mainFunc = llvm::Function::Create(mainType, llvm::Function::ExternalLinkage, "main", module.get());
    llvm::BasicBlock* mainBB = llvm::BasicBlock::Create(context, "entry", mainFunc);
    
    // Save current insert point
    auto savedInsertBlock = builder.GetInsertBlock();
    auto savedInsertPoint = builder.GetInsertPoint();

    builder.SetInsertPoint(mainBB);
    
    llvm::Function* targetFunc = module->getFunction(targetFuncName);
    if (!targetFunc) {
        throw std::runtime_error("Function " + targetFuncName + " not found in module");
    }

    builder.CreateCall(targetFunc);
    builder.CreateRet(llvm::ConstantInt::get(builder.getInt32Ty(), 0));
    llvm::verifyFunction(*mainFunc);
    
    // Restore insert point (optional, but good practice if we were doing more)
    if (savedInsertBlock)
        builder.SetInsertPoint(savedInsertBlock, savedInsertPoint);
}

void Codegen::printModule() const {
    module->print(outs(), nullptr);
}

std::unique_ptr<llvm::Module>& Codegen::getModule() {
    return module;
}
