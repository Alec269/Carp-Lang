# Carp lang

A crappy recreation of the basic features of C. This was made only for learning.

## Plan

First implement a C-like syntax-ed language excluding pointers.

- Phase 1: Create an Interpreter to make the language functional
- Phase 2: Create compiler backend via LLVM

## Warning - Still InDev. So -

- No `IR` Gen
- No **LLVM** use
- Parser **Incomplete**
- Semantic Analyser **Incomplete**

### Currently Supported Features

- Tokenisation of the following tokens
  - *int* keyword
  - *string* keyword
  - *identifier* (x,y,msg...)
  - `int literal` (5,543,33....)
  - *semicolon* (;)
  - Single-line and Block Comments
  - `string literal` ("hello world")
  - *if* Keyword
  - *while* Keyword
  - basic structure of *if* and *while* statements
  - Operators like `+, -, *, /, =, !=, >, <, >=, =<`
- Expr support
  - literals: `5`, `"hello"`
  - Vars: `x`, `y`, `z`...
  - bracket expr: `(5 + 3)`
  - unary: `-`
  - Binary:
    - `+`
    - `-`
    - `*`
    - `/`
- Stmt Support
  - var decls
    - strings: `string name = "Bob";`
    - int: `int x = 5;`
    - S.Analyser checks
      - Redeclaration
      - Type mismatch
  - Assignment
    - eg: `x = 10;`
    - S.Analyser checks
      - Var exists
      - Type mismatch
  - If stmt
    - `if (x) { ... } else { ... }`
    - Currently,
    - conditions must evaluate to wither 1 or 0
    - treating non-zero as true (just like C)
  - while stmt
    - `while () { ... }`
    - Same as if
- Blocks and Scopes
  - x disappears after the ending brace

```c++
{
    int x = 5;
}
```
