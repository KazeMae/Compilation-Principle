# Compilation-Principle

SWUST（西南科技大学）**编译原理** course project: a small compiler for a C-like teaching language.

The pipeline is **lexer → recursive-descent parser → semantic analysis**. Each stage can be printed on its own. This is coursework, not a production compiler.

## Language

Keywords: `int`, `if`, `else`, `while`, `for`, `read`, `write`.

Integers, assignment, arithmetic, comparisons, compound statements `{ ... }`. Sample (`test.test`):

```c
{
    int i;
    for(i = 0; i < 3; i = i + 1)
        write i;
}
```

Sources:

| File | Stage |
|---|---|
| `lexer.cpp` / `lexer.hpp` | Tokens: identifier, number, keyword, operator, delimiter, comment |
| `parse.cpp` / `parse.hpp` | Recursive-descent syntax tree (`if` / `while` / `for` / `read` / `write` / assignment) |
| `semantic.cpp` / `semantic.hpp` | Semantic check and intermediate code |
| `compiler.cpp` / `main.cpp` | CLI: read source, run the selected stage, write the dump |

## Build

C++20. The portable command:

```bash
g++ -std=c++20 -Wall -g lexer.cpp parse.cpp semantic.cpp compiler.cpp main.cpp -o Tcompiler.exe
```

`make` also works if `g++` is on `PATH` (the `clean` target in `makefile` uses Windows `del`).

## Run

```
./Tcompiler.exe <source> [-o <output>] [-l | -p | -s]
```

| Flag | Meaning |
|---|---|
| *(none)* | Default dump (lexer output unless a later stage is selected) |
| `-o` / `-O` *file* | Output path (default: `<source>.out`) |
| `-l` / `-L` | Lexer only |
| `-p` / `-P` | Parse (syntax tree) |
| `-s` / `-S` | Semantic analysis / intermediate code |

Examples:

```bash
./Tcompiler.exe test.test
./Tcompiler.exe test.test -o test.out -l   # lexer
./Tcompiler.exe test.test -o test.out -p   # parser
./Tcompiler.exe test.test -o test.out -s   # semantic
```
