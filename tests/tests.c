#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../cargparse.h"
#include "test_core.h"

/* clang-format off */
CARGPARSE_INIT(test_argparse,
    "test [OPTION]... [FILE]..\ntest [FILE]...",
    "Description example.",
    "Epilog example.",
    CARGPARSE_OPTION_INT('n', "number", "number of something"),
    CARGPARSE_OPTION_BOOL(CARGPARSE_NO_SHORT, "bool", "bool for something"),
    CARGPARSE_OPTION_STRING(CARGPARSE_NO_SHORT, "some-str", "some string"),
    CARGPARSE_OPTION_FLOAT('f', "float", "some float"),
    CARGPARSE_OPTION_POSITIONAL("positional1", "positional argument example"),
    CARGPARSE_OPTION_POSITIONAL("positional2", "positional argument example"),
    CARGPARSE_OPTION_POSITIONAL("positional3", "positional argument example"),
);
/* clang-format on */

int
test_argparse_init_test(void) {
    int i;
    /* and init by hand */
    const cargparse_option_t hand_init_opts[7] = {
        {CARGPARSE_OPTION_TYPE_INT, 'n', "number", "number of something"},
        {CARGPARSE_OPTION_TYPE_BOOL, CARGPARSE_NO_SHORT, "bool", "bool for something"},
        {CARGPARSE_OPTION_TYPE_STR, CARGPARSE_NO_SHORT, "some-str", "some string"},
        {CARGPARSE_OPTION_TYPE_FLOAT, 'f', "float", "some float"},
        {CARGPARSE_OPTION_TYPE_POS, CARGPARSE_NO_SHORT, "positional1", "positional argument example"},
        {CARGPARSE_OPTION_TYPE_POS, CARGPARSE_NO_SHORT, "positional2", "positional argument example"},
        {CARGPARSE_OPTION_TYPE_POS, CARGPARSE_NO_SHORT, "positional3", "positional argument example"},
    };
    cargparse_parse_res_t hand_init_parse_res[5] = {0};
    const cargparse_t hand_init_test_argparse = {"test [OPTION]... [FILE]..\ntest [FILE]...",
                                                 "Description example.",
                                                 "Epilog example.",
                                                 hand_init_opts,
                                                 hand_init_parse_res,
                                                 7};

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

int
init_null_test(void) {
    CARGPARSE_INIT(test_ap, NULL, NULL, NULL, CARGPARSE_OPTION_INT(-1, NULL, NULL));

    TEST_EQ(test_ap.usages, (const char *)NULL);
    TEST_EQ(test_ap.description, (const char *)NULL);
    TEST_EQ(test_ap.epilog, (const char *)NULL);
    TEST_EQ(test_ap.n_options, 1);

    cargparse_option_t opt = {CARGPARSE_OPTION_TYPE_INT, -1, NULL, NULL};
    TEST(cmp_options(&test_ap.options[0], &opt));

    return 0;
}

int
option_init_test(void) {
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
    const cargparse_option_t o5h = {CARGPARSE_OPTION_TYPE_POS, CARGPARSE_NO_SHORT, "positional",
                                    "some positional"};
    TEST(cmp_options(&o5m, &o5h));

    return 0;
}

int
test_argparse_all_opts(void) {
    int i;
    char *argv[] = {"program", "-n",   "10", "--bool", "--some-str", "Some str",
                    "--float", "8.89", "--", "pos1",   "pos2",       "pos3"};
    cargparse_parse(&test_argparse, sizeof(argv) / sizeof(char *), argv);

    const cargparse_parse_res_t parse_res[] = {
        {true, 10, 0.0, NULL},  {true, 0, 0.0, NULL},   {true, 0, 0.0, "Some str"}, {true, 0, 8.89, NULL},
        {true, 0, 0.0, "pos1"}, {true, 0, 0.0, "pos2"}, {true, 0, 0.0, "pos3"},
    };
    for (i = 0; i < test_argparse.n_options; i++) {
        TEST(cmp_parse_res(&test_argparse.parse_res[i], &parse_res[i]));
    }
    CARGPARSE_PARSE_RES_CLEANUP(&test_argparse);

    return 0;
}

int
test_argparse_no_opts(void) {
    int i;
    char *argv[] = {"program"};
    cargparse_parse(&test_argparse, sizeof(argv) / sizeof(char *), argv);

    const cargparse_parse_res_t parse_res[] = {
        {false, 0, 0.0, NULL}, {false, 0, 0.0, NULL}, {false, 0, 0.0, NULL}, {false, 0, 0.0, NULL},
        {false, 0, 0.0, NULL}, {false, 0, 0.0, NULL}, {false, 0, 0.0, NULL},
    };
    for (i = 0; i < test_argparse.n_options; i++) {
        TEST(cmp_parse_res(&test_argparse.parse_res[i], &parse_res[i]));
    }
    CARGPARSE_PARSE_RES_CLEANUP(&test_argparse);

    return 0;
}

int
test_argparse_short_name_errors(void) {
    /* only non-existent options */
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_ERR_UNKNOWN_OPTION, "-a");
    /* non-existent options */
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_ERR_UNKNOWN_OPTION, "-a", " 12341");
    /* non-existent options after positional */
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_ERR_UNKNOWN_OPTION, "12341", "-a");
    /* nothing after int option */
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_ERR_OPTION_NEEDS_ARG, "-n");
    /* not number after int option */
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_ERR_INVALID_VALUE, "-n", "ffdsaf");
    /* other option after int option */
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_ERR_OPTION_NEEDS_ARG, "-n", "--float", "89.99");
    /* duplicating options */
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_ERR_OPTION_ALREADY_SET, "-n", "10", "-n", "9");

    return 0;
}

int
test_argparse_long_name_errors(void) {
    /* only non-existent options */
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_ERR_UNKNOWN_OPTION, "--non-existent");
    /* non-existent options */
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_ERR_UNKNOWN_OPTION, "--non-existent", "value1");
    /* non-existent options after positional */
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_ERR_UNKNOWN_OPTION, "positional1", "--non-existent");
    /* nothing after int option */
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_ERR_OPTION_NEEDS_ARG, "--float");
    /* not number after int option */
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_ERR_INVALID_VALUE, "--float", "ffdsaf");
    /* other option after int option */
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_ERR_OPTION_NEEDS_ARG, "--float", "--float", "89.99");
    /* duplicating options */
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_ERR_OPTION_ALREADY_SET, "--float", "10.1", "--float", "9.9");

    return 0;
}

int
test_argparse_positional(void) {
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_OK, "pos1");
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_OK, "pos1", "pos2");
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_OK, "pos1", "pos2", "pos3");
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_ERR_UNEXPECTED_POSITIONAL, "pos1", "pos2", "pos3", "pos4");
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_OK, "--", "pos1");
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_OK, "pos1", "--", "pos2");
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_OK, "pos1", "--", "pos2", "pos3");
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_OK, "--", "pos1", "pos2", "pos3");
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_OK);

    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_OK, "pos1", "--float", "89.09", "pos2", "--", "pos3");
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_OK, "-n", "10", "pos1", "--float", "89.09", "pos2", "pos3");

    return 0;
}

int
main(void) {
    printf("\nRunning tests...\n");

    RUN_TEST(test_argparse_init_test);
    RUN_TEST(init_null_test);
    RUN_TEST(option_init_test);
    RUN_TEST(test_argparse_all_opts);
    RUN_TEST(test_argparse_no_opts);
    RUN_TEST(test_argparse_short_name_errors);
    RUN_TEST(test_argparse_long_name_errors);
    RUN_TEST(test_argparse_positional);

    print_test_summary();

    return 0;
}
