# Pi Documentation

## What is Pi?
Pi is a lean, explicit system programming language designed for simplicity and performance. Built on top of the **LLVM** compiler infrastructure, Pi aims to provide a modern, minimal approach to low-level programming without the complexity of legacy systems.

The core philosophy of Pi is **pragmatism**:
*   **Explicit**: No hidden control flow or magic conversions.
*   **Lean**: A small, comprehensible grammar that fits in your head.
*   **Modern**: Built from the ground up with AI-augmented workflows in mind.

## Key Features
*   **Static Typing**: Explicit integer (`int8` to `int64`) and character (`char8` to `char32`) widths.
*   **LLVM Backend**: Compiles to optimized LLVM IR for native performance.
*   **Simplified EBNF**: A clean, recursive-descent friendly grammar.
*   **First-class Functions**: Modular code structure (`func main() -> int32`).

## Contents

*   [**Getting Started**](./getting_started.md)
    *   Installation, building the compiler, and writing your first "Hello World".
*   [**Language Reference**](./language_reference.md)
    *   A complete guide to Pi's syntax, types, and control flow.
*   [**Internals & Contributing**](./internals.md)
    *   Deep dive into the compiler architecture and our unique AI-driven development flow.
