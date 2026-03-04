#ifndef HOME_H
#define HOME_H

/*
 * File: home.h
 * Description: Declarations for the Home menu UI.
 */

#include "ui.h"

/* Maximum valid menu number on the Home screen (0..MAX_VALID_NUMBER). */
#define MAX_VALID_NUMBER 3

/**
 * @brief Prints the Home menu screen.
 */
void print_homescreen(void);

/**
 * @brief Handles user interaction in the Home menu.
 *
 * @return Next UI state depending on user selection.
 */
ui_state home_menu(void);

#endif /* HOME_H */