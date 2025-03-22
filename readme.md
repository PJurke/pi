# PI Language

```shell
clang++ -std=c++17 -g source/main.cpp source/Lexer.cpp source/Parser.cpp source/Token.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o pi-lang
```