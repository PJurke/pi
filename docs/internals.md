# Compiler Internals & Contributing

This document outlines the architecture of the Pi compiler and the workflow for contributing, specifically tailored for an **AI-Augmented** development process.

## Architecture Pipeline

The compiler (`picc`) follows a standard multi-pass architecture:

1.  **Lexer (`source/Lexer.cpp`)**: Converts raw source code (`.pi`) into a stream of **Tokens**.
2.  **Parser (`source/Parser.cpp`)**: Consumes tokens and builds the **Abstract Syntax Tree (AST)** based on the grammar.
3.  **Code Generation (`source/Codegen.cpp`)**: Traverses the AST and emits **LLVM IR**.
4.  **LLVM Backend**: The emitted IR is valid logic that can be executed by `lli` or compiled to native machine code by `llc`.

### Directory Structure
*   `source/`: C++ implementation files.
*   `include/`: Header files defining the AST, Tokens, and Interfaces.
*   `tests/`: Test suite (FileCheck based).
*   `docs/`: This documentation.

---

## AI-Augmented Workflow

We use a structured flow to ensure consistency between the Language Spec (EBNF), the Implementation, and the Documentation. When adding a new feature, follow these phases:

### Phase 1: Specification by Example (Design)
*Goal: Define the User Experience (UX) of the syntax before implementation.*

1.  Describe the feature and intent.
2.  Generate 3-5 code snippets (Happy Paths + Edge Cases).
3.  Reach consensus on the syntax.

### Phase 2: Grammar & Structural Skeleton
*Goal: Formalize syntax and structure.*

1.  Update `ebnf.md`.
2.  Update AST nodes (`include/AST.h`), Parser, and Visitors.
3.  Ensure code compiles (even if logic is empty).

### Phase 3: Test Generation (Red State)
*Goal: Create executable requirements.*

1.  Create **Parser Tests** (Syntax validation).
2.  Create **Semantic Tests** (Logic validation).
3.  Run tests -> They should **fail** assertions (Red State).

### Phase 4: Implementation (Green State)
*Goal: Implement logic.*

1.  Implement Type Checking and IR Generation.
2.  Iterate until all Phase 3 tests pass (Green State).

### Phase 5: Documentation & Consistency
*Goal: Polish.*

1.  Update `docs/language_reference.md` with new examples.
2.  Verify standard library impact.

## Running Tests

The project uses a Python-based test runner that leverages `FileCheck` style pattern matching.

```bash
# From the project root
python3 test_runner.py
```
