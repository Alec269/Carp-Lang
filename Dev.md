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

## Tokeniser

It takes source code:

```carp  
   int x = 5;
```

And turns it into tokens:

```cs
[T_int] [T_identifier(x)] [T_eq] [T_numLit(5)] [T_semi]
```

## Parser

A parser takes this (*tokens*):

`int x = 5;`

**Tokens**:

```cs
[T_int] [T_identifier(x)] [T_eq] [T_numLit(5)] [T_semi]
```

And turns it into a **structure**:

```sh
VariableDeclaration
 ├─ type: int
 ├─ name: x
 └─ value:
      NumberLiteral(5)
```

That *structured representation* is the **AST**.

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

[v0.0.20-Indev]

```carp

// variable declarations
int x = 5;
/* long y = 10; */
float z = 3;
string msg = "hello world";


if (x) {
   string msg = "value is five";
   msg = "jj";
}


while (x) {
   msg = "mono sodium glutamate";
   if (x) {
      int v = 6;
   }
}

```

Indev Compiler output:

```sh
TokenType order : 0 | Textual: 'int' Pos: 2:1
TokenType order : 11 | Textual: 'x' Pos: 2:5
TokenType order : 18 | Textual: '=' Pos: 2:7
TokenType order : 12 | Textual: '5' Pos: 2:9
TokenType order : 19 | Textual: ';' Pos: 2:10
TokenType order : 4 | Textual: 'float' Pos: 4:1
TokenType order : 11 | Textual: 'z' Pos: 4:7
TokenType order : 18 | Textual: '=' Pos: 4:9
TokenType order : 12 | Textual: '3' Pos: 4:11
TokenType order : 19 | Textual: ';' Pos: 4:12
TokenType order : 3 | Textual: 'string' Pos: 5:1
TokenType order : 11 | Textual: 'msg' Pos: 5:8
TokenType order : 18 | Textual: '=' Pos: 5:12
TokenType order : 13 | Textual: 'hello world' Pos: 5:14
TokenType order : 19 | Textual: ';' Pos: 5:27
TokenType order : 7 | Textual: 'if' Pos: 8:1
TokenType order : 29 | Textual: '(' Pos: 8:4
TokenType order : 11 | Textual: 'x' Pos: 8:5
TokenType order : 30 | Textual: ')' Pos: 8:6
TokenType order : 31 | Textual: '{' Pos: 8:8
TokenType order : 3 | Textual: 'string' Pos: 9:4
TokenType order : 11 | Textual: 'msg' Pos: 9:11
TokenType order : 18 | Textual: '=' Pos: 9:15
TokenType order : 13 | Textual: 'value is five' Pos: 9:17
TokenType order : 19 | Textual: ';' Pos: 9:32
TokenType order : 11 | Textual: 'msg' Pos: 10:4
TokenType order : 18 | Textual: '=' Pos: 10:8
TokenType order : 13 | Textual: 'jj' Pos: 10:10
TokenType order : 19 | Textual: ';' Pos: 10:14
TokenType order : 32 | Textual: '}' Pos: 11:1
TokenType order : 9 | Textual: 'while' Pos: 14:1
TokenType order : 29 | Textual: '(' Pos: 14:7
TokenType order : 11 | Textual: 'x' Pos: 14:8
TokenType order : 30 | Textual: ')' Pos: 14:9
TokenType order : 31 | Textual: '{' Pos: 14:11
TokenType order : 11 | Textual: 'msg' Pos: 15:4
TokenType order : 18 | Textual: '=' Pos: 15:8
TokenType order : 13 | Textual: 'mono sodium glutamate' Pos: 15:10
TokenType order : 19 | Textual: ';' Pos: 15:33
TokenType order : 32 | Textual: '}' Pos: 16:1
TokenType order : 35 | Textual: '' Pos: 18:1
VarDeclStmt
 type: int
 name: x
 initExpr:
  NumberExpr(5)
VarDeclStmt
 type: float
 name: z
 initExpr:
  NumberExpr(3)
VarDeclStmt
 type: string
 name: msg
 initExpr:
  StringExpr("hello world")
IfStmt
  condition:
  IdentExpr(x)
  then:
  BlockStmt
   VarDeclStmt
    type: string
    name: msg
    initExpr:
     StringExpr("value is five")
   AssignStmt
    name: msg
    value:
     StringExpr("jj")
WHileLoopStmt:
  condition:
  IdentExpr(x)
  body:
  BlockStmt
   AssignStmt
    name: msg
    value:
     StringExpr("mono sodium glutamate")
```

## Semantic Analyser

Semantic analysis answers questions like:

- Is this variable used before it’s declared?
- Is this variable declared twice in the same scope?
- Is string assigned to an int?
- Does if (x) make sense type-wise?
- Does while ("hello") make sense?
- Does inner var shadow outer var correctly?

The Parser cannot do this — and should not.
