#include "test_core.h"

int total_tests = 0;
int passed_tests = 0;
int failed_tests = 0;

void print_test_summary(void) {
    printf("\n=== TEST RESULTS ===\n");
    printf("Total:  %d\n", total_tests);
    printf("Passed: %d\n", passed_tests);
    printf("Failed: %d\n", failed_tests);

    if (failed_tests == 0) {
        printf("All tests passed!\n");
    }
}
