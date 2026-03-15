#include <stdio.h>

extern void test_generics(void);
extern void test_settings_all(void);
extern void test_save_handler(void);

extern void test_ui(void);
extern void test_ui_home(void);
extern void test_ui_help(void);
extern void test_ui_config(void);
extern void test_ui_simulation(void);
extern void test_ui_statistics(void);
extern void test_ui_storage(void);

int main(void) {
    test_save_handler();
    test_generics();
    test_settings_all();

    test_ui();
    test_ui_home();
    test_ui_help();
    test_ui_config();
    test_ui_simulation();
    test_ui_statistics();
    test_ui_storage();

    printf("All test passed.");
    return 0;
}
