# **LAB 4: CONSTRUCTION OF SYMBOL TABLE**

## **Objectives**:
- To design a lexical analyzer.
- To recognize various lexemes and generate their corresponding tokens.
- To store the tokens in the symbol table.

---

## **Prerequisites**:
- Knowledge of the C programming language.
- Knowledge of file pointers.
- Knowledge of data structures.

---

## **Introduction**:
A **symbol table** is a crucial data structure used in compilers and interpreters to store information about identifiers (such as variable names, function names, etc.) in a source program along with their attributes. It is created during the **lexical analysis** phase and updated in the **syntax and semantic analysis** phases.

### **Attributes Stored in a Symbol Table**:
- **Variable Name**: Name of the identifier.
- **Variable Type**: Data type of the identifier.
- **Size**: Memory size occupied by the variable.
- **Scope**: Visibility and lifetime of the variable.
- **Arguments**: If applicable, the function parameters.
- **Number of Arguments**: Count of function parameters.
- **Return Type**: Return type of a function.

A **hash table** is commonly used for symbol table implementation due to its fast lookup capability.

---

## **Expected Output**:

---

## **Conclusion**:
In this lab, we implemented a **symbol table** using a simple C program. We learned how to:
- Identify and store lexemes as tokens.
- Maintain attributes like type, size, and scope.
- Use data structures (array-based hash table) for efficient lookup and storage.

This knowledge is essential for **compiler design**, especially in **lexical analysis and parsing**.

---

🚀 **Happy Coding!** 🎯

