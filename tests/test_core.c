#include "test_core.h"

#include "../cargparse.h"

int total_tests = 0;
int passed_tests = 0;
int failed_tests = 0;

void
print_test_summary(void) {
    printf("\n=== TEST RESULTS ===\n");
    printf("Total:  %d\n", total_tests);
    printf("Passed: %d\n", passed_tests);
    printf("Failed: %d\n", failed_tests);

    if (failed_tests == 0) {
        printf("All tests passed!\n");
    }
}

int
cmp_options(const cargparse_option_t *opt1, const cargparse_option_t *opt2) {
    if (opt1->type != opt2->type) return 0;
    if (opt1->short_name != opt2->short_name) return 0;
    if (opt1->long_name && opt2->long_name && strcmp(opt1->long_name, opt2->long_name) != 0) return 0;
    if ((opt1->long_name && !opt2->long_name) || (!opt1->long_name && opt2->long_name)) return 0;
    if (opt1->help && opt2->help && strcmp(opt1->help, opt2->help) != 0) return 0;
    if ((opt1->help && !opt2->help) || (!opt1->help && opt2->help)) return 0;
    return 1;
}

int
cmp_parse_res(const cargparse_parse_res_t *pr1, const cargparse_parse_res_t *pr2) {
    if (pr1->is_got != pr2->is_got) return 0;
    if (pr1->valueint != pr2->valueint) return 0;
    if (pr1->valuefloat != pr2->valuefloat) return 0;
    if (pr1->valuestr && pr2->valuestr && strcmp(pr1->valuestr, pr2->valuestr) != 0) return 0;
    if ((pr1->valuestr && !pr2->valuestr) || (!pr1->valuestr && pr2->valuestr)) return 0;
    return 1;
}
