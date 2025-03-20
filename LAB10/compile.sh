#!/bin/bash

# Remove old files
rm -f lex.yy.c bison.tab.c bison.tab.h parser

# Generate parser files with bison
bison -d bison.y

# Generate lexer with flex
flex bison.l

# Compile everything
gcc -Wall -o parser lex.yy.c bison.tab.c -lm

# Test if compilation was successful
if [ -f parser ]; then
    echo "Compilation successful. Run the parser with ./parser"
else
    echo "Compilation failed."
fi
