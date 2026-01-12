# EBNF

**Program**<br>
The definition of a program.

```ebnf
Program ::= { FunctionDefinition }
```

**Functions and Statements**<br>
A function consists of a series of statements.

```ebnf
FunctionDefinition ::= "func" Identifier "(" ")" "->" Type "{" { Statement } "}"
```

For now, print and const statements are allowed.

```ebnf

Statement ::= PrintStatement | ConstStatement
```

```ebnf
PrintStatement ::= "print" "(" StringLiteral ")"
```

Only numeric literals are allowed for now

```ebnf
ConstStatement ::= "const" Identifier ":" Type "=" Expression
```

**Types and Constants**<br>
Pi has single characters (char) and integers (int).

```ebnf
Type ::= "char8" | "char16" | "char32" | "int8" | "int16" | "int32" | "int64"
```

```ebnf
Identifier ::= Letter { Letter | Digit }
```

**Expressions**<br>
Arithmetic expressions with support for addition, subtraction, multiplication, and division.

```ebnf
Expression ::= Term { ("+" | "-") Term }
```

```ebnf
Term ::= Factor { ("*" | "/") Factor }
```

```ebnf
Factor ::= NumberLiteral | "(" Expression ")"
```

**Literals**<br>
```ebnf
Literal ::= NumberLiteral | CharLiteral | StringLiteral
```

```ebnf
NumberLiteral ::= Digit { Digit }
```

```ebnf
CharLiteral ::= "'" AnySingleCharExceptQuoteOrBackslash "'"
```

```ebnf
StringLiteral ::= '"' { AnyCharExceptQuote } '"'
```

**Single Symbols**<br>
```ebnf
Letter ::= "A" | "B" | "C" | ... | "Z" | "a" | "b" | ... | "z"
```

```ebnf
Digit ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
```

```ebnf
AnyCharExceptQuote ::= any character except '"'
```