#include <stdio.h>

int main() {
    int a = 5;
    int b = 10;
    int sum;

    // Inline assembly to add a and b
    __asm__ (
        "addl %%ebx, %%eax;"   // Add ebx to eax
        : "=a" (sum)           // Output operand: sum stored in eax
        : "a" (a), "b" (b)     // Input operands: a in eax, b in ebx
    );

    printf("Sum of %d and %d is %d", a, b, sum);
    return 0;
}
