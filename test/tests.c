#include <stdio.h>

#include "test_core.h"
#include "../cargparse.h"

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
        TEST_EQ(lib_init_ap.options[i].type, hand_init_ap.options[i].type,
                "init_test option type mismatch");
        TEST_EQ(lib_init_ap.options[i].short_name, hand_init_ap.options[i].short_name,
                "init_test option short_name mismatch");
        TEST_EQ_STR(lib_init_ap.options[i].long_name, hand_init_ap.options[i].long_name,
                   "init_test option long_name mismatch");
        TEST_EQ_STR(lib_init_ap.options[i].help, hand_init_ap.options[i].help,
                   "init_test option help mismatch");
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

    TEST_EQ(test_ap.options[0].type, (cargparse_option_type_e)CARGPARSE_OPTION_TYPE_INT,
            "init_test option type mismatch");
    TEST_EQ(test_ap.options[0].short_name, (char)-1, "init_test option short_name mismatch");
    TEST_EQ(test_ap.options[0].long_name, (const char*)NULL, "init_test option long_name mismatch");
    TEST_EQ(test_ap.options[0].help, (const char*)NULL, "init_test option help mismatch");

    return 0;
}

int main(void) {
    printf("\nRunning tests...\n");

    RUN_TEST(init_test);
    RUN_TEST(init_null_test);

    print_test_summary();

    return 0;
}
