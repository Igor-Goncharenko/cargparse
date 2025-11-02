#include <stdio.h>

#include "cargparse.h"

/* ./argparse_example test -n 10 11 12 --float 8.001 -10.1 --some-str Some\ str -- pos1 pos2 */
int
main(int argc, char **argv) {
    unsigned idx;
    long i;
    double f;
    bool b;
    const char *s;
    cargparse_err_e ret;

    /* clang-format off */
    CARGPARSE_INIT(argparse,
            "test [OPTION]... [FILE]..\ntest [FILE]...",
            "Description example.",
            "Epilog example.",
            CARGPARSE_OPTION_INT('n', "number", "number of something", CARGPARSE_FLAG_REQUIRED, CARGPARSE_NARGS_ONE_OR_MORE),
            CARGPARSE_OPTION_STRING(CARGPARSE_NO_SHORT, "some-str", "some string", CARGPARSE_FLAG_NONE, CARGPARSE_NARGS_ZERO_OR_MORE),
            CARGPARSE_OPTION_FLOAT('f', "float", "some float", CARGPARSE_FLAG_NONE, 2),

            CARGPARSE_OPTION_BOOL('b', "bool1", "bool for something", CARGPARSE_FLAG_NONE),
            CARGPARSE_OPTION_BOOL('v', "bool2", "bool for something", CARGPARSE_FLAG_NONE),
            CARGPARSE_OPTION_BOOL('k', "bool3", "bool for something", CARGPARSE_FLAG_NONE),

            CARGPARSE_OPTION_POSITIONAL("positional1", "positional argument example", CARGPARSE_FLAG_REQUIRED),
            CARGPARSE_OPTION_POSITIONAL("pos2", "positional argument example number 2", CARGPARSE_FLAG_NONE),
            CARGPARSE_OPTION_POSITIONAL("posit3", "positional argument example number 3", CARGPARSE_FLAG_NONE),
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

    idx = 0;
    while (cargparse_get_str_long(&argparse, "some-str", &s, "default value", idx) == CARGPARSE_OK) {
        printf("%u) Got string value: \"%s\"\n", idx + 1, s);
        idx++;
    }

    idx = 0;
    while (cargparse_get_int_short(&argparse, 'n', &i, 0, idx) == CARGPARSE_OK) {
        printf("%u) Got int value: %ld\n", idx + 1, i);
        idx++;
    }

    idx = 0;
    while (cargparse_get_float_short(&argparse, 'f', &f, 0.0, idx) == CARGPARSE_OK) {
        printf("%u) Got float value: %lf\n", idx + 1, f);
        idx++;
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
