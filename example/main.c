#include "cargparse.h"

#include <stdio.h>

int main(int argc, char **argv) {
    int i;

    CARGPARSE_INIT(argparse,
            "test [OPTION]... [FILE]..\ntest [FILE]...",
            "Description example.",
            "Epilog example.",
            CARGPARSE_OPTION_INIT(CARGPARSE_OPTION_INT, 'n', "number", "number of something"),
            CARGPARSE_OPTION_INIT(CARGPARSE_OPTION_BOOL, 'b', "bool", "bool for something"),
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
