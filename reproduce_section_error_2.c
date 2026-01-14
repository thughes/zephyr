#include <stdio.h>

#define SECTION_ATTR __attribute__((section(".my_section")))

// Declaration with attribute
SECTION_ATTR extern int my_var;

// Definition with DIFFERENT attribute (or same) - testing if repetition matters or mismatch
// Mismatch:
// __attribute__((section(".other_section"))) int my_var = 10;

// Same:
SECTION_ATTR int my_var = 10;

int main() {
    printf("Var: %d\n", my_var);
    return 0;
}
