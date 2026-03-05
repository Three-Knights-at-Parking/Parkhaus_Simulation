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

validation_flag validate_user_input(int user_choice, int max_valid_number)
{
    if (user_choice < 0)
    {
        printf("Your input is not a valid integer!\n");
        printf("Please press ENTER and try again...\n");

        return INVALID;
    }

    if (user_choice > max_valid_number)
    {
        printf("The number you entered is invalid!\n");
        printf("Please only choose between the numbers displayed.\n");
        printf("Press ENTER and try again...\n");

        return INVALID;
    }

    return VALID;
}