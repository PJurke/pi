#include "../include/Codegen.h"
#include "../include/Logger.h"
#include "../include/Token.h"

#include <llvm/Support/TargetSelect.h>

using namespace llvm;

static std::string formatError(const Token& token, const std::string& message) {
    return "Error: [Line " + std::to_string(token.line) + ", Col " + std::to_string(token.column) + "] " + message;
}

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

    if(retTypeStr == "char8" || retTypeStr == "int8" || retTypeStr == "uint8")
        return builder.getInt8Ty();
    else if(retTypeStr == "char16" || retTypeStr == "int16" || retTypeStr == "uint16")
        return builder.getInt16Ty();
    else if(retTypeStr == "char32" || retTypeStr == "int32" || retTypeStr == "uint32")
        return builder.getInt32Ty();
    else if(retTypeStr == "int64" || retTypeStr == "uint64")
        return builder.getInt64Ty();
    else if(retTypeStr == "void")
        return builder.getVoidTy();
    
    throw std::runtime_error("Unsupported return type: " + retTypeStr);

}

bool Codegen::isUnsignedType(const std::string &typeStr) {
    if (typeStr.find("uint") == 0) return true;
    return false;

}

void Codegen::generateCode(const FuncNode* funcAST) {

    // Determine the LLVM type for the return type of the function
    llvm::Type* retType = nullptr;
    try {
        retType = getReturnType(funcAST->returnType);
    } catch (const std::exception& e) {
        throw std::runtime_error(formatError(funcAST->token, e.what()));
    }

    // Create the functionC signature
    FunctionType* funcType = FunctionType::get(retType, false);
    Function* func = Function::Create(funcType, Function::ExternalLinkage, funcAST->name, module.get());
    BasicBlock* funcBB = BasicBlock::Create(context, "entry", func);
    builder.SetInsertPoint(funcBB);

    // clear the symbol table for the new function scope
    namedValues.clear();
    isUnsignedVar.clear();

    // Here we treat a series of print statements as a function body
    for (const auto& stmt : funcAST->body) {
        if (auto printNode = dynamic_cast<const PrintNode*>(stmt.get())) {
            generatePrint(printNode);
        } else if (auto constNode = dynamic_cast<const ConstNode*>(stmt.get())) {
            generateConst(constNode);
        } else if (auto returnNode = dynamic_cast<const ReturnNode*>(stmt.get())) {
            generateReturn(returnNode, retType, isUnsignedType(funcAST->returnType));
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

void Codegen::generateReturn(const ReturnNode* returnNode, llvm::Type* expectedRetType, bool isUnsigned) {
    if (!returnNode->returnValue) {
        if (!expectedRetType->isVoidTy())
            throw std::runtime_error(formatError(returnNode->token, "Function must return a value"));
        builder.CreateRetVoid();
        return;
    }

    if (expectedRetType->isVoidTy())
        throw std::runtime_error(formatError(returnNode->token, "Void function cannot return a value"));

    llvm::Value* retVal = generateExpression(returnNode->returnValue.get()).first;

    if (retVal->getType() != expectedRetType) {
        // Simple implicit cast attempt
        if (expectedRetType->isIntegerTy() && retVal->getType()->isIntegerTy()) {
            retVal = builder.CreateIntCast(retVal, expectedRetType, !isUnsigned, "casttmp");
        } else {
             throw std::runtime_error(formatError(returnNode->token, "Return type mismatch"));
        }
    }

    builder.CreateRet(retVal);
}

std::pair<llvm::Value*, bool> Codegen::generateExpression(const ASTNode* node) {
    if (auto numberNode = dynamic_cast<const NumberNode*>(node)) {
        return {llvm::ConstantInt::get(builder.getInt64Ty(), numberNode->value), false};
    }
    else if (auto charNode = dynamic_cast<const CharNode*>(node)) {
        return {llvm::ConstantInt::get(builder.getInt8Ty(), charNode->value), false};
    }
    else if (auto variableNode = dynamic_cast<const VariableNode*>(node)) {
        llvm::AllocaInst* alloca = namedValues[variableNode->name];
        if (!alloca) {
             throw std::runtime_error(formatError(variableNode->token, "Unknown variable: " + variableNode->name));
        }
        bool isUnsigned = isUnsignedVar[variableNode->name];
        return {builder.CreateLoad(alloca->getAllocatedType(), alloca, variableNode->name.c_str()), isUnsigned};
    }
    else if (auto binaryNode = dynamic_cast<const BinaryOpNode*>(node)) {
        auto leftResult = generateExpression(binaryNode->left.get());
        auto rightResult = generateExpression(binaryNode->right.get());
        
        llvm::Value* left = leftResult.first;
        llvm::Value* right = rightResult.first;
        
        bool isUnsigned = leftResult.second || rightResult.second;

        if (binaryNode->op == "+")
            return {builder.CreateAdd(left, right, "addtmp"), isUnsigned};
        if (binaryNode->op == "-")
            return {builder.CreateSub(left, right, "subtmp"), isUnsigned};
        if (binaryNode->op == "*")
            return {builder.CreateMul(left, right, "multmp"), isUnsigned};
        if (binaryNode->op == "/") {
            // Check for division by literal zero
            if (auto numNode = dynamic_cast<const NumberNode*>(binaryNode->right.get())) {
                if (numNode->value == 0) {
                     throw std::runtime_error(formatError(binaryNode->token, "Division by zero"));
                }
            }

            if (isUnsigned)
                return {builder.CreateUDiv(left, right, "divtmp"), true};
            else
                return {builder.CreateSDiv(left, right, "divtmp"), false};
        }
        
        throw std::runtime_error(formatError(binaryNode->token, "Unknown binary operator: " + binaryNode->op));
    }
    
    throw std::runtime_error(formatError(node->token, "Unknown expression node type"));
}

void Codegen::generateConst(const ConstNode* constNode) {

    // Determine the corresponding LLVM type for the constant
    llvm::Type* llvmType = nullptr;
    try {
        llvmType = getReturnType(constNode->type);
    } catch (const std::exception& e) {
        throw std::runtime_error(formatError(constNode->token, e.what()));
    }

    // Create a local variable (allocaInst)
    llvm::AllocaInst* allocaInst = builder.CreateAlloca(llvmType, nullptr, constNode->name);

    // [Semantic Check] Constant range check for integer literals
    if (auto numNode = dynamic_cast<const NumberNode*>(constNode->value.get())) {
        int64_t val = numNode->value;
        std::string type = constNode->type;

        if (type == "int8") {
            if (val < -128 || val > 127) throw std::runtime_error(formatError(numNode->token, "Constant value out of range (int8)"));
        } else if (type == "uint8") {
            if (val < 0 || val > 255) throw std::runtime_error(formatError(numNode->token, "Constant value out of range (uint8)"));
        } else if (type == "int16") {
            if (val < -32768 || val > 32767) throw std::runtime_error(formatError(numNode->token, "Constant value out of range (int16)"));
        } else if (type == "uint16") {
            if (val < 0 || val > 65535) throw std::runtime_error(formatError(numNode->token, "Constant value out of range (uint16)"));
        } else if (type == "uint32") {
            if (val < 0 || val > 4294967295) throw std::runtime_error(formatError(numNode->token, "Constant value out of range (uint32)"));
        }
        // int32 and int64 are generally covered by the parser's integer limit (if strictly 32-bit int), but good to be safe.
    }

    // Evaluate validity of the expression
    llvm::Value* initVal = generateExpression(constNode->value.get()).first;

    // Cast the value to the target type if necessary
    if (initVal->getType() != llvmType) {
        initVal = builder.CreateIntCast(initVal, llvmType, !isUnsignedType(constNode->type), "casttmp");
    }

    // Write the constant value to the variable
    builder.CreateStore(initVal, allocaInst);
    
    // Register in symbol table after initialization to prevent self-reference
    namedValues[constNode->name] = allocaInst;
    isUnsignedVar[constNode->name] = isUnsignedType(constNode->type);

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
