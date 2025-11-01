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
    CARGPARSE_OPTION_INT('n', "number", "number of something", CARGPARSE_FLAG_NONE),
    CARGPARSE_OPTION_BOOL(CARGPARSE_NO_SHORT, "bool", "bool for something", CARGPARSE_FLAG_NONE),
    CARGPARSE_OPTION_STRING(CARGPARSE_NO_SHORT, "some-str", "some string", CARGPARSE_FLAG_NONE),
    CARGPARSE_OPTION_FLOAT('f', "float", "some float", CARGPARSE_FLAG_NONE),
    CARGPARSE_OPTION_POSITIONAL("positional1", "positional argument example", CARGPARSE_FLAG_NONE),
    CARGPARSE_OPTION_POSITIONAL("positional2", "positional argument example", CARGPARSE_FLAG_NONE),
    CARGPARSE_OPTION_POSITIONAL("positional3", "positional argument example", CARGPARSE_FLAG_NONE),
);
/* clang-format on */

int
test_argparse_init_test(void) {
    int i;
    /* and init by hand */
    const cargparse_option_t hand_init_opts[7] = {
        {CARGPARSE_OPTION_TYPE_INT, 'n', "number", "number of something", CARGPARSE_FLAG_NONE},
        {CARGPARSE_OPTION_TYPE_BOOL, CARGPARSE_NO_SHORT, "bool", "bool for something", CARGPARSE_FLAG_NONE},
        {CARGPARSE_OPTION_TYPE_STR, CARGPARSE_NO_SHORT, "some-str", "some string", CARGPARSE_FLAG_NONE},
        {CARGPARSE_OPTION_TYPE_FLOAT, 'f', "float", "some float", CARGPARSE_FLAG_NONE},
        {CARGPARSE_OPTION_TYPE_POS, CARGPARSE_NO_SHORT, "positional1", "positional argument example",
         CARGPARSE_FLAG_NONE},
        {CARGPARSE_OPTION_TYPE_POS, CARGPARSE_NO_SHORT, "positional2", "positional argument example",
         CARGPARSE_FLAG_NONE},
        {CARGPARSE_OPTION_TYPE_POS, CARGPARSE_NO_SHORT, "positional3", "positional argument example",
         CARGPARSE_FLAG_NONE},
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
    CARGPARSE_INIT(test_ap, NULL, NULL, NULL, CARGPARSE_OPTION_INT(-1, NULL, NULL, CARGPARSE_FLAG_NONE));

    TEST_EQ(test_ap.usages, (const char *)NULL);
    TEST_EQ(test_ap.description, (const char *)NULL);
    TEST_EQ(test_ap.epilog, (const char *)NULL);
    TEST_EQ(test_ap.n_options, 1);

    cargparse_option_t opt = {CARGPARSE_OPTION_TYPE_INT, -1, NULL, NULL, CARGPARSE_FLAG_NONE};
    TEST(cmp_options(&test_ap.options[0], &opt));

    return 0;
}

int
option_init_test(void) {
    const cargparse_option_t o1m = CARGPARSE_OPTION_BOOL('b', "bool", "some bool", CARGPARSE_FLAG_NONE);
    const cargparse_option_t o1h = {CARGPARSE_OPTION_TYPE_BOOL, 'b', "bool", "some bool",
                                    CARGPARSE_FLAG_NONE};
    TEST(cmp_options(&o1m, &o1h));

    const cargparse_option_t o2m = CARGPARSE_OPTION_INT('i', "int", "some int", CARGPARSE_FLAG_NONE);
    const cargparse_option_t o2h = {CARGPARSE_OPTION_TYPE_INT, 'i', "int", "some int", CARGPARSE_FLAG_NONE};
    TEST(cmp_options(&o2m, &o2h));

    const cargparse_option_t o3m = CARGPARSE_OPTION_FLOAT('f', "float", "some float", CARGPARSE_FLAG_NONE);
    const cargparse_option_t o3h = {CARGPARSE_OPTION_TYPE_FLOAT, 'f', "float", "some float",
                                    CARGPARSE_FLAG_NONE};
    TEST(cmp_options(&o3m, &o3h));

    const cargparse_option_t o4m = CARGPARSE_OPTION_STRING('s', "string", "some string", CARGPARSE_FLAG_NONE);
    const cargparse_option_t o4h = {CARGPARSE_OPTION_TYPE_STR, 's', "string", "some string",
                                    CARGPARSE_FLAG_NONE};
    TEST(cmp_options(&o4m, &o4h));

    const cargparse_option_t o5m =
        CARGPARSE_OPTION_POSITIONAL("positional", "some positional", CARGPARSE_FLAG_NONE);
    const cargparse_option_t o5h = {CARGPARSE_OPTION_TYPE_POS, CARGPARSE_NO_SHORT, "positional",
                                    "some positional", CARGPARSE_FLAG_NONE};
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
        {true, "10"},   {true, NULL},   {true, "Some str"}, {true, "8.89"},
        {true, "pos1"}, {true, "pos2"}, {true, "pos3"},
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
        {false, NULL}, {false, NULL}, {false, NULL}, {false, NULL},
        {false, NULL}, {false, NULL}, {false, NULL},
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
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_ERR_OPTION_UNKNOWN, "-a");
    /* non-existent options */
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_ERR_OPTION_UNKNOWN, "-a", " 12341");
    /* non-existent options after positional */
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_ERR_OPTION_UNKNOWN, "12341", "-a");
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
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_ERR_OPTION_UNKNOWN, "--non-existent");
    /* non-existent options */
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_ERR_OPTION_UNKNOWN, "--non-existent", "value1");
    /* non-existent options after positional */
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_ERR_OPTION_UNKNOWN, "positional1", "--non-existent");
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
    const cargparse_parse_res_t parse_res1[] = {
        {false, NULL},  {false, NULL}, {false, NULL}, {false, NULL},
        {true, "pos1"}, {false, NULL}, {false, NULL},
    };
    const cargparse_parse_res_t parse_res2[] = {
        {false, NULL},  {false, NULL},  {false, NULL}, {false, NULL},
        {true, "pos1"}, {true, "pos2"}, {false, NULL},
    };
    const cargparse_parse_res_t parse_res3[] = {
        {false, NULL},  {false, NULL},  {false, NULL},  {false, NULL},
        {true, "pos1"}, {true, "pos2"}, {true, "pos3"},
    };
    const cargparse_parse_res_t parse_res4[] = {
        {false, NULL},  {true, NULL},   {false, NULL},  {true, "-89.09"},
        {true, "pos1"}, {true, "pos2"}, {true, "pos3"},
    };
    const cargparse_parse_res_t parse_res5[] = {
        {true, "10"},   {false, NULL},  {false, NULL},  {true, "89.09"},
        {true, "pos1"}, {true, "pos2"}, {true, "pos3"},
    };

    TEST_PARSE_RES(&test_argparse, parse_res1, "pos1");

    TEST_PARSE_RES(&test_argparse, parse_res1, "pos1");
    TEST_PARSE_RES(&test_argparse, parse_res2, "pos1", "pos2");
    TEST_PARSE_RES(&test_argparse, parse_res3, "pos1", "pos2", "pos3");
    TEST_PARSE_RES(&test_argparse, parse_res1, "--", "pos1");
    TEST_PARSE_RES(&test_argparse, parse_res2, "pos1", "--", "pos2");
    TEST_PARSE_RES(&test_argparse, parse_res3, "pos1", "--", "pos2", "pos3");
    TEST_PARSE_RES(&test_argparse, parse_res3, "--", "pos1", "pos2", "pos3");

    TEST_PARSE_RES(&test_argparse, parse_res4, "pos1", "--float", "-89.09", "pos2", "--bool", "--", "pos3");
    TEST_PARSE_RES(&test_argparse, parse_res5, "-n", "10", "pos1", "--float", "89.09", "pos2", "pos3");

    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_GOT_ZERO_ARGS);
    TEST_PARSE_ERROR(&test_argparse, CARGPARSE_ERR_UNEXPECTED_POSITIONAL, "pos1", "pos2", "pos3", "pos4");

    return 0;
}

int
test_argparse_getters(void) {
    bool b;
    long d;
    double f;
    const char *s;

    int i;
    char *argv[] = {"program", "-n",   "10", "--bool", "--some-str", "Some str",
                    "--float", "8.89", "--", "pos1",   "pos2",       "pos3"};
    cargparse_parse(&test_argparse, sizeof(argv) / sizeof(char *), argv);

    const cargparse_parse_res_t parse_res[] = {
        {true, "10"},   {true, NULL},   {true, "Some str"}, {true, "8.89"},
        {true, "pos1"}, {true, "pos2"}, {true, "pos3"},
    };
    for (i = 0; i < test_argparse.n_options; i++) {
        TEST(cmp_parse_res(&test_argparse.parse_res[i], &parse_res[i]));
    }

    TEST_EQ(cargparse_get_bool_long(&test_argparse, "bool", &b), (cargparse_err_e)CARGPARSE_OK);
    TEST_EQ(b, (bool)true);

    TEST_EQ(cargparse_get_str_long(&test_argparse, "some-str", &s, NULL), (cargparse_err_e)CARGPARSE_OK);
    TEST(strcmp(s, "Some str") == 0);

    TEST_EQ(cargparse_get_float_short(&test_argparse, 'f', &f, 0.0), (cargparse_err_e)CARGPARSE_OK);
    TEST_EQ((long)(f * 1000), (long)(8.89 * 1000));

    TEST_EQ(cargparse_get_float_long(&test_argparse, "float", &f, 0.0), (cargparse_err_e)CARGPARSE_OK);
    TEST_EQ((long)(f * 1000), (long)(8.89 * 1000));

    TEST_EQ(cargparse_get_int_short(&test_argparse, 'n', &d, 0), (cargparse_err_e)CARGPARSE_OK);
    TEST_EQ(d, (long)10);

    TEST_EQ(cargparse_get_int_long(&test_argparse, "number", &d, 0), (cargparse_err_e)CARGPARSE_OK);
    TEST_EQ(d, (long)10);

    TEST_EQ(cargparse_get_positional(&test_argparse, "positional1", &s, NULL), (cargparse_err_e)CARGPARSE_OK);
    TEST(strcmp(s, "pos1") == 0);

    TEST_EQ(cargparse_get_positional(&test_argparse, "positional2", &s, NULL), (cargparse_err_e)CARGPARSE_OK);
    TEST(strcmp(s, "pos2") == 0);

    TEST_EQ(cargparse_get_positional(&test_argparse, "positional3", &s, NULL), (cargparse_err_e)CARGPARSE_OK);
    TEST(strcmp(s, "pos3") == 0);

    CARGPARSE_PARSE_RES_CLEANUP(&test_argparse);
    return 0;
}

int
test_argparse_getters_unknown(void) {
    bool b;
    long d;
    double f;
    const char *s;

    int i;
    char *argv[] = {"program"};
    cargparse_parse(&test_argparse, sizeof(argv) / sizeof(char *), argv);

    const cargparse_parse_res_t parse_res[] = {
        {false, NULL}, {false, NULL}, {false, NULL}, {false, NULL},
        {false, NULL}, {false, NULL}, {false, NULL},
    };
    for (i = 0; i < test_argparse.n_options; i++) {
        TEST(cmp_parse_res(&test_argparse.parse_res[i], &parse_res[i]));
    }

    TEST_EQ(cargparse_get_bool_long(&test_argparse, "bool-unk", &b),
            (cargparse_err_e)CARGPARSE_ERR_OPTION_UNKNOWN);

    TEST_EQ(cargparse_get_str_long(&test_argparse, "some-str-unk", &s, "default str"),
            (cargparse_err_e)CARGPARSE_ERR_OPTION_UNKNOWN);

    TEST_EQ(cargparse_get_float_short(&test_argparse, 'p', &f, -999.9),
            (cargparse_err_e)CARGPARSE_ERR_OPTION_UNKNOWN);

    TEST_EQ(cargparse_get_float_long(&test_argparse, "float-ukn", &f, -1111.1),
            (cargparse_err_e)CARGPARSE_ERR_OPTION_UNKNOWN);

    TEST_EQ(cargparse_get_int_short(&test_argparse, 'a', &d, 1234),
            (cargparse_err_e)CARGPARSE_ERR_OPTION_UNKNOWN);

    TEST_EQ(cargparse_get_int_long(&test_argparse, "number-unk", &d, -4321),
            (cargparse_err_e)CARGPARSE_ERR_OPTION_UNKNOWN);

    TEST_EQ(cargparse_get_positional(&test_argparse, "positional1-ukn", &s, "pos1_default"),
            (cargparse_err_e)CARGPARSE_ERR_OPTION_UNKNOWN);

    TEST_EQ(cargparse_get_positional(&test_argparse, "positional2-ukn", &s, "pos2_default"),
            (cargparse_err_e)CARGPARSE_ERR_OPTION_UNKNOWN);

    TEST_EQ(cargparse_get_positional(&test_argparse, "positional3-ukn", &s, "pos3_default"),
            (cargparse_err_e)CARGPARSE_ERR_OPTION_UNKNOWN);

    CARGPARSE_PARSE_RES_CLEANUP(&test_argparse);
    return 0;
}

int
test_argparse_getters_defaults(void) {
    bool b;
    long d;
    double f;
    const char *s;

    int i;
    char *argv[] = {"program"};
    cargparse_parse(&test_argparse, sizeof(argv) / sizeof(char *), argv);

    const cargparse_parse_res_t parse_res[] = {
        {false, NULL}, {false, NULL}, {false, NULL}, {false, NULL},
        {false, NULL}, {false, NULL}, {false, NULL},
    };
    for (i = 0; i < test_argparse.n_options; i++) {
        TEST(cmp_parse_res(&test_argparse.parse_res[i], &parse_res[i]));
    }

    TEST_EQ(cargparse_get_bool_long(&test_argparse, "bool", &b), (cargparse_err_e)CARGPARSE_DEFAULT_VALUE);
    TEST_EQ(b, (bool)false);

    TEST_EQ(cargparse_get_str_long(&test_argparse, "some-str", &s, "default str"),
            (cargparse_err_e)CARGPARSE_DEFAULT_VALUE);
    TEST(strcmp(s, "default str") == 0);

    TEST_EQ(cargparse_get_float_short(&test_argparse, 'f', &f, -999.9),
            (cargparse_err_e)CARGPARSE_DEFAULT_VALUE);
    TEST_EQ((long)(f * 1000), (long)(-999.9 * 1000));

    TEST_EQ(cargparse_get_float_long(&test_argparse, "float", &f, -1111.1),
            (cargparse_err_e)CARGPARSE_DEFAULT_VALUE);
    TEST_EQ((long)(f * 1000), (long)(-1111.1 * 1000));

    TEST_EQ(cargparse_get_int_short(&test_argparse, 'n', &d, 1234), (cargparse_err_e)CARGPARSE_DEFAULT_VALUE);
    TEST_EQ(d, (long)1234);

    TEST_EQ(cargparse_get_int_long(&test_argparse, "number", &d, -4321),
            (cargparse_err_e)CARGPARSE_DEFAULT_VALUE);
    TEST_EQ(d, (long)-4321);

    TEST_EQ(cargparse_get_positional(&test_argparse, "positional1", &s, "pos1_default"),
            (cargparse_err_e)CARGPARSE_DEFAULT_VALUE);
    TEST(strcmp(s, "pos1_default") == 0);

    TEST_EQ(cargparse_get_positional(&test_argparse, "positional2", &s, "pos2_default"),
            (cargparse_err_e)CARGPARSE_DEFAULT_VALUE);
    TEST(strcmp(s, "pos2_default") == 0);

    TEST_EQ(cargparse_get_positional(&test_argparse, "positional3", &s, "pos3_default"),
            (cargparse_err_e)CARGPARSE_DEFAULT_VALUE);
    TEST(strcmp(s, "pos3_default") == 0);

    CARGPARSE_PARSE_RES_CLEANUP(&test_argparse);
    return 0;
}

int
test_required_args(void) {
    /* clang-format off */
    CARGPARSE_INIT(test_req, NULL, NULL, NULL,
        CARGPARSE_OPTION_INT('n', "number", "number of something", CARGPARSE_FLAG_REQUIRED),
        CARGPARSE_OPTION_INT('d', "d number", "d number of something", CARGPARSE_FLAG_NONE),
        CARGPARSE_OPTION_POSITIONAL("pos1", "first positional argument", CARGPARSE_FLAG_REQUIRED),
    );
    /* clang-format on */

    TEST_PARSE_ERROR(&test_req, CARGPARSE_GOT_ZERO_ARGS);
    TEST_PARSE_ERROR(&test_req, CARGPARSE_ERR_NOT_ALL_REQUIRED_OPTIONS, "-n", "10");
    TEST_PARSE_ERROR(&test_req, CARGPARSE_ERR_NOT_ALL_REQUIRED_OPTIONS, "-n", "10", "-d", "99");
    TEST_PARSE_ERROR(&test_req, CARGPARSE_ERR_NOT_ALL_REQUIRED_OPTIONS, "-d", "10", "pos1 value");
    TEST_PARSE_ERROR(&test_req, CARGPARSE_ERR_NOT_ALL_REQUIRED_OPTIONS, "pos1 value");
    TEST_PARSE_ERROR(&test_req, CARGPARSE_OK, "-n", "10", "pos1 value");
    TEST_PARSE_ERROR(&test_req, CARGPARSE_OK, "-n", "10", "pos1 value", "-d", "99");

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
    RUN_TEST(test_argparse_getters);
    RUN_TEST(test_argparse_getters_unknown);
    RUN_TEST(test_argparse_getters_defaults);
    RUN_TEST(test_required_args);

    print_test_summary();

    return 0;
}
