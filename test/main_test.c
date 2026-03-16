#include <stdio.h>

extern void test_generics(void);
extern void test_settings_all(void);
extern void test_save_handler(void);
extern void test_parkhaus(void);
extern void test_simulation(void);
extern void test_demand(void);
extern void test_gate_routing(void);
extern void test_stats(void);
extern void test_statlist(void);
extern void test_ui(void);
extern void test_ui_home(void);
extern void test_ui_help(void);
extern void test_ui_config(void);
extern void test_ui_simulation(void);
extern void test_ui_statistics(void);
extern void test_ui_storage(void);
extern void test_queue(void);
extern void test_rng(void);

int main(void) {
    test_save_handler();
    test_generics();
    test_settings_all();

    // UI TESTS
    // test_ui();
    // test_ui_home();
    // test_ui_help();
    // test_ui_config();
    // test_ui_simulation();
    // test_ui_statistics();
    // test_ui_storage();

    //SIMULATION TEST
    test_rng();
    test_gate_routing();
    test_queue();
    test_statlist();
    test_stats();
    test_parkhaus();
    test_simulation();

    printf("All test passed.");
    return 0;
}
