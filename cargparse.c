#include "cargparse.h"

#include <stdio.h>
#include <string.h>

typedef enum {
    CARGPARSE_ARG_INCORRECT = -1,
    CARGPARSE_ARG_POS = 0,
    CARGPARSE_ARG_SHORT,
    CARGPARSE_ARG_LONG,
    CARGPARSE_ARG_DOUBLE_HYPHEN,
} cargparse_arg_type_t;

void cargparse_print_help(const cargparse_t *const self) {
    int i;
    const char *ch_ptr_start, *ch_ptr_end;

    if (self->usages) {
        ch_ptr_start = self->usages;
        ch_ptr_end = strchr(ch_ptr_start, '\n');
        if (ch_ptr_end) {
            printf("Usages: %.*s\n", (int)(ch_ptr_end - ch_ptr_start), ch_ptr_start);
            ch_ptr_start = ch_ptr_end + 1;
            while ((ch_ptr_end = strchr(ch_ptr_start, '\n'))) {
                printf("        %.*s\n", (int)(ch_ptr_end - ch_ptr_start), ch_ptr_start);
                ch_ptr_start = ch_ptr_end + 1;
            }
            if (*ch_ptr_start != '\0') {
                printf("        %s\n", ch_ptr_start);
            }
        } else {
            printf("Usages: %s\n", ch_ptr_start);
        }
        printf("\n");
    }
    if (self->description) {
        printf("%s\n", self->description);
        printf("\n");
    }
    if (self->n_options > 0) {
        for (i = 0; i < self->n_options; i++) {
            printf("  -%c --%s %s\n", self->options[i].short_name, self->options[i].long_name,
                   self->options[i].help);
        }
        printf("\n");
    }
    if (self->epilog) {
        printf("%s\n", self->epilog);
    }
}

static int _cargparse_search_long_option(const cargparse_t *const self, const char *long_name) {
    int i;
    for (i = 0; i < self->n_options; i++) {
        if (self->options[i].type != CARGPARSE_OPTION_POSITIONAL && self->options[i].long_name &&
            strcmp(self->options[i].long_name, long_name) == 0)
            return i;
    }
    return -1;
}

static int _cargparse_search_short_option(const cargparse_t *const self, const char short_name) {
    int i;
    for (i = 0; i < self->n_options; i++) {
        if (self->options[i].type != CARGPARSE_OPTION_POSITIONAL &&
            self->options[i].short_name != -1 && self->options[i].short_name == short_name)
            return i;
    }
    return -1;
}

static int _cargparse_get_next_positional_opt(const cargparse_t *const self, const int prev_pos_i) {
    int i;
    for (i = prev_pos_i + 1; i < self->n_options; i++) {
        if (self->options[i].type == CARGPARSE_OPTION_POSITIONAL) {
            return i;
        }
    }
    return -1;
}

static cargparse_arg_type_t _cargparse_get_arg_type(const char *arg) {
    if (!arg || arg[0] == '\0') {
        return CARGPARSE_ARG_INCORRECT;
    }

    if (arg[0] == '-' && arg[1] == '-' && arg[2] == '\0') {
        return CARGPARSE_ARG_DOUBLE_HYPHEN;
    }
    if (arg[0] == '-' && arg[1] == '-' && arg[2] != '\0' && arg[2] != '-') {
        return CARGPARSE_ARG_LONG;
    }
    if (arg[0] == '-' && arg[1] != '\0' && arg[1] != '-') {
        return CARGPARSE_ARG_SHORT;
    }
    if (arg[0] != '-') {
        return CARGPARSE_ARG_POS;
    }

    return CARGPARSE_ARG_INCORRECT;
}

int cargparse_parse(cargparse_t *const self, const int argc, char **argv) {
    int i, opt_idx, last_pos_i;
    bool after_double_hyphen;
    cargparse_arg_type_t arg_type;
    const char *arg;

    opt_idx = -1;
    last_pos_i = -1;
    after_double_hyphen = false;

    for (i = 1; i < argc; i++) {
        arg = argv[i];
        arg_type = _cargparse_get_arg_type(arg);

        if (after_double_hyphen) {
            last_pos_i = _cargparse_get_next_positional_opt(self, last_pos_i);
            if (last_pos_i != -1) {
                self->parse_res[last_pos_i].valuestr = arg;
            } else {
                fprintf(stderr, "Error: Unexpected positional argument '%s'\n", arg);
                return -1;
            }
            continue;
        }

        switch(arg_type) {
            case CARGPARSE_ARG_POS:
                if (opt_idx == -1) { /* parse as positional argument */
                    last_pos_i = _cargparse_get_next_positional_opt(self, last_pos_i);
                    if (last_pos_i != -1) {
                        self->parse_res[last_pos_i].valuestr = arg;
                    } else {
                        fprintf(stderr, "Error: Unexpected positional argument '%s'\n", arg);
                        return -1;
                    }
                } else { /* parse with short/long argument which was received earlier */
                    self->parse_res[opt_idx].valuestr = arg;
                    opt_idx = -1;
                }
                break;
            case CARGPARSE_ARG_SHORT:
                opt_idx = _cargparse_search_short_option(self, *(arg + 1));
                if (opt_idx == -1) {
                    fprintf(stderr, "Error: unknown option '-%c'\n", *(arg + 1));
                    return -1;
                }
                if (self->options[opt_idx].type == CARGPARSE_OPTION_BOOL) {
                    self->parse_res[opt_idx].valuebool = true;
                    opt_idx = -1; /* we dont need to parse next argv in bool */
                }
                break;
            case CARGPARSE_ARG_LONG:
                opt_idx = _cargparse_search_long_option(self, arg + 2);
                if (opt_idx == -1) {
                    fprintf(stderr, "Error: unknown option '--%s'\n", arg);
                    return -1;
                }
                if (self->options[opt_idx].type == CARGPARSE_OPTION_BOOL) {
                    self->parse_res[opt_idx].valuebool = true;
                    opt_idx = -1; /* we dont need to parse next argv in bool */
                }
                break;
            case CARGPARSE_ARG_DOUBLE_HYPHEN:
                after_double_hyphen = true;
                opt_idx = -1;
                break;
            case CARGPARSE_ARG_INCORRECT:
                break;
        }
    }

    return 0;
}
