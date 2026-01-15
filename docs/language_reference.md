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
| `uint8`, `uint16`, `uint32`, `uint64` | Unsigned Integers |
| `char8`, `char16`, `char32` | Character types |

**Syntax:**
```ebnf
Type ::= "char8" | "char16" | "char32" | "int8" | "int16" | "int32" | "int64" | "uint8" | "uint16" | "uint32" | "uint64"
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

### Return
The `return` statement ends the execution of a function and optionally returns a value to the caller. The returned value must match the function's return type.

```ebnf
ReturnStatement ::= "return" [ Expression ]
```

**Example:**
```pi
func add(a: int32, b: int32) -> int32 {
    return a + b
}

func doNothing() -> void {
    return // Optional for void functions at the end of block
}
```

## Expressions
Pi supports standard arithmetic operations. Precedence follows standard mathematical rules (multiplication/division before addition/subtraction).

```ebnf
Expression ::= Term { ("+" | "-") Term }
Term       ::= Factor { ("*" | "/") Factor }
Factor     ::= NumberLiteral | CharLiteral | Identifier | "(" Expression ")"
```

**Example:**
```pi
const a: int32 = 10
const b: int32 = 20
const c: int32 = a + b // Result: 30 (Variables in expressions)
const d: int32 = (a * 2) + b // Result: 40
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
