# Getting Started with Pi

## Prerequisites

To build the Pi compiler (`picc`), you need a standard C++ development environment:

*   **C++ Compiler**: Must support **C++17** (e.g., clang++, g++).
*   **CMake**: Version 3.10 or higher.
*   **LLVM**: Version 15+ (development libraries).
    *   *macOS*: `brew install llvm`
    *   *Linux*: `apt-get install llvm-dev`

## Building the Compiler

Pi uses CMake for its build system. Follow these steps to compile `picc`:

```bash
# 1. Create a build directory
mkdir -p build && cd build

# 2. Configure the project (pointing to your LLVM installation if needed)
cmake ..

# 3. Build the executable
cmake --build .
```

After a successful build, the compiler executable `picc` will be located in the `build/` directory.

## Hello World

Create a file named `hello.pi` with the following content:

```pi
func main() -> int32 {
    print("Hello World")
}
```

### Compiling and Running

Currently, the Pi compiler emits **LLVM IR** (Intermediate Representation). You can run this directly using the LLVM interpreter (`lli`).

```bash
# 1. Compile Pi source code to LLVM IR
./build/picc hello.pi > hello.ll

# 2. Run the LLVM IR
lli hello.ll
```

**Expected Output:**
```
Hello World
```

## Next Steps
Now that you have the compiler running, dive into the [Language Reference](./language_reference.md) to learn about types, variables, and expressions.
