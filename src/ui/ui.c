#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/ui/ui.h"

int user_input(void)
{
    char buffer[64];
    long value = 0;

    printf("Enter the number (int) you want to navigate to: ");

    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return -1;
    }

    value = strtol(buffer, &endptr, 10);

    return (int)value;
}