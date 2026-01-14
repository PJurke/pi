# Language Reference

This document defines the syntax and semantics of the Pi programming language.

## Program Structure
A Pi program consists of a series of function definitions. The entry point is the `main` function.

```ebnf
Program ::= { FunctionDefinition }
```

**Example:**
```pi
func main() -> int32 {
    // Statements...
}
```

## Functions
Functions are declared with the `func` keyword, followed by an identifier, empty parentheses (parameters coming soon), a return arrow `->`, and the return type.

```ebnf
FunctionDefinition ::= "func" Identifier "(" ")" "->" ReturnType "{" { Statement } "}"
ReturnType         ::= Type | "void"
```

**Example:**
```pi
func performCalc() -> int32 {
    const x: int32 = 10
    const y: int32 = 20
    print("Result:")
    const res: int32 = x + y
}
```

## Types
Pi is a statically typed language with explicit bit-width integers and keys.

| Type | Description |
| :--- | :--- |
| `void` | No return value (functions only) |
| `int8`, `int16`, `int32`, `int64` | Signed Integers |
| `char8`, `char16`, `char32` | Character types |

**Syntax:**
```ebnf
Type ::= "char8" | "char16" | "char32" | "int8" | "int16" | "int32" | "int64"
```

## Statements

### Constants
Variables in Pi are immutable constants declared with `const`. You must explicitly specify the type.

```ebnf
ConstStatement ::= "const" Identifier ":" Type "=" Expression
```

**Example:**
```pi
const width: int32 = 800
const height: int32 = 600
```

### Print
The `print` statement outputs a string literal to stdout.

```ebnf
PrintStatement ::= "print" "(" StringLiteral ")"
```

**Example:**
```pi
print("System OK")
```

## Expressions
Pi supports standard arithmetic operations. Precedence follows standard mathematical rules (multiplication/division before addition/subtraction).

```ebnf
Expression ::= Term { ("+" | "-") Term }
Term       ::= Factor { ("*" | "/") Factor }
Factor     ::= NumberLiteral | CharLiteral | "(" Expression ")"
```

**Example:**
```pi
const a: int32 = 10 + 5 * 2  // Result: 20
const b: int32 = (10 + 5) * 2 // Result: 30
```

## Literals

*   **Numbers**: Decimal digits (e.g., `123`, `0`, `99`).
*   **Characters**: Single quotes (e.g., `'A'`, `'z'`).
*   **Strings**: Double quotes (e.g., `"Hello World"`).

## Comments
Line comments start with `//` and are ignored by the compiler.

```pi
// This is a comment
const x: int32 = 1 // Comments can be here too
```
