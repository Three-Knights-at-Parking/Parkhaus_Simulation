#include <stdio.h>

extern void test_generics(void);
extern void test_settings_all(void);
extern void test_save_handler(void);

int main(void) {
    test_save_handler();
    test_generics();
    test_settings_all();
    printf("All test passed.");
    return 0;
}
