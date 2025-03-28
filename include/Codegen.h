#ifndef CODEGEN_H
#define CODEGEN_H

#include <memory>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>

#include "Parser.h"

/**
 * @brief Class for generating the LLVM IR from an AST.
 *
 * This class encapsulates the logic for code generation for the Pi Language.
 * It initializes the LLVM module, the IRBuilder and declares necessary external functions.
 */
class Codegen {
public:

    /**
     * @brief Constructor.
     *
     * Initializes the LLVM context, the module and the IRBuilder.
     * Also declares the external C function puts.
     */
    Codegen();

    /**
     * @brief Generates the LLVM IR code for a given function.
     *
     * Generates the corresponding LLVM function based on the transferred AST (FuncNode),
     * adds the function body (e.g. a print command) and verifies the function.
     *
     * @param funcAST Pointer to the AST node of the function.
     */
    void generateCode(const FuncNode* funcAST);

    /**
     * @brief Outputs the generated LLVM module on stdout.
     */
    void printModule() const;

    /**
     * @brief Access to the LLVM module.
     *
     * @return Reference to the unique_ptr that contains the module.
     */
    std::unique_ptr<llvm::Module>& getModule();
    
private:
    llvm::LLVMContext context;                  ///< LLVM context
    std::unique_ptr<llvm::Module> module;       ///< The LLVM module that contains the generated code
    llvm::IRBuilder<> builder;                  ///< Builder for the creation of LLVM IR
    llvm::FunctionCallee putsFunc;              ///< Declaration of the external C function puts

    /**
     * @brief Converts a return type string into an LLVM type.
     *
     * Supported are char8/int8, char16/int16, char32/int32 and int64.
     *
     * @param retTypeStr The string that describes the return type.
     * @return Pointer to the corresponding LLVM type.
     * @throws std::runtime_error if the type is not supported.
     */
    llvm::Type* getReturnType(const std::string &retTypeStr);
};

#endif