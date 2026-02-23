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

- Tokenisation, Parsing and Semantic Analysis of the following
  - *int* keyword
  - *string* keyword
  - *bool* keyword
  - *identifier* (x,y,msg...)
  - `int literal` (5,543,33....)
  - *semicolon* (;)
  - Single-line and Block Comments
  - `string literal` ("hello world")
  - `Booleans` (true, false)
  - *if* Keyword
  - *while* Keyword
  - basic structure of *if* and *while* statements
  - Operators like `+, -, *, /, =, ==, !=, >, <, [<=, =<], [>=, =>]`
- Expr support
  - literals: `5`, `"hello"`
  - boolean: `true`, `false`
  - bracket expr: `(5 + 3)`
  - unary: `-`
  - Binary:
    - `+`
    - `-`
    - `*`
    - `/`
    - `=`
    - `==`
    - `!=`
    - `>`
    - `<`
    - `<=`, `=<`
    - `>=`, `=>`
- Stmt Support
  - var decls
    - string: `string name = "Bob";`
    - int: `int x = 5;`
    - bool: `bool isStudent = true;`
    - Semantic Analyser checks
      - Redeclaration
      - Type mismatch
  - Assignment
    - eg: `x = 10;`
    - Semantic Analyser checks
      - Var exists
      - Type mismatch
  - If stmt
    - `if (x > 5) { ... } else { ... }`
    - Currently,
    - conditions must evaluate to a boolean
  - while stmt
    - `while ( x > 5) { ... }`
    - Same as if
- Blocks and Scopes
  - x disappears after the ending brace

```c++
{
    int x = 5;
}
```
- printing the location of semantic error
  ```c++
  30| int v = "String";
  ```
  Output: 
  
  ```sh
  Semantic Error:
     Error at 30:6 -> Type mismatch in declaration of v
  ```
