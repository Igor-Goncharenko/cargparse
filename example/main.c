#include "cargparse.h"

#include <stdio.h>

/* ./argparse_example pos1 -n 10 --bool pos2 --some-str SOme\ str --float 89.342 -- pos3 */
int main(int argc, char **argv) {
    int i;
    float f;
    bool b;
    const char *s;

    CARGPARSE_INIT(argparse,
            "test [OPTION]... [FILE]..\ntest [FILE]...",
            "Description example.",
            "Epilog example.",
            CARGPARSE_OPTION_INT('n', "number", "number of something"),
            CARGPARSE_OPTION_BOOL(-1, "bool", "bool for something"),
            CARGPARSE_OPTION_STRING(-1, "some-str", "some string"),
            CARGPARSE_OPTION_FLOAT('f', "float", "some float"),
            CARGPARSE_OPTION_POSITIONAL("positional1", "positional argument example"),
            CARGPARSE_OPTION_POSITIONAL("pos2", "positional argument example number 2"),
            CARGPARSE_OPTION_POSITIONAL("posit3", "positional argument example number 3"),
    );

    cargparse_print_help(&argparse);

    if (cargparse_parse(&argparse, argc, argv) == -1) {
        fprintf(stderr, "Failed to parse options\n");
        return 1;
    }

    if (cargparse_get_bool_long(&argparse, "bool", &b) != -1) {
        printf("Got bool value: %s\n", b ? "true" : "false");
    } else {
        printf("Did not find \"bool\"\n");
    }

    if (cargparse_get_str_long(&argparse, "some-str", &s, "default value") != -1) {
        printf("Got string value: \"%s\"\n", s);
    } else {
        printf("Did not find \"some-str\"\n");
    }

    if (cargparse_get_int_short(&argparse, 'n', &i, 0) != -1) {
        printf("Got int value: %d\n", i);
    } else {
        printf("Did not find 'n'\n");
    }

    if (cargparse_get_float_short(&argparse, 'f', &f, 0.0) != -1) {
        printf("Got float value: %f\n", f);
    } else {
        printf("Did not find 'f'\n");
    }

    if (cargparse_get_positional(&argparse, "positional1", &s, "pos1_default") != -1) {
        printf("Got posit1 value: %s\n", s);
    } else {
        printf("Did not find \"posit1\"\n");
    }

    if (cargparse_get_positional(&argparse, "pos2", &s, "pos2_default") != -1) {
        printf("Got posit2 value: %s\n", s);
    } else {
        printf("Did not find \"posit2\"\n");
    }

    if (cargparse_get_positional(&argparse, "posit3", &s, "pos3_default") != -1) {
        printf("Got posit3 value: %s\n", s);
    } else {
        printf("Did not find \"posit3\"\n");
    }

    return 0;
}
