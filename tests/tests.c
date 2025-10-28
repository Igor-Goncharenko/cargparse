#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "test_core.h"
#include "../cargparse.h"

CARGPARSE_INIT(test_argparse,
    "test [OPTION]... [FILE]..\ntest [FILE]...",
    "Description example.",
    "Epilog example.",
    CARGPARSE_OPTION_INT('n', "number", "number of something"),
    CARGPARSE_OPTION_BOOL(-1, "bool", "bool for something"),
    CARGPARSE_OPTION_STRING(-1, "some-str", "some string"),
    CARGPARSE_OPTION_FLOAT('f', "float", "some float"),
    CARGPARSE_OPTION_POSITIONAL("positional1", "positional argument example"),
);

int test_argparse_init_test(void) {
    int i;
    /* and init by hand */
    const cargparse_option_t hand_init_opts[5] = {
        { CARGPARSE_OPTION_TYPE_INT, 'n', "number", "number of something" },
        { CARGPARSE_OPTION_TYPE_BOOL, -1, "bool", "bool for something" },
        { CARGPARSE_OPTION_TYPE_STR, -1, "some-str", "some string" },
        { CARGPARSE_OPTION_TYPE_FLOAT, 'f', "float", "some float" },
        { CARGPARSE_OPTION_TYPE_POS, -1, "positional1", "positional argument example" },
    };
    cargparse_parse_res_t hand_init_parse_res[5] = { 0 };
    const cargparse_t hand_init_test_argparse = {
        "test [OPTION]... [FILE]..\ntest [FILE]...",
        "Description example.",
        "Epilog example.",
        hand_init_opts,
        hand_init_parse_res,
        5
    };

    /* compare macro init and hand init */
    TEST_EQ_STR(test_argparse.usages, hand_init_test_argparse.usages);
    TEST_EQ_STR(test_argparse.description, hand_init_test_argparse.description);
    TEST_EQ_STR(test_argparse.epilog, hand_init_test_argparse.epilog);
    TEST_EQ(test_argparse.n_options, hand_init_test_argparse.n_options);
    for (i = 0; i < test_argparse.n_options; i++) {
        TEST(cmp_options(&test_argparse.options[i], &hand_init_test_argparse.options[i]));
    }

    return 0;
}

int init_null_test(void) {
    CARGPARSE_INIT(test_ap, NULL, NULL, NULL,
            CARGPARSE_OPTION_INT(-1, NULL, NULL)
    );

    TEST_EQ(test_ap.usages, (const char*)NULL);
    TEST_EQ(test_ap.description, (const char*)NULL);
    TEST_EQ(test_ap.epilog, (const char*)NULL);
    TEST_EQ(test_ap.n_options, 1);

    cargparse_option_t opt = { CARGPARSE_OPTION_TYPE_INT, -1, NULL, NULL };
    TEST(cmp_options(&test_ap.options[0], &opt));

    return 0;
}

int option_init_test(void) {
    const cargparse_option_t o1m = CARGPARSE_OPTION_BOOL('b', "bool", "some bool");
    const cargparse_option_t o1h = {CARGPARSE_OPTION_TYPE_BOOL, 'b', "bool", "some bool"};
    TEST(cmp_options(&o1m, &o1h));

    const cargparse_option_t o2m = CARGPARSE_OPTION_INT('i', "int", "some int");
    const cargparse_option_t o2h = {CARGPARSE_OPTION_TYPE_INT, 'i', "int", "some int"};
    TEST(cmp_options(&o2m, &o2h));

    const cargparse_option_t o3m = CARGPARSE_OPTION_FLOAT('f', "float", "some float");
    const cargparse_option_t o3h = {CARGPARSE_OPTION_TYPE_FLOAT, 'f', "float", "some float"};
    TEST(cmp_options(&o3m, &o3h));

    const cargparse_option_t o4m = CARGPARSE_OPTION_STRING('s', "string", "some string");
    const cargparse_option_t o4h = {CARGPARSE_OPTION_TYPE_STR, 's', "string", "some string"};
    TEST(cmp_options(&o4m, &o4h));

    const cargparse_option_t o5m = CARGPARSE_OPTION_POSITIONAL("positional", "some positional");
    const cargparse_option_t o5h = {CARGPARSE_OPTION_TYPE_POS, -1, "positional", "some positional"};
    TEST(cmp_options(&o5m, &o5h));

    return 0;
}

int test_argparse_all_opts(void) {
    int i;
    char *argv[] = {
        "program", "-n", "10", "--bool", "--some-str", "Some str", "--float", "8.89", "--", "pos1"
    };
    cargparse_parse(&test_argparse, sizeof(argv) / sizeof(char*), argv);

    const cargparse_parse_res_t parse_res[] = {
        { true, 10, 0.0, NULL },
        { true, 0, 0.0, NULL },
        { true, 0, 0.0, "Some str" },
        { true, 0, 8.89, NULL },
        { true, 0, 0.0, "pos1" },
    };
    for (i = 0; i < test_argparse.n_options; i++) {
        TEST(cmp_parse_res(&test_argparse.parse_res[i], &parse_res[i]));
    }
    CARGPARSE_PARSE_RES_CLEANUP(&test_argparse);

    return 0;
}

int test_argparse_no_opts(void) {
    int i;
    char *argv[] = { "program" };
    cargparse_parse(&test_argparse, sizeof(argv) / sizeof(char*), argv);

    const cargparse_parse_res_t parse_res[] = {
        { false, 0, 0.0, NULL },
        { false, 0, 0.0, NULL },
        { false, 0, 0.0, NULL },
        { false, 0, 0.0, NULL },
        { false, 0, 0.0, NULL },
    };
    for (i = 0; i < test_argparse.n_options; i++) {
        TEST(cmp_parse_res(&test_argparse.parse_res[i], &parse_res[i]));
    }
    CARGPARSE_PARSE_RES_CLEANUP(&test_argparse);

    return 0;
}

int test_argparse_short_name_errors(void) {
    /* TODO: Replace -1 if cargparse_err_t type will be created */

    /* only non-existent options */
    TEST_PARSE_ERROR(&test_argparse, -1, "-a");
    /* non-existent options */
    TEST_PARSE_ERROR(&test_argparse, -1, "-a", " 12341");
    /* non-existent options after positional */
    TEST_PARSE_ERROR(&test_argparse, -1, "12341", "-a");
    /* nothing after int option */
    TEST_PARSE_ERROR(&test_argparse, -1, "-n");
    /* not number after int option */
    TEST_PARSE_ERROR(&test_argparse, -1, "-n", "ffdsaf");
    /* other option after int option */
    TEST_PARSE_ERROR(&test_argparse, -1, "-n", "--float", "89.99");

    return 0;
}

int test_argparse_long_name_errors(void) {
    /* TODO: Replace -1 if cargparse_err_t type will be created */

    /* only non-existent options */
    TEST_PARSE_ERROR(&test_argparse, -1, "--non-existent");
    /* non-existent options */
    TEST_PARSE_ERROR(&test_argparse, -1, "--non-existent", "value1");
    /* non-existent options after positional */
    TEST_PARSE_ERROR(&test_argparse, -1, "positional1", "--non-existent" );
    /* nothing after int option */
    TEST_PARSE_ERROR(&test_argparse, -1, "--float");
    /* not number after int option */
    TEST_PARSE_ERROR(&test_argparse, -1, "--float", "ffdsaf");
    /* other option after int option */
    TEST_PARSE_ERROR(&test_argparse, -1, "--float", "--float", "89.99");

    return 0;
}

int main(void) {
    printf("\nRunning tests...\n");

    RUN_TEST(test_argparse_init_test);
    RUN_TEST(init_null_test);
    RUN_TEST(option_init_test);
    RUN_TEST(test_argparse_all_opts);
    RUN_TEST(test_argparse_no_opts);
    RUN_TEST(test_argparse_short_name_errors);
    RUN_TEST(test_argparse_long_name_errors);

    print_test_summary();

    return 0;
}
