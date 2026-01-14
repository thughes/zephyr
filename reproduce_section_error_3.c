#include <stdio.h>

#define SECTION_ATTR __attribute__((section(".my_section")))

// Declaration WITH attribute
SECTION_ATTR extern int my_var;

// Definition WITHOUT attribute
int my_var = 10;

int main() {
    printf("Var: %d\n", my_var);
    return 0;
}
