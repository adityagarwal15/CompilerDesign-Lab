#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    char c, buf[10];
    FILE *fp = fopen("digit.c", "r");  // Open the file

    if (fp == NULL) {  // Check if the file was opened successfully
        printf("Cannot open file\n");
        exit(0);
    }

    c = fgetc(fp);  // Read the first character

    while (c != EOF) {  // Continue reading till the end of file
        int i = 0;
        buf[0] = '\0';  // Reset buffer

        // Check for assignment operator or relational operator (==)
        if (c == '=') {
            buf[i++] = c;  // Add '=' to the buffer
            c = fgetc(fp);  // Read the next character
            if (c == '=') {  // If the next character is '='
                buf[i++] = c;  // Add '=' to the buffer
                buf[i] = '\0';
                printf("\nRelational operator: %s", buf);
            } else {  // If the next character is not '='
                buf[i] = '\0';
                printf("\nAssignment operator: %s", buf);
            }
        } 
        // Check for other relational operators (<, >, !=)
        else if (c == '<' || c == '>' || c == '!') {
            buf[i++] = c;  // Add the relational operator to the buffer
            c = fgetc(fp);  // Read the next character
            if (c == '=') {  // If the next character is '='
                buf[i++] = c;  // Add '=' to the buffer
                buf[i] = '\0';
                printf("\nRelational operator: %s", buf);
            } else {  // If the next character is not '='
                buf[i] = '\0';
                // Display the simple relational operator
                if (buf[0] == '<')
                    printf("\nRelational operator: Less Than (LT)");
                else if (buf[0] == '>')
                    printf("\nRelational operator: Greater Than (GT)");
                else if (buf[0] == '!')
                    printf("\nRelational operator: Not Equal (NE)");
            }
        } else {
            buf[i] = '\0';  // Reset the buffer if no operator is found
        }

        c = fgetc(fp);  // Read the next character
    }

    fclose(fp);  // Close the file after processing
    return 0;
}

