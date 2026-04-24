#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// gcc -Wall -pedantic -o number_digits number_digits.c

// gcc --help

int main() {
    char input[100];
    int digits[10] = {0}; // Array to keep track of which digits have appeared

    printf("Enter a number: ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        printf("Error reading input.\n");
        return 1;
    }

    // Remove trailing newline if present
    size_t len = strlen(input);
    if (len > 0 && input[len - 1] == '\n') {
        input[len - 1] = '\0';
    }

    printf("Digits in the number: ");
    for (size_t i = 0; i < strlen(input); i++) {
        if (input[i] >= '0' && input[i] <= '9') {
            int digit = input[i] - '0';
            if (!digits[digit]) {
                printf("%d ", digit);
                digits[digit] = 1;
            }
        }
    }
    printf("\n");

    return 0;
}