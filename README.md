# C++ Subset Compiler

A complete compiler implementation for the WLP4 (a subset of C++) programming language, which translates WLP4 source code to MIPS assembly language.

## Overview

WLP4 is a C-like programming language that supports:
- Integer types and pointers
- Functions and procedures
- Control flow (if/else, while loops)
- Dynamic memory allocation (new/delete)
- Arrays and pointer arithmetic
- Function calls and recursion

This compiler implements the full compilation pipeline from source code to executable MIPS assembly:

1. **Lexical Analysis** - Tokenizes source code
2. **Syntax Analysis** - Parses tokens into an abstract syntax tree
3. **Semantic Analysis** - Performs type checking and builds symbol tables
4. **Code Generation** - Translates to MIPS assembly
5. **Linking** - Combines generated code with runtime libraries

## Project Structure

```
├── examples/           # MIPS assembly examples and exercises
├── mips-exercises/     # Additional MIPS programming exercises
├── assembly-scanner/   # Scanner for MIPS assembly files
├── scanner/            # Lexical analyzer (tokenizer) for WLP4
├── parser/             # Syntax analyzer with SLR parsing
├── type-checker/       # Semantic analyzer and type checker
├── code-generator/     # Code generator (WLP4 → MIPS assembly)
└── linker/             # Linker utilities and runtime support
```

## Components

### Lexical Analysis (`scanner/`)
The scanner tokenizes WLP4 source code, recognizing keywords, identifiers, operators, and literals. It implements deterministic finite automata (DFA) for pattern matching and handles:
- Keywords: `int`, `wain`, `if`, `else`, `while`, `return`, `new`, `delete`, etc.
- Operators: arithmetic, comparison, and logical operators
- Literals: integers and NULL
- Delimiters: parentheses, braces, brackets, commas, semicolons

**Main file**: `scanner/q5/wlp4scan.cc`

### Syntax Analysis (`parser/`)
The parser uses an SLR (Simple Left-to-Right) parsing algorithm to build a parse tree from the token stream. It implements:
- Context-free grammar parsing
- Shift-reduce parsing with error handling
- Abstract syntax tree construction

**Main file**: `parser/wlp4parse.cc`  
**Grammar definition**: `parser/wlp4data.h`

### Semantic Analysis (`type-checker/`)
The type checker performs static semantic analysis:
- Type checking for expressions and assignments
- Symbol table management for variables and functions
- Scope resolution
- Parameter type verification
- Array bounds and pointer type validation

**Main file**: `type-checker/wlp4type.cc`

### Code Generation (`code-generator/`)
The code generator translates the type-checked intermediate representation to MIPS assembly:
- Register allocation and management
- Expression evaluation
- Control flow translation (if/else, loops)
- Function call/return handling
- Stack frame management
- Dynamic memory allocation support

**Main file**: `code-generator/wlp4gen.cc`

### Assembly Scanner (`assembly-scanner/`)
A utility scanner for parsing MIPS assembly files, used for testing and intermediate processing.

### Linker (`linker/`)
Runtime support and linking utilities that combine generated code with:
- Memory allocation routines
- I/O functions (println, readWord, etc.)
- System libraries

## Building and Usage

### Prerequisites
- C++14 compatible compiler (g++ or clang++)
- MIPS assembler and simulator
- Unix/Linux environment (for linker tools)

### Compilation Pipeline

The typical compilation process follows these steps:

```bash
# 1. Lexical analysis
wlp4scan < program.wlp4 > program.wlp4i

# 2. Syntax analysis
wlp4parse < program.wlp4i > program.wlp4ti

# 3. Semantic analysis (type checking)
wlp4type < program.wlp4ti > program.wlp4ti

# 4. Code generation
./code-generator/a.out < program.wlp4ti > program.asm

# 5. Assembly and linking
cs241.linkasm < program.asm > program.merl
cs241.linker program.merl print.merl alloc.merl > linked.merl
cs241.merl 0 < linked.merl > program.mips
```

### Quick Build Script

The `code-generator/run.sh` script demonstrates the complete compilation pipeline:

```bash
cd code-generator
./run.sh
```

## Example WLP4 Program

```cpp
int max(int a, int b) {
  int answer = 0;
  if (a > b) {
    answer = a;
  } else {
    answer = b;
  }
  return answer;
}

int wain(int* arr, int size) {
  int* ptr = NULL;
  int result = 0;
  ptr = new int[10];
  result = max(5, 10);
  delete[] ptr;
  return result;
}
```

## Features

- **Full WLP4 Language Support**: Implements all language features including functions, pointers, arrays, and control structures
- **Type Safety**: Comprehensive static type checking
- **Optimized Code Generation**: Efficient MIPS assembly output
- **Error Handling**: Detailed error messages for lexical, syntax, and semantic errors
- **Standard Library Integration**: Links with runtime libraries for I/O and memory management

## Implementation Details

- **Language**: C++14
- **Parsing Algorithm**: SLR (Simple Left-to-Right)
- **Target Architecture**: MIPS32
- **Memory Model**: Stack-based with heap allocation support
- **Register Convention**: Uses standard MIPS calling conventions

## Testing

The project includes various test cases in the `examples/`, `mips-exercises/`, and `test/` directories covering:
- Basic arithmetic and control flow
- Function calls and recursion
- Pointer operations
- Dynamic memory allocation
- Edge cases and error handling

## License

This is an educational compiler implementation project.

