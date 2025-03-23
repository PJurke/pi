# EBNF

```ebnf
Program ::= { FunctionDef }
```

```ebnf
FunctionDef ::= "func" Identifier "(" ")" "->" Type "{" Statement "}"
```

```ebnf
Statement ::= PrintStatement
```

```ebnf
PrintStatement ::= "print" "(" StringLiteral ")"
```

```ebnf
Type ::= "char8" | "char16" | "char32" | "int8" | "int16" | "int32" | "int64"
```

```ebnf
Identifier ::= Letter { Letter | Digit }
```

```ebnf
StringLiteral ::= '"' { AnyCharExceptQuote } '"'
```

```ebnf
Letter ::= "A" | "B" | "C" | ... | "Z" | "a" | "b" | ... | "z"
```

```ebnf
Digit ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
```

```ebnf
AnyCharExceptQuote ::= any character except '"'
```