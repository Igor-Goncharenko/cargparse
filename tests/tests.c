#include <stdio.h>

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

static int cmp_options(const cargparse_option_t *opt1, const cargparse_option_t *opt2) {
    if (opt1->type != opt2->type)
        return 0;
    if (opt1->short_name != opt2->short_name)
        return 0;
    if (opt1->long_name && opt2->long_name && strcmp(opt1->long_name, opt2->long_name) != 0)
        return 0;
    if ((opt1->long_name && !opt2->long_name) || (!opt1->long_name && opt2->long_name))
        return 0;
    if (opt1->help && opt2->help && strcmp(opt1->help, opt2->help) != 0)
        return 0;
    if ((opt1->help && !opt2->help) || (!opt1->help && opt2->help))
        return 0;
    return 1;
}

static int cmp_parse_res(const cargparse_parse_res_t *pr1, const cargparse_parse_res_t *pr2) {
    if (pr1->is_got != pr2->is_got)
        return 0;
    if (pr1->valueint != pr2->valueint)
        return 0;
    if (pr1->valuefloat != pr2->valuefloat)
        return 0;
    if (pr1->valuestr && pr2->valuestr && strcmp(pr1->valuestr, pr2->valuestr) != 0)
        return 0;
    if ((pr1->valuestr && !pr2->valuestr) || (!pr1->valuestr && pr2->valuestr))
        return 0;
    return 1;
}

int test_argparse_init_test(void) {
    int i;
    /* and init by hand */
    const cargparse_option_t hand_init_opts[5] = {
        { CARGPARSE_OPTION_TYPE_INT, 'n', "number", "number of something" },
        { CARGPARSE_OPTION_TYPE_BOOL, -1, "bool", "bool for something" },
        { CARGPARSE_OPTION_TYPE_STR, -1, "some-str", "some string" },
        { CARGPARSE_OPTION_TYPE_FLOAT, 'f', "float", "some float" },
        {CARGPARSE_OPTION_TYPE_POS, -1, "positional1", "positional argument example" },
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
    TEST_EQ_STR(test_argparse.usages, hand_init_test_argparse.usages, "init_test usages");
    TEST_EQ_STR(test_argparse.description, hand_init_test_argparse.description, "init_test description");
    TEST_EQ_STR(test_argparse.epilog, hand_init_test_argparse.epilog, "init_test epilog");
    TEST_EQ(test_argparse.n_options, hand_init_test_argparse.n_options, "init_test n_options");
    for (i = 0; i < test_argparse.n_options; i++) {
        TEST(cmp_options(&test_argparse.options[i], &hand_init_test_argparse.options[i]),
             "init_test cmp_options failed");
    }

    return 0;
}

int init_null_test(void) {
    CARGPARSE_INIT(test_ap, NULL, NULL, NULL,
            CARGPARSE_OPTION_INT(-1, NULL, NULL)
    );

    TEST_EQ(test_ap.usages, (const char*)NULL, "init_test usages");
    TEST_EQ(test_ap.description, (const char*)NULL, "init_test description");
    TEST_EQ(test_ap.epilog, (const char*)NULL, "init_test epilog");
    TEST_EQ(test_ap.n_options, 1, "init_test n_options");

    cargparse_option_t opt = { CARGPARSE_OPTION_TYPE_INT, -1, NULL, NULL };
    TEST(cmp_options(&test_ap.options[0], &opt), "init_null_test cmp_options failed");

    return 0;
}

int option_init_test(void) {
    const cargparse_option_t o1m = CARGPARSE_OPTION_BOOL('b', "bool", "some bool");
    const cargparse_option_t o1h = {CARGPARSE_OPTION_TYPE_BOOL, 'b', "bool", "some bool"};
    TEST(cmp_options(&o1m, &o1h), "o1m != o1h");

    const cargparse_option_t o2m = CARGPARSE_OPTION_INT('i', "int", "some int");
    const cargparse_option_t o2h = {CARGPARSE_OPTION_TYPE_INT, 'i', "int", "some int"};
    TEST(cmp_options(&o2m, &o2h), "o2m != o2h");

    const cargparse_option_t o3m = CARGPARSE_OPTION_FLOAT('f', "float", "some float");
    const cargparse_option_t o3h = {CARGPARSE_OPTION_TYPE_FLOAT, 'f', "float", "some float"};
    TEST(cmp_options(&o3m, &o3h), "o3m != o3h");

    const cargparse_option_t o4m = CARGPARSE_OPTION_STRING('s', "string", "some string");
    const cargparse_option_t o4h = {CARGPARSE_OPTION_TYPE_STR, 's', "string", "some string"};
    TEST(cmp_options(&o4m, &o4h), "o4m != o4h");

    const cargparse_option_t o5m = CARGPARSE_OPTION_POSITIONAL("positional", "some positional");
    const cargparse_option_t o5h = {CARGPARSE_OPTION_TYPE_POS, -1, "positional", "some positional"};
    TEST(cmp_options(&o5m, &o5h), "o5m != o5h");

    return 0;
}

int main(void) {
    printf("\nRunning tests...\n");

    RUN_TEST(test_argparse_init_test);
    RUN_TEST(init_null_test);
    RUN_TEST(option_init_test);

    print_test_summary();

    return 0;
}
