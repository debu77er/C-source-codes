#include <stdio.h>

int main() {
    int n = 10; // Number of Fibonacci numbers to generate
    int a = 0;  // First Fibonacci number
    int b = 1;  // Second Fibonacci number
    int next;

    printf("Fibonacci sequence up to %d terms:\n", n);
    printf("%d ", a);
    printf("%d ", b);

    for (int i = 2; i < n; i++) {
        // Use inline assembly to compute next = a + b
        __asm__ (
            "addl %%ebx, %%eax;"
            : "=a" (next)     // Output: next Fibonacci number
            : "a" (a), "b" (b) // Inputs: a and b
        );

        printf("%d ", next);

        // Update a and b for next iteration
        a = b;
        b = next;
    }

    return 0;
}
