# AI-Augmented Compiler Development Flow

This document outlines the standard workflow for implementing features in this system programming language. It is designed to leverage **Antigravity (AI IDE)** capabilities, combining Test-Driven Development (TDD) with AI-assisted code generation and refactoring.

**For the AI Agent:** When assisting with a new feature, strictly follow these sequential phases to ensure consistency between Grammar, AST, Implementation, and Documentation.

---

## Phase 1: Specification by Example (Design)
*Goal: Define the User Experience (UX) of the syntax before implementation.*

1.  **Developer Action:** Describe the feature (e.g., "Enum Pattern Matching") and intent.
2.  **AI Action:** Generate 3-5 code snippets demonstrating the proposed syntax.
    * Include **Happy Paths** (standard usage).
    * Include **Edge Cases** (nested structures, error states).
3.  **Outcome:** A consensus on how the code *looks* and *feels*.

## Phase 2: Grammar & Structural Skeleton
*Goal: Formalize the syntax and prepare the internal data structures.*

1.  **Developer/AI Action:** Update the **EBNF** (or grammar definition file) to support the new syntax.
2.  **AI Action (Cascading Update):** Immediately reflect these changes in the compiler infrastructure:
    * Update **AST definitions** (Structs/Classes/Enums).
    * Update the **Parser** logic (if not auto-generated).
    * Update **Visitors/Traversers** (add missing interface methods).
    * *Note: The code should compile, but the feature logic is not yet implemented.*

## Phase 3: Test Generation (Red State)
*Goal: Convert the design specs into executable requirements.*

1.  **AI Action:** Convert the snippets from *Phase 1* into the project's test suite format.
    * Create **Parser Tests** (Ensure the syntax produces the correct AST).
    * Create **Semantic Tests** (Expect compilation errors or specific runtime outputs).
2.  **Status Check:** Run tests. They must **compile** (thanks to Phase 2) but **fail** assertions (logic missing).

## Phase 4: Implementation (Green State)
*Goal: Implement the compiler logic.*

1.  **Developer/AI Action:** Implement the feature in the Compiler Backend:
    * **Type Checker:** Validate types, scoping, and semantic rules.
    * **IR Generation:** Emit the correct Intermediate Representation.
    * **Memory Layout:** Define how data is stored (if applicable).
2.  **Iterative Loop:** Modify code until all tests from *Phase 3* pass.

## Phase 5: Documentation & Consistency
*Goal: Ensure the ecosystem is up to date.*

1.  **AI Action:**
    * Update the standard library (if the feature requires it).
    * Update `docs/` using the examples from *Phase 1*.
    * Check if syntax highlighters (VS Code extensions, etc.) need regex updates.

---

## 🤖 System Instructions for AI Agent

When the user requests a new language feature:
1.  **Do not start modifying the backend immediately.**
2.  Ask for or propose **syntax examples** first.
3.  When the EBNF is modified, automatically check the **AST** for required updates to prevent drift.
4.  Treat the AST as the source of truth for the compiler's internal structure.
5.  Always verify that changes in the grammar are reflected in the `PrettyPrinter` or `Formatter` to ensure tooling consistency.