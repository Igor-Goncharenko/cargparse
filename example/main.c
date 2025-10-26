#include "cargparse.h"

int main(int argc, char **argv) {
    CARGPARSE_INIT(argparse,
            "test [OPTION]... [FILE]..\ntest [FILE]...",
            "Description example.",
            "Epilog example.",
            CARGPARSE_OPTION_INIT(CARGPARSE_OPTION_INT, 'n', "number", "number of something"),
            CARGPARSE_OPTION_INIT(CARGPARSE_OPTION_BOOL, 'b', "bool", "bool for something"),
            );

    cargparse_print_help(&argparse);

    return 0;
}
