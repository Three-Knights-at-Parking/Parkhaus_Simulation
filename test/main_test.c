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

int main(void) {
//     test_save_handler();
//     test_generics();
//     test_settings_all();
    test_demand();
  //  test_gate_routing();
    // test_statlist();
    // test_stats();
    // test_parkhaus();
    // test_simulation();
    printf("All test passed.");
    return 0;
}
