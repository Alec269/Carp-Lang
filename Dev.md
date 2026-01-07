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
