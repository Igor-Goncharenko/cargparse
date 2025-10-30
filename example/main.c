#include <stdio.h>

#include "cargparse.h"

/* ./argparse_example pos1 -n 10 --bool pos2 --some-str SOme\ str --float 89.342 -- pos3 */
int
main(int argc, char **argv) {
    int i;
    float f;
    bool b;
    const char *s;
    cargparse_err_e ret;

    /* clang-format off */
    CARGPARSE_INIT(argparse,
            "test [OPTION]... [FILE]..\ntest [FILE]...",
            "Description example.",
            "Epilog example.",
            CARGPARSE_OPTION_INT('n', "number", "number of something"),
            CARGPARSE_OPTION_STRING(CARGPARSE_NO_SHORT, "some-str", "some string"),
            CARGPARSE_OPTION_FLOAT('f', "float", "some float"),

            CARGPARSE_OPTION_BOOL('b', "bool1", "bool for something"),
            CARGPARSE_OPTION_BOOL('v', "bool2", "bool for something"),
            CARGPARSE_OPTION_BOOL('k', "bool3", "bool for something"),

            CARGPARSE_OPTION_POSITIONAL("positional1", "positional argument example"),
            CARGPARSE_OPTION_POSITIONAL("pos2", "positional argument example number 2"),
            CARGPARSE_OPTION_POSITIONAL("posit3", "positional argument example number 3"),
    );
    /* clang-format on */

    cargparse_print_help(&argparse);

    if ((ret = cargparse_parse(&argparse, argc, argv)) != CARGPARSE_OK) {
        fprintf(stderr, "Failed to parse options: %d\n", ret);
        return 1;
    }

    if (cargparse_get_bool_short(&argparse, 'b', &b) == CARGPARSE_OK) {
        printf("Got bool value 'b': %s\n", b ? "true" : "false");
    }
    if (cargparse_get_bool_short(&argparse, 'v', &b) == CARGPARSE_OK) {
        printf("Got bool value 'v': %s\n", b ? "true" : "false");
    }
    if (cargparse_get_bool_short(&argparse, 'k', &b) == CARGPARSE_OK) {
        printf("Got bool value 'k': %s\n", b ? "true" : "false");
    }

    if (cargparse_get_str_long(&argparse, "some-str", &s, "default value") == CARGPARSE_OK) {
        printf("Got string value: \"%s\"\n", s);
    } else {
        printf("Did not find \"some-str\"\n");
    }

    if (cargparse_get_int_short(&argparse, 'n', &i, 0) == CARGPARSE_OK) {
        printf("Got int value: %d\n", i);
    } else {
        printf("Did not find 'n'\n");
    }

    if (cargparse_get_float_short(&argparse, 'f', &f, 0.0) == CARGPARSE_OK) {
        printf("Got float value: %f\n", f);
    } else {
        printf("Did not find 'f'\n");
    }

    if (cargparse_get_positional(&argparse, "positional1", &s, "pos1_default") == CARGPARSE_OK) {
        printf("Got posit1 value: %s\n", s);
    } else {
        printf("Did not find \"posit1\"\n");
    }

    if (cargparse_get_positional(&argparse, "pos2", &s, "pos2_default") == CARGPARSE_OK) {
        printf("Got posit2 value: %s\n", s);
    } else {
        printf("Did not find \"posit2\"\n");
    }

    if (cargparse_get_positional(&argparse, "posit3", &s, "pos3_default") == CARGPARSE_OK) {
        printf("Got posit3 value: %s\n", s);
    } else {
        printf("Did not find \"posit3\"\n");
    }

    return 0;
}
