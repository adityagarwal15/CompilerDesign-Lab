# FLEX Lab Exercises

This repository contains a series of FLEX programming exercises that demonstrate how to perform lexical analysis on different types of input and process them using regular expressions. FLEX (Fast Lexical Analyzer) is a tool used to generate scanners (lexical analyzers) based on regular expressions.

## Lab Exercises

Below are the tasks performed in this repository:

### 1. Count the number of vowels and consonants in the given input.

In this exercise, a FLEX program is written to count the number of vowels and consonants in a given string. The program uses regular expressions to match vowels and consonants, then tracks the counts as it processes the input.

**Key Concepts:**
- Matching vowels and consonants using regular expressions.
- Using the `yytext` and `fputc()` functions to print the results.

### 2. Count the number of words, characters, blanks, and lines in a given text.

This program reads a text file and counts the number of words, characters, spaces, and lines. It utilizes regular expressions to count the number of words based on spaces and newline characters.

**Key Concepts:**
- Using the `\n` character to identify line breaks.
- Counting words by separating text on spaces.
- Handling different kinds of whitespace (spaces, newlines).

### 3. Find the number of positive integers, negative integers, positive floating-point numbers, and negative floating-point numbers.

This FLEX program detects and counts positive integers, negative integers, positive floating-point numbers, and negative floating-point numbers in a given text. Regular expressions are used to match the different number formats, and counts are updated accordingly.

**Key Concepts:**
- Regular expressions for detecting integers and floating-point numbers.
- Handling negative numbers with the '-' symbol.

### 4. Given an input C file, replace all `scanf` with `READ` and `printf` with `WRITE` statements, also find the number of `scanf` and `printf` in the file.

This FLEX program processes a C file, replacing all occurrences of `scanf` with `READ` and `printf` with `WRITE`. Additionally, it counts how many times these functions appear in the code.

**Key Concepts:**
- Tokenizing C function calls (`scanf` and `printf`) and replacing them.
- Tracking the number of replacements made.

### 5. Convert a number from decimal to hexadecimal notation.

This FLEX program reads a text file, identifies decimal numbers, and converts them to their hexadecimal equivalents. The output is written to a new file with the hexadecimal format `0xXXXX`.

**Key Concepts:**
- Converting decimal numbers to hexadecimal using `fprintf()`.
- Writing the output in the desired format.

### 6. Convert uppercase characters to lowercase characters of a C file excluding the characters present in the comments.

In this exercise, the program processes a C file and converts all uppercase letters to lowercase, while preserving the case of characters inside comments (i.e., code comments like `//` or `/* */`). The program uses regular expressions to differentiate between code and comment sections.

**Key Concepts:**
- Identifying comments using regular expressions.
- Transforming text while excluding comment sections.

---

## How to Use

To use the FLEX programs, follow the instructions below:

### 1. Install FLEX (if not already installed)
You can install FLEX on Ubuntu using the following command:

```bash
sudo apt-get install flex
```


### 2. Compile the FLEX programs

For each .l file, use the following commands to compile and run the program:

```flex <filename>.l
gcc lex.yy.c -o <output_program_name> -lfl
./<output_program_name> <input_file> <output_file>
```

