# **Lab 3: Construction of Token Generator**

## **Objectives**:
- To design a token generator.
- To recognize various lexemes and generate their corresponding tokens.

---

## **Prerequisites**:
- Knowledge of the C programming language.
- Knowledge of file pointers.
- Knowledge of data structures.

---

## **Introduction**:
In this lab, we focus on lexical analysis, which is the first phase of the compilation process. A lexical analyzer, or scanner, is responsible for recognizing lexemes and converting them into tokens. Tokens are symbolic names for elements of the source program, such as keywords, operators, and identifiers. The task of the lexical analyzer is to read the source code and generate these tokens, which will later be processed by the syntax analyzer.

A **token** is the smallest unit of meaningful data in a program, such as:
- **Keywords** (e.g., `if`, `int`)
- **Identifiers** (e.g., variable names)
- **Constants** (e.g., numbers)
- **Operators** (e.g., `+`, `-`, `=`)

The program generates tokens based on predefined patterns. For example, the lexeme `if` matches the pattern for the keyword `if`.

---

## **Solved Exercise**:

### **Problem**: Identification of Arithmetic and Relational Operators

#### **Algorithm**:
1. Open the input C file in read mode.
2. Check if the file exists; display an error message if it doesn't.
3. Read each character from the input file.
4. If the character is '=', check if the next character is also '='. If so, identify it as a relational operator, otherwise as an assignment operator.
5. If the character is '<', '>', or '!', check for '=' in the next character to identify relational operators like LTE, GTE, or NE.
6. Repeat the process until the end of the file (EOF) is encountered.
7. Output the identified operators.

> **See the code for the solved exercise here:** [solved_exercise folder](./SOLVED_EXERCISE)

---

## **Lab Exercises**:

### **1. Identify and Categorize Tokens**:
- Write functions to identify and categorize the following tokens:
  - Arithmetic operators (`+`, `-`, `*`, `/`)
  - Relational operators (`<`, `>`, `<=`, `>=`, `==`, `!=`)
  - Logical operators (`&&`, `||`, `!`)
  - Special symbols (e.g., `;`, `,`, `{`, `}`)
  - Keywords (e.g., `int`, `float`, `if`, `while`)
  - Numerical constants
  - String literals
  - Identifiers

> **See the code for Q1 here:** [q1 folder](./Q1)

### **2. Design a Lexical Analyzer**:
- Design a lexical analyzer that includes a `getNextToken()` function for processing a simple C program.
- The `getNextToken()` function must:
  - Generate a token structure containing the row number, column number, and token type for each identified token.
  - Ignore tokens inside single-line or multi-line comments.
  - Ignore tokens inside string literals.
  - Strip out preprocessor directives (e.g., `#define`, `#include`).

> **See the code for Q2 here:** [q2 folder](./Q2)

---

## **Conclusion**:
This lab introduces you to the concept of lexical analysis and token generation, which are essential steps in the compilation process. You learned how to design a token generator, recognize lexemes, and generate tokens for different components of a C program. These exercises are foundational in understanding how compilers analyze source code.

---

🚀 **Happy Learning and Coding!**
