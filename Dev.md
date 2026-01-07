# Compiler pipeline

```sh

'Source code'
   ↓
'Tokenizer / Lexer'        → characters → tokens
   ↓
'Parser'                   → tokens → AST
   ↓
'Semantic analysis'        → AST → checked AST
   ↓
'Codegen / Interpreter'    → AST → machine code / bytecode / execution

```

## What a tokeniser does

It takes source code:

```carp  
   int x = 5;
```

And turns it into tokens:

```cs
[T_int] [T_identifier(x)] [T_eq] [T_numLit(5)] [T_semi]
```

## What a parser actually does

A parser takes this:

`int x = 5;`

Tokens:

```cs
[T_int] [T_identifier(x)] [T_eq] [T_numLit(5)] [T_semi]
```

And turns it into structure:

```sh
VariableDeclaration
 ├─ type: int
 ├─ name: x
 └─ value:
      NumberLiteral(5)
```

That structured representation is the AST.

## Why we need an AST

Tokens are flat:

```sh
   x == y + 3 * z
```

Tokens don’t tell you:

- which operator runs first?
- what is inside what?
- what is a statement vs expression?

The AST makes this explicit:

```sh
BinaryExpr (==)
 ├─ Identifier(x)
 └─ BinaryExpr (+)
     ├─ Identifier(y)
     └─ BinaryExpr (*)
         ├─ Number(3)
         └─ Identifier(z)
```

## Parse tree vs AST (important)

Parse tree (too detailed)

- Includes every grammar rule, parentheses, etc.

AST (what's needed)

- Removes useless syntax
- Keeps only meaning

---

[v0.0.2]

```carp

```

Compiler output:

```sh
TokenType order : 0 | Textual: 'int' Pos: 2:1
TokenType order : 12 | Textual: 'x' Pos: 2:5
TokenType order : 19 | Textual: '=' Pos: 2:7
TokenType order : 13 | Textual: '5' Pos: 2:9
TokenType order : 20 | Textual: ';' Pos: 2:10
TokenType order : 4 | Textual: 'float' Pos: 4:1
TokenType order : 12 | Textual: 'z' Pos: 4:7
TokenType order : 19 | Textual: '=' Pos: 4:9
TokenType order : 13 | Textual: '3' Pos: 4:11
TokenType order : 20 | Textual: ';' Pos: 4:12
TokenType order : 3 | Textual: 'string' Pos: 5:1
TokenType order : 12 | Textual: 'msg' Pos: 5:8
TokenType order : 19 | Textual: '=' Pos: 5:12
TokenType order : 14 | Textual: 'hello world' Pos: 5:14
TokenType order : 20 | Textual: ';' Pos: 5:27
TokenType order : 36 | Textual: '' Pos: 6:1
VarDeclStmt
 type: int
 name: x
 init:
  NumberExpr(5)
VarDeclStmt
 type: float
 name: z
 init:
  NumberExpr(3)
VarDeclStmt
 type: string
 name: msg
 init:
  StringExpr("hello world")
```
