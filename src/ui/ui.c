#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../include/ui/ui.h"

int user_input(void)
{
    char buffer[64];
    char *endptr = NULL;
    long value = 0;

    printf("Enter the number (int) you want to navigate to: ");

    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return -1;
    }

    errno = 0;
    value = strtol(buffer, &endptr, 10);

    /* No digits were found - endptr points to the first element of buffer*/
    if (endptr == buffer)
    {
        return -1;
    }

    /* Overflow/underflow */
    if (errno != 0)
    {
        return -1;
    }

    return (int)value;
}