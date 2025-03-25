cmake_minimum_required(VERSION 3.10)
project(PiCompiler)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Search for LLVM (requires LLVM to be correctly installed and configured)
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Add the LLVM-specific include paths and definitions
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Add the include path for your own header files (assuming they are in the “include” folder)
include_directories(${PROJECT_SOURCE_DIR}/include)

# List of source code files (adjust the paths if necessary)
set(SOURCE_FILES
    source/main.cpp
    source/Codegen.cpp
    source/Lexer.cpp
    source/Parser.cpp
    source/Token.cpp
)

# Create the executable
add_executable(picc ${SOURCE_FILES})

# Get the necessary LLVM libraries:
# We add the “native” component here in addition to “core” to link the native target functions (AArch64).
llvm_map_components_to_libnames(llvm_libs core native)

# Link the LLVM libraries to your project
target_link_libraries(picc ${llvm_libs})