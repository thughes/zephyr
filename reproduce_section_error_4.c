#include <stdio.h>

#define SECTION_ATTR __attribute__((section(".my_section")))

// Declaration WITHOUT attribute
extern int my_var;

// Definition WITH attribute
SECTION_ATTR int my_var = 10;

int main() {
    printf("Var: %d\n", my_var);
    return 0;
}
