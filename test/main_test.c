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
extern void test_config_file_handler(void);
extern void test_car(void);
extern void test_vehicle_list_generic(void);

int main(void) {


    // UI TESTS
    test_ui();
    test_ui_home();
    test_ui_help();
    test_ui_config();
    test_ui_simulation();
    test_ui_statistics();
    test_ui_storage();

    printf("---------------------------------------- \n");
    printf("All UI Tests passed. \n");
    printf("---------------------------------------- \n");

    test_save_handler();
    test_generics();
    test_settings_all();
    test_statlist();
    test_rng();
    test_config_file_handler();
    test_vehicle_list_generic();
    test_car();

    printf("---------------------------------------- \n");
    printf("All Generic Tests passed. \n");
    printf("---------------------------------------- \n");

    //SIMULATION TEST
    test_demand();
    test_gate_routing();
    test_queue();
    test_stats();
    test_parkhaus();
    test_simulation();

    printf("---------------------------------------- \n");
    printf("All Simulation Tests passed. \n");
    printf("---------------------------------------- \n");
    printf("All test passed.");
    return 0;
}
