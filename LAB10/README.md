# Lab 10 - Programs on Bison

This repository contains a series of programs written using **Bison**, a parser generator for context-free grammars. Bison is used to build parsers for different types of inputs, enabling the validation of expressions, control structures, and variable declarations.

## Lab Exercises

Below are the tasks performed in this repository:

### 1. Check a Valid Declaration Statement

In this exercise, a Bison program is written to check if a variable declaration is syntactically correct. It validates various data types such as `int`, `float`, `char`, and `double`, as well as array declarations.

**Key Concepts:**
- Declaration of variables and arrays.
- Checking syntax for variable declaration statements.

### 2. Check a Valid Decision-Making Statement

This Bison program validates decision-making statements like `if`, `else`, `while`, `for`, and `do-while` loops. The program ensures proper syntax for these control structures.

**Key Concepts:**
- Syntax validation for `if`, `else`, `while`, `for`, and `do-while` loops.
- Conditional expressions and blocks of code.

### 3. Evaluate Arithmetic Expressions Involving Operations `+`, `-`, `*`, and `/`

This Bison program parses and evaluates arithmetic expressions in infix notation. It supports the four basic arithmetic operators: addition, subtraction, multiplication, and division.

**Key Concepts:**
- Evaluating infix arithmetic expressions.
- Operator precedence and handling division by zero.

### 4. Validate and Evaluate Postfix Notation

This Bison program validates and evaluates expressions in **postfix notation**. It handles basic arithmetic operations such as addition, subtraction, multiplication, and division in postfix format.

**Key Concepts:**
- Parsing and evaluating postfix expressions.
- Using a stack-based approach for postfix notation.

---

## How to Use

To use the Bison programs, follow the instructions below:

### 1. Install Bison and Flex (if not already installed)

You can install **Bison** and **Flex** on Ubuntu using the following commands:

```bash
sudo apt-get install bison flex
```

## 2. Compile and Run the Bison Programs

Follow these steps to compile and run the programs:

    1. Write the Bison Program (bison.y) and the Flex Program (bison.l).

    2. Generate the Bison Parser: Compile the Bison program to generate the C source code:

    ```
    bison -d bison.y
    ```

    This generates two files: bison.tab.c (C source) and bison.tab.h (header file).

    3. Compile the Flex Program: Compile the Flex program:

    flex bison.l

    4. Compile the C Files: After generating the C files from Bison and Flex, compile them together with GCC:

    gcc lex.yy.c bison.tab.c -o parser -lm

    5. Run the Program: Finally, run the program and provide the appropriate input:

    ./parser

3. 3. Example Usage

Once the program is running, you will be prompted with the following instructions:


Multi-purpose Parser
Commands:
1. To validate declaration: 'decl' followed by a declaration statement
2. To validate decision making: 'decision' followed by a control structure
3. To evaluate infix expression: 'infix' followed by an expression
4. To evaluate postfix expression: 'postfix' followed by an expression
Example: 'infix 2 + 3 * 4'

Examples:

    To check a valid declaration:

decl int a, b;

To validate a decision-making statement:

decision if (a > b) { return 1; } else { return 0; }

To evaluate an infix expression:

infix 2 + 3 * 4

To evaluate a postfix expression:

postfix 2 3 4 * +





---

You can copy and paste this code into your `README.md` file. It should provide a complete, clear, and professional overview of your **Lab 10 - Programs on Bison** repository. Let me know if you need any further modifications!



    

