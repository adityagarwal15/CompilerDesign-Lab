# **Lab 1: Basic File Handling Operations**

## **Objectives**:
- Getting familiar with various file handling system calls.
- Ability to perform basic file operations in C.

---

## **Prerequisites**:
- Knowledge of the C programming language.
- Basic understanding of file operations in C.

---

## **Introduction**:
In any programming language, it is essential to understand file handling. In C, files represent a sequence of bytes stored on disk, and file operations like reading and writing are crucial for many applications. This lab covers basic file handling techniques in C, such as reading from and writing to files, copying contents between files, and other basic file manipulations.

---

## **Solved Exercise**:

### **Problem**: Copy contents from a source file to a destination file.

#### **Algorithm**:
1. Enter the source filename.
2. Check if the source file exists. If not, display an error message and exit.
3. Enter the destination filename.
4. Read each character from the source file and write it to the destination file.
5. Continue until the end of the source file (EOF).
6. Close both files.

---

## **Lab Exercises**:
### Write a C program for the following tasks:

1. **Count the number of lines and characters in a file**:
   - Open the file and traverse through its contents to count the number of lines and characters.

2. **Reverse the file contents and store it in another file**:
   - Read the file in reverse order and write the reversed contents to a new file.
   - Display the size of the file using appropriate file handling functions.

3. **Merge lines alternatively from two files and store them in a resultant file**:
   - Open both files and alternate between their lines to create a new file.

4. **Identify verbs in a given statement and perform operations in a hash table**:
   - **Insert**: Insert a verb into a hash table.
     - Syntax: `insert(char *str)`
   - **Search**: Search for a verb in the hash table.
     - Syntax: `int search(char *key)`
   - If the verb exists, return the hash value. If not, return -1.

---

## **Conclusion**:
This lab focused on file handling techniques in C, emphasizing tasks such as copying file contents, merging files, and performing basic operations with files. These exercises help build a foundation for handling files in real-world applications.

---

🚀 **Happy Learning and Coding!**
