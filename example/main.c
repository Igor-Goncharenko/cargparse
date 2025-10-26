#include "cargparse.h"

#include <stdio.h>

/* ./argparse_example pos1 -n 10 --bool pos2 -- pos3 */
int main(int argc, char **argv) {
    int i;

    CARGPARSE_INIT(argparse,
            "test [OPTION]... [FILE]..\ntest [FILE]...",
            "Description example.",
            "Epilog example.",
            CARGPARSE_OPTION_INIT(CARGPARSE_OPTION_INT, 'n', "number", "number of something"),
            CARGPARSE_OPTION_INIT(CARGPARSE_OPTION_BOOL, -1, "bool", "bool for something"),
            CARGPARSE_OPTION_INIT(CARGPARSE_OPTION_POSITIONAL, -1, "positional1", "positional argument example"),
            CARGPARSE_OPTION_INIT(CARGPARSE_OPTION_POSITIONAL, -1, "pos2", "positional argument example number 2"),
            CARGPARSE_OPTION_INIT(CARGPARSE_OPTION_POSITIONAL, -1, "posit3", "positional argument example number 3"),
            );

    cargparse_print_help(&argparse);

    cargparse_parse(&argparse, argc, argv);

    for (i = 0; i < argparse.n_options; i++) {
        printf("%d) %c %s : %s %d\n", i + 1, argparse.options[i].short_name,
               argparse.options[i].long_name, argparse.parse_res[i].valuestr,
               argparse.parse_res[i].valuebool);
    }

    return 0;
}
