# **Lab 2: Preliminary Scanning Applications**

## **Objectives**:
- To understand the basics of the scanning process.
- Ability to preprocess the input file so that it becomes suitable for compilation.

---

## **Prerequisites**:
- Knowledge of the C programming language.
- Knowledge of file pointers and string handling functions.

---

## **Introduction**:
In this lab, we focus on preprocessing an input file so that it becomes suitable for the scanning process. Preprocessing involves removing blank spaces, tabs, preprocessor directives, and comments from the given input C file. This is a crucial step before the compilation phase, where the code is cleaned up for further processing.

---

## **Solved Exercise**:

### **Problem**: Removal of Single and Multiline Comments

#### **Algorithm**:
1. Open the input C file in read mode.
2. Check if the file exists. If it doesn’t exist, display an error message and exit.
3. Open the output file in write mode.
4. Read each character from the input file.
5. If the character is '/', check if it is a start of a comment.
   - If the next character is '/', continue reading until a newline is encountered.
   - If the next character is '*', continue reading until the next '*' and check for a '/'. 
6. If it's not a comment, write the character to the output file.
7. Repeat the process until the end of the file (EOF).
8. Close both files.

---

## **Lab Exercises**:
### Write a C program for the following tasks:

1. **Replace blank spaces and tabs with a single space**:
   - Take a file as input and preprocess it to replace all blank spaces and tabs with a single space. Write the output to a new file.

2. **Discard Preprocessor Directives**:
   - Write a program that removes preprocessor directives (like `#include`, `#define`, etc.) from a given C file.

3. **Recognize C Keywords and Print Them in Uppercase**:
   - Take a C program as input, identify all the keywords, and print them in uppercase in the output.

---

## **Conclusion**:
This lab focused on the basics of scanning and preprocessing files for compilation. It involved removing comments, blank spaces, and preprocessor directives, as well as recognizing and modifying C keywords. These tasks are vital in understanding the preprocessing phase in compiler design.

---

🚀 **Happy Learning and Coding!**
