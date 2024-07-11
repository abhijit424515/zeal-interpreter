# Zeal : Monkey Language Intepreter in C++

This is my implementation of `Zeal`, an interpreter for the Monkey Language, made in C++. 

The [Monkey Language](./monkey.md) is a simple programming language that is used in the book `Writing an Interpreter in Go` by Thorsten Ball. The book is a great resource for learning how to write an interpreter for a programming language. I decided to implement the interpreter in C++ as a way to learn more about the language and to practice writing interpreters.

The implementation described in the book is written in Go, so I used `flex` and `bison` to generate the lexer and parser respectively. The generated lexer and parser codes are then used to parse the Monkey Language source code and generate an Abstract Syntax Tree (AST). The AST is then evaluated to produce the output of the program. 

Zeal uses an object model to represent entities like integers, doubles, strings, booleans, null and closures. 

# Build and Run

> To build Zeal, you need to have `flex` and `bison` installed on your system. 

Run `make`, which will generate the `zeal` executable.

# Latest Features

- Built-in functions

## What's Next

- Arrays
- Hashmaps