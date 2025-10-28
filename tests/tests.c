#include <stdio.h>

#include "test_core.h"
#include "../cargparse.h"

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

int init_test(void) {
    int i;

    /* init with library macro */
    CARGPARSE_INIT(lib_init_ap,
            "test [OPTION]... [FILE]..\ntest [FILE]...",
            "Description example.",
            "Epilog example.",
            CARGPARSE_OPTION_INT('n', "number", "number of something"),
            CARGPARSE_OPTION_BOOL(-1, "bool", "bool for something"),
            CARGPARSE_OPTION_STRING(-1, "some-str", "some string"),
            CARGPARSE_OPTION_FLOAT('f', "float", "some float"),
            CARGPARSE_OPTION_POSITIONAL("positional1", "positional argument example"),
    );
    /* and init by hand */
    cargparse_option_t hand_init_opts[5] = {
        { CARGPARSE_OPTION_TYPE_INT, 'n', "number", "number of something" },
        { CARGPARSE_OPTION_TYPE_BOOL, -1, "bool", "bool for something" },
        { CARGPARSE_OPTION_TYPE_STR, -1, "some-str", "some string" },
        { CARGPARSE_OPTION_TYPE_FLOAT, 'f', "float", "some float" },
        {CARGPARSE_OPTION_TYPE_POS, -1, "positional1", "positional argument example" },
    };
    cargparse_parse_res_t hand_init_parse_res[5] = { 0 };
    cargparse_t hand_init_ap = {
        "test [OPTION]... [FILE]..\ntest [FILE]...",
        "Description example.",
        "Epilog example.",
        hand_init_opts,
        hand_init_parse_res,
        5
    };

    /* compare macro init and hand init */
    TEST_EQ_STR(lib_init_ap.usages, hand_init_ap.usages, "init_test usages");
    TEST_EQ_STR(lib_init_ap.description, hand_init_ap.description, "init_test description");
    TEST_EQ_STR(lib_init_ap.epilog, hand_init_ap.epilog, "init_test epilog");
    TEST_EQ(lib_init_ap.n_options, hand_init_ap.n_options, "init_test n_options");
    for (i = 0; i < lib_init_ap.n_options; i++) {
        TEST(cmp_options(&lib_init_ap.options[i], &hand_init_ap.options[i]),
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

    RUN_TEST(init_test);
    RUN_TEST(init_null_test);
    RUN_TEST(option_init_test);

    print_test_summary();

    return 0;
}
