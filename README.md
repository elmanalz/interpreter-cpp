# expr-interpreter

A recursive-descent parser and evaluator for arithmetic expressions, written from scratch in
C++17 with no dependencies. Lexer, parser, abstract syntax tree, and tree-walking evaluator —
the same pipeline a real compiler front-end uses, at a size one person can read.

```
$ ./calc
6^6*2*(8+4)^2/36
((((6 ^ 6) * 2) * ((8 + 4) ^ 2)) / 36)
373248

2 + 3 * (4 - 1) ^ 2 ^ 2 / -6
(2 + ((3 * ((4 - 1) ^ (2 ^ 2))) / (-6)))
-38.5

1 + * 2
Parser Error: Expected Number, parenthesis, or identifier at 4.
```

Each input prints twice: the parsed expression, fully parenthesised, and then its value. The
parentheses are not decoration — they show the tree that was built, which is how the parser is
tested.

## Features

- Five precedence levels: `+ -`, `* / %`, unary `- +`, `^`, and atoms
- Correct associativity in both directions — `10-3-2` is `5`, `2^3^2` is `512`
- Parentheses nested to any depth
- Chained unary operators (`--5`, `2 * -3`, `2^-1`)
- Floating-point throughout, with `fmod` for `%`
- Errors report the column, and no malformed input is silently accepted
- Division and modulo by zero are rejected rather than producing `inf` or crashing

## The grammar

```
expression  →  term  ( ('+' | '-') term )*
term        →  unary ( ('*' | '/' | '%') unary )*
unary       →  ('-' | '+') unary  |  factor
factor      →  primary ( '^' unary )?
primary     →  NUMBER | IDENTIFIER | '(' expression ')'
```

Each rule becomes one function. That is the whole technique.

## Build

Requires a C++17 compiler and `make`.

```bash
make            # build the REPL and the test suite
make run        # build and run the tests
make run-app    # build and run the REPL
make clean      # remove the binaries
```

## Layout

```
token.h      TokenKind and Token
lexer.h      the lexer's public interface: one function
lexer.cpp    lex(), plus its helpers in an anonymous namespace
ast.h        Node and the four node types
ast.cpp      to_string() and eval() for each node
parser.h     the Parser class
parser.cpp   the five parse functions and the cursor helpers
main.cpp     the REPL
tests.cpp    test suite
Makefile     build rules
```

## License

MIT.
