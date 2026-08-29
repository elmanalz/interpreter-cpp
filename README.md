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

## Design

**Three stages that know nothing about each other.** The lexer turns characters into tokens and
has no idea what a valid expression looks like — it will happily produce the tokens for `1 + * 2`.
The parser turns tokens into a tree and never sees a character. The evaluator walks the tree and
never sees a token. Keeping them ignorant of one another is what makes each independently
testable, and it means the evaluator could be swapped for a code generator without touching the
parser.

**Precedence is not a table of numbers — it is the depth of each rule in the call graph.** A
level obtains its operands by calling the level below, and that lower level greedily consumes
everything at its own precedence before returning. So when `parse_expression` sees `+` in
`2 + 3 * 4` and calls `parse_term` for the right-hand side, `parse_term` swallows `3 * 4` whole
before control ever comes back. Multiplication binds tighter because it sits deeper in the
ladder, not because anything compares priorities.

**Associativity is the difference between a loop and a recursive call.** `parse_term` uses a
`while` loop and folds each new operand into an accumulator on the left, so `*`, `/` and `%` are
left-associative: `100/10/2` parses as `((100 / 10) / 2)` = 5. `factor` instead recurses for its
right operand, which greedily takes the rest of the chain, so `^` is right-associative:
`2^3^2` parses as `2^(3^2)` = 512, not `(2^3)^2` = 64. One is a loop, the other is recursion,
and that single difference is the entire mechanism.

**Parentheses need no special machinery.** `primary` is the deepest rule, and on seeing `(` it
calls `expression` — the shallowest. That resets the precedence ladder to the bottom, which is
exactly what parentheses mean. On the way out, the inner `expression` stops at `)` on its own,
because `)` is not one of its operators; `primary` then consumes it with `expect`. **The call
stack does the depth counting**, so there is no bracket counter anywhere in the code, and
mismatched parentheses are reported with an exact column.

**Unary sits above `^`, deliberately.** That makes `-2^2` parse as `-(2^2)` = −4, matching C++,
Python and standard mathematical convention, rather than `(-2)^2` = 4. Placing `unary` below
`factor` would flip it. The choice is recorded in a test.

**The grammar is the validator.** There are exactly two places that reject input: `primary`,
when the current token cannot begin an operand, and `expect`, when a required token is missing.
Between them they catch `2*`, `*3`, `2**3`, `42 43`, `(1+2`, `1+2)`, `()` and empty input, with
no hand-written rules. Note that "two operators cannot be adjacent" is *not* a rule anywhere —
it emerges, and it correctly still permits `2 * -3` and `1 - -2`, which a hand-written version of
that rule would wrongly reject.

**Ownership is `std::unique_ptr` throughout.** Each node owns its children, so destroying the
root frees the whole tree recursively and the word `delete` appears nowhere — including when a
parse error throws part-way through building a subtree, where a hand-managed version would leak.
`Node` has a virtual destructor, without which deleting through a base pointer would skip the
derived destructor and orphan every child below it.

**Single forward pass.** The cursor only ever moves forward and every token is consumed exactly
once, so parsing is O(n) in the number of tokens. Depth in the ladder is a constant, fixed by
the grammar rather than by the input.

## Build

Requires a C++17 compiler and `make`.

```bash
make            # build the REPL and the test suite
make run        # build and run the tests
make run-app    # build and run the REPL
make clean      # remove the binaries
```

Compiles with zero warnings under `-Wall -Wextra -pedantic`.

## Tests

74 checks in `tests.cpp`, using a small hand-written harness rather than an external framework.
Most assert on the **shape** of the parsed tree rather than the computed value, because a value
cannot distinguish a correct tree from an incorrect one — `2*3*4` is 24 under either
associativity, and only the parentheses reveal which was built.

The associativity cases deliberately avoid `+` and `*`, since both are commutative and
associative and would therefore pass whichever way the tree was assembled. `10-3-2` and
`100/10/2` do not have that problem.

21 of the checks are malformed inputs that must be rejected.

```
$ make run
74 / 74 checks passed
ALL TESTS PASSED
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

## Roadmap

- Variables (`x = 5`), which needs an environment and two-token lookahead
- Built-in functions: `sqrt`, `sin`, `max`
- Comparison operators, which slot in as one more precedence level above `expression`
- Convert the fixed ladder to **precedence climbing** — one function taking a minimum-precedence
  parameter plus an operator table, replacing the five levels. This is what Clang uses for
  binary expressions.
- A recursion-depth limit, so deeply nested parentheses produce an error rather than a stack
  overflow

## License

MIT.
