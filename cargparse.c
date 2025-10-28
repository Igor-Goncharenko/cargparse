#include "cargparse.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef enum {
    CARGPARSE_ARG_INCORRECT = -1,
    CARGPARSE_ARG_POS = 0,
    CARGPARSE_ARG_SHORT,
    CARGPARSE_ARG_LONG,
    CARGPARSE_ARG_DOUBLE_HYPHEN,
} cargparse_arg_type_t;

static void _cargparse_print_option(const cargparse_option_t *opt) {
    const char *OPT_TYPE_STR[] = { "POS", "BOOL", "INT", "FLOAT", "STR" };

    printf("  %5s  ", OPT_TYPE_STR[opt->type]);

    if (opt->type == CARGPARSE_OPTION_TYPE_POS || opt->short_name == -1) {
        printf("    ");
    } else {
        printf("-%c  ", opt->short_name);
    }

    if (opt->type == CARGPARSE_OPTION_TYPE_POS || !opt->long_name) {
        printf("%17s  ", "");
    } else {
        printf("--%-15s  ", opt->long_name);
    }

    printf("%s\n", opt->help ? opt->help : "");
}

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
            _cargparse_print_option(&self->options[i]);
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
        if (self->options[i].long_name && strcmp(self->options[i].long_name, long_name) == 0)
            return i;
    }
    return -1;
}

static int _cargparse_search_short_option(const cargparse_t *const self, const char short_name) {
    int i;
    for (i = 0; i < self->n_options; i++) {
        if (self->options[i].short_name != -1 && self->options[i].short_name == short_name)
            return i;
    }
    return -1;
}

static int _cargparse_get_next_positional_opt(const cargparse_t *const self, const int prev_pos_i) {
    int i;
    for (i = prev_pos_i + 1; i < self->n_options; i++) {
        if (self->options[i].type == CARGPARSE_OPTION_TYPE_POS) {
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
    if (arg[0] == '-' && (arg[1] == '\0' || isdigit(arg[1]))) {
        return CARGPARSE_ARG_POS;
    }
    if (arg[0] == '-') {
        return CARGPARSE_ARG_SHORT;
    }
    return CARGPARSE_ARG_POS;
}

static int _cargparse_parse_int(const char *str, int *result) {
    char *endptr;
    long val;

    val = strtol(str, &endptr, 10);

    if (endptr == str) {
        fprintf(stderr, "Error: not a valid number '%s'\n", str);
        return -1;
    }
    if (*endptr != '\0') {
        fprintf(stderr, "Error: extra characters after number '%s'\n", str);
        return -1;
    }

    *result = (int)val;
    return 0;
}

static int _cargparse_parse_float(const char *str, float *result) {
    char *endptr;
    double val;

    val = strtod(str, &endptr);

    if (endptr == str) {
        fprintf(stderr, "Error: not a valid number '%s'\n", str);
        return -1;
    }
    if (*endptr != '\0') {
        fprintf(stderr, "Error: extra characters after number '%s'\n", str);
        return -1;
    }

    *result = (float)val;
    return 0;
}

static int _cargparse_set_parse_res(const cargparse_option_type_e type, const char *arg_str,
                                    cargparse_parse_res_t *parse_res) {
    switch (type) {
        case CARGPARSE_OPTION_TYPE_STR:
        case CARGPARSE_OPTION_TYPE_POS:
            parse_res->valuestr = arg_str;
            break;
        case CARGPARSE_OPTION_TYPE_BOOL:
            break;
        case CARGPARSE_OPTION_TYPE_INT:
            if (_cargparse_parse_int(arg_str, &parse_res->valueint) == -1) {
                return -1;
            }
            break;
        case CARGPARSE_OPTION_TYPE_FLOAT:
            if (_cargparse_parse_float(arg_str, &parse_res->valuefloat) == -1) {
                return -1;
            }
            break;
    }
    parse_res->is_got = true;
    return 0;
}

int cargparse_parse(cargparse_t *const self, const int argc, char **argv) {
    int i, opt_idx, last_pos_i;
    bool after_double_hyphen;
    const char *arg;

    opt_idx = -1;
    last_pos_i = -1;
    after_double_hyphen = false;

    for (i = 1; i < argc; i++) {
        arg = argv[i];

        if (after_double_hyphen) {
            last_pos_i = _cargparse_get_next_positional_opt(self, last_pos_i);
            if (last_pos_i == -1 || _cargparse_set_parse_res(self->options[last_pos_i].type,
                arg, &self->parse_res[last_pos_i]) == -1) {
                fprintf(stderr, "Error: Unexpected positional argument '%s'\n", arg);
                return -1;
            }
            continue;
        }

        switch(_cargparse_get_arg_type(arg)) {
            case CARGPARSE_ARG_POS:
                if (opt_idx == -1) { /* parse as positional argument */
                    last_pos_i = _cargparse_get_next_positional_opt(self, last_pos_i);
                    if (last_pos_i == -1 || _cargparse_set_parse_res(self->options[last_pos_i].type,
                        arg, &self->parse_res[last_pos_i]) == -1) {
                        fprintf(stderr, "Error: Unexpected positional argument '%s'\n", arg);
                        return -1;
                    }
                } else { /* parse with short/long argument which was received earlier */
                    _cargparse_set_parse_res(self->options[opt_idx].type, arg, &self->parse_res[opt_idx]);
                    opt_idx = -1;
                }
                break;
            case CARGPARSE_ARG_SHORT:
                opt_idx = _cargparse_search_short_option(self, *(arg + 1));
                if (opt_idx == -1) {
                    fprintf(stderr, "Error: unknown option '-%c'\n", *(arg + 1));
                    return -1;
                }
                if (self->options[opt_idx].type == CARGPARSE_OPTION_TYPE_BOOL) {
                    _cargparse_set_parse_res(self->options[opt_idx].type, arg, &self->parse_res[opt_idx]);
                    opt_idx = -1; /* we dont need to parse next argv in bool */
                }
                break;
            case CARGPARSE_ARG_LONG:
                opt_idx = _cargparse_search_long_option(self, arg + 2);
                if (opt_idx == -1) {
                    fprintf(stderr, "Error: unknown option '--%s'\n", arg);
                    return -1;
                }
                if (self->options[opt_idx].type == CARGPARSE_OPTION_TYPE_BOOL) {
                    _cargparse_set_parse_res(self->options[opt_idx].type, arg, &self->parse_res[opt_idx]);
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

static int _cargparse_find_opt(const cargparse_t *const self, const char short_name,
                               const char *long_name) {
    if (short_name != -1) {
        return _cargparse_search_short_option(self, short_name);
    }
    if (long_name != NULL) {
        return _cargparse_search_long_option(self, long_name);
    }
    return -1;
}

static int _cargparse_get_check_opt(const cargparse_t *const self, const cargparse_option_type_e type,
                                    const char short_name, const char *long_name) {
    int opt_idx;
    const char *opt_name;

    opt_idx = _cargparse_find_opt(self, short_name, long_name);
    opt_name = long_name ? long_name : (short_name != -1 ? &short_name : "unknown");

    if (opt_idx == -1) {
        fprintf(stderr, "Error: unknown option '%s'\n", opt_name);
        return -1;
    }
    if (self->options[opt_idx].type != type) {
        fprintf(stderr, "Error: incorrect type for option '%s'. Got=%d, expected=%d\n",
                opt_name, self->options[opt_idx].type, type);
        return -1;
    }

    return opt_idx;
}

static int _cargparse_get_bool_value(const cargparse_t *const self, const char short_name,
                                     const char *long_name, bool *result) {
    int opt_idx;
    if ((opt_idx = _cargparse_get_check_opt(self, CARGPARSE_OPTION_TYPE_BOOL, short_name, long_name)) == -1) {
        return -1;
    }
    if (!self->parse_res[opt_idx].is_got) {
        *result = false;
        return 1;
    }
    *result = true;
    return 0;
}

static int _cargparse_get_string_value(const cargparse_t *self, char short_name,
                                       const char *long_name, const char **result,
                                       const char *default_value) {
    int opt_idx;
    if ((opt_idx = _cargparse_get_check_opt(self, CARGPARSE_OPTION_TYPE_STR, short_name, long_name)) == -1) {
        return -1;
    }
    if (!self->parse_res[opt_idx].is_got) {
        *result = default_value;
        return default_value ? 1 : 0;
    }
    *result = self->parse_res[opt_idx].valuestr;
    return 0;
}

static int _cargparse_get_int_value(const cargparse_t *self, char short_name,
                                    const char *long_name, int *result,
                                    const int default_value) {
    int opt_idx;
    if ((opt_idx = _cargparse_get_check_opt(self, CARGPARSE_OPTION_TYPE_INT, short_name, long_name)) == -1) {
        return -1;
    }
    if (!self->parse_res[opt_idx].is_got) {
        *result = default_value;
        return default_value ? 1 : 0;
    }
    *result = self->parse_res[opt_idx].valueint;
    return 0;
}

static int _cargparse_get_float_value(const cargparse_t *self, char short_name,
                                      const char *long_name, float *result,
                                      const float default_value) {
    int opt_idx;
    if ((opt_idx = _cargparse_get_check_opt(self, CARGPARSE_OPTION_TYPE_FLOAT, short_name, long_name)) == -1) {
        return -1;
    }
    if (!self->parse_res[opt_idx].is_got) {
        *result = default_value;
        return default_value ? 1 : 0;
    }
    *result = self->parse_res[opt_idx].valuefloat;
    return 0;
}

int cargparse_get_bool_long(const cargparse_t *const self, const char *long_name, bool *valuebool) {
    return _cargparse_get_bool_value(self, -1, long_name, valuebool);
}

int cargparse_get_bool_short(const cargparse_t *const self, const char short_name, bool *valuebool) {
    return _cargparse_get_bool_value(self, short_name, NULL, valuebool);
}

int cargparse_get_str_long(const cargparse_t *const self, const char *long_name,
                           const char **valuestr, const char *default_value) {
    return _cargparse_get_string_value(self, -1, long_name, valuestr, default_value);
}

int cargparse_get_str_short(const cargparse_t *const self, const char short_name,
                            const char **valuestr, const char *default_value) {
    return _cargparse_get_string_value(self, short_name, NULL, valuestr, default_value);
}

int cargparse_get_int_long(const cargparse_t *const self, const char *long_name, int *valueint,
                           const int default_value) {
    return _cargparse_get_int_value(self, -1, long_name, valueint, default_value);
}

int cargparse_get_int_short(const cargparse_t *const self, const char short_name, int *valueint,
                            const int default_value) {
    return _cargparse_get_int_value(self, short_name, NULL, valueint, default_value);
}

int cargparse_get_float_long(const cargparse_t *const self, const char *long_name, float *valuefloat,
                             const float default_value) {
    return _cargparse_get_float_value(self, -1, long_name, valuefloat, default_value);
}

int cargparse_get_float_short(const cargparse_t *const self, const char short_name, float *valuefloat,
                              const float default_value) {
    return _cargparse_get_float_value(self, short_name, NULL, valuefloat, default_value);
}

int cargparse_get_positional(const cargparse_t *const self, const char *long_name,
                             const char **valuestr, const char *default_value) {
    int opt_idx;
    if ((opt_idx = _cargparse_get_check_opt(self, CARGPARSE_OPTION_TYPE_POS, -1, long_name)) == -1) {
        return -1;
    }
    if (!self->parse_res[opt_idx].is_got) {
        *valuestr = default_value;
        return default_value ? 1 : 0;
    }
    *valuestr = self->parse_res[opt_idx].valuestr;
    return 0;
}
