# Scheme Interpreter

## Overview
Welcome to the Scheme Interpreter project! This is a lightweight and efficient interpreter for the Scheme programming language, built in C++. It supports key Scheme features such as lexical scoping, variable definitions, list operations, arithmetic functions, and lambda expressions.

## Features
### Tokenizer
- Breaks down input into meaningful components: numbers, symbols, brackets, quotes, and special tokens.
- Recognizes integer constants, mathematical symbols, and Scheme-specific characters.
- Skips extra spaces and correctly handles whitespace.
- Works with streaming input, processing tokens as they arrive.

### Parser
- Converts tokens into a structured abstract syntax tree (AST).
- Understands numbers, symbols, and core Scheme expressions.
- Supports lists and pairs, including dotted pair notation.
- Detects and reports syntax errors like unbalanced parentheses and incorrect list structures.

### Evaluation
- Follows Scheme evaluation rules, handling self-evaluating expressions (numbers, booleans, lists).
- Supports defining variables (`define`), modifying them (`set!`), and lexical scoping.
- Includes built-in arithmetic operations (`+`, `-`, `*`, `/`), comparisons (`=`, `<`, `>`, `<=`, `>=`), and logical operators (`and`, `or`, `not`).
- Provides essential list functions like `cons`, `car`, `cdr`, `list`, `list-ref`, and `list-tail`.

### Lambda Expressions
- Implements anonymous functions (`lambda`) with closure support.
- Allows nested function definitions and mutual recursion.
- Supports function redefinition and proper scoping.
- Includes shorthand function definitions using `(define (func-name args...) body...)`.

### Special Forms
- Implements `if` expressions with correct evaluation order and error handling.
- Supports `quote` and `quasiquote` for working with list literals.
- Provides `begin` for sequentially evaluating multiple expressions.
- Includes predicates like `pair?`, `list?`, and `null?` to check data types.

### Error Handling
- Detects and raises errors for syntax issues (`SyntaxError`), runtime errors (`RuntimeError`), and undefined variables (`NameError`).
- Prevents evaluation of invalid expressions and provides clear error messages.

## Example Usage
```scheme
(define x 10)
(define y 5)
(+ x y) ; Returns 15

(define (square n) (* n n))
(square 4) ; Returns 16

(if (> x y) "x is greater" "y is greater")

(define (factorial n) (if (= n 0) 1 (* n (factorial (- n 1)))))
(factorial 5) ; Returns 120
```

## Installation
1. Clone the repository:
   ```sh
   git clone https://github.com/pshpsh76/scheme-interpreter.git
   cd scheme-interpreter
   ```
2. Build the project using CMake:
   ```sh
   mkdir build
   cd build
   cmake ..
   make scheme_intepreter
   ```
3. Start the interpreter:
   ```sh
   ./scheme_interpreter
