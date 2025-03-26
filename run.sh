#!/bin/bash

# Script to build and run pi-lang

# Exit immediately if a command exits with a non-zero status
set -e

# Create build directory if it doesn't exist
mkdir -p build

# Navigate to build directory
cd build

# Run CMake configuration
cmake ..

# Build the project
cmake --build .

# Compiler the given .pi file to LLVM IR
./picc ../examples/const.pi > const.ll

# Run the generated LLVM IR
lli const.ll