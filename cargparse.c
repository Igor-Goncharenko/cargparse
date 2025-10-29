#include "cargparse.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    CARGPARSE_ARG_INCORRECT = -1,
    CARGPARSE_ARG_POS = 0,
    CARGPARSE_ARG_SHORT,
    CARGPARSE_ARG_LONG,
    CARGPARSE_ARG_DOUBLE_HYPHEN,
} cargparse_arg_type_e;

static void
_cargparse_print_option(const cargparse_option_t *opt) {
    const char *OPT_TYPE_STR[] = {"POS", "BOOL", "INT", "FLOAT", "STR"};

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

void
cargparse_print_help(const cargparse_t *const self) {
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

static int
_cargparse_search_long_option(const cargparse_t *const self, const char *long_name) {
    int i;
    for (i = 0; i < self->n_options; i++) {
        if (self->options[i].long_name != CARGPARSE_NO_LONG &&
            strcmp(self->options[i].long_name, long_name) == 0) {
            return i;
        }
    }
    return -1;
}

static int
_cargparse_search_short_option(const cargparse_t *const self, const char short_name) {
    int i;
    for (i = 0; i < self->n_options; i++) {
        if (self->options[i].short_name != CARGPARSE_NO_SHORT && self->options[i].short_name == short_name) {
            return i;
        }
    }
    return -1;
}

static int
_cargparse_get_next_positional_opt(const cargparse_t *const self, const int prev_pos_i) {
    int i;
    for (i = prev_pos_i + 1; i < self->n_options; i++) {
        if (self->options[i].type == CARGPARSE_OPTION_TYPE_POS) {
            return i;
        }
    }
    return -1;
}

static cargparse_arg_type_e
_cargparse_get_arg_type(const char *arg) {
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

static cargparse_err_e
_cargparse_parse_int(const char *str, int *result) {
    char *endptr;
    long val;

    if (!str) {
        return CARGPARSE_ERR_NULL_ARGUMENT;
    }

    val = strtol(str, &endptr, 10);

    if (endptr == str) {
        fprintf(stderr, "Error: not a valid number '%s'\n", str);
        return CARGPARSE_ERR_INVALID_VALUE;
    }
    if (*endptr != '\0') {
        fprintf(stderr, "Error: extra characters after number '%s'\n", str);
        return CARGPARSE_ERR_INVALID_VALUE;
    }

    *result = (int)val;
    return CARGPARSE_OK;
}

static cargparse_err_e
_cargparse_parse_float(const char *str, float *result) {
    char *endptr;
    double val;

    if (!str) {
        return CARGPARSE_ERR_NULL_ARGUMENT;
    }

    val = strtod(str, &endptr);

    if (endptr == str) {
        fprintf(stderr, "Error: not a valid number '%s'\n", str);
        return CARGPARSE_ERR_INVALID_VALUE;
    }
    if (*endptr != '\0') {
        fprintf(stderr, "Error: extra characters after number '%s'\n", str);
        return CARGPARSE_ERR_INVALID_VALUE;
    }

    *result = (float)val;
    return CARGPARSE_OK;
}

static cargparse_err_e
_cargparse_set_parse_res(const cargparse_option_type_e type, const char *arg_str,
                         cargparse_parse_res_t *parse_res) {
    cargparse_err_e ret = CARGPARSE_OK;
    switch (type) {
        case CARGPARSE_OPTION_TYPE_STR:
        case CARGPARSE_OPTION_TYPE_POS:
            parse_res->valuestr = arg_str;
            break;
        case CARGPARSE_OPTION_TYPE_BOOL:
            break;
        case CARGPARSE_OPTION_TYPE_INT:
            ret = _cargparse_parse_int(arg_str, &parse_res->valueint);
            break;
        case CARGPARSE_OPTION_TYPE_FLOAT:
            ret = _cargparse_parse_float(arg_str, &parse_res->valuefloat);
            break;
    }
    parse_res->is_got = (ret == 0) ? true : false;
    return ret;
}

static cargparse_err_e
_cargparse_handle_bool_option(cargparse_t *const self, int opt_idx, int *opt_idx_ptr) {
    cargparse_err_e ret = CARGPARSE_OK;
    if (self->options[opt_idx].type == CARGPARSE_OPTION_TYPE_BOOL) {
        *opt_idx_ptr = -1;
        ret = _cargparse_set_parse_res(self->options[opt_idx].type, NULL, &self->parse_res[opt_idx]);
    }
    return ret;
}

static cargparse_err_e
_cargparse_handle_positional_arg(cargparse_t *const self, const char *arg, int *last_pos_i) {
    *last_pos_i = _cargparse_get_next_positional_opt(self, *last_pos_i);
    if (*last_pos_i == -1) {
        fprintf(stderr, "Error: Unexpected positional argument '%s'\n", arg);
        return CARGPARSE_ERR_UNEXPECTED_POSITIONAL;
    }
    return _cargparse_set_parse_res(self->options[*last_pos_i].type, arg, &self->parse_res[*last_pos_i]);
}

static cargparse_err_e
_cargparse_handle_option_arg(cargparse_t *const self, int opt_idx, const char *arg) {
    if (self->parse_res[opt_idx].is_got) {
        fprintf(stderr, "Error: option already got\n");
        return CARGPARSE_ERR_OPTION_ALREADY_SET;
    }
    return _cargparse_set_parse_res(self->options[opt_idx].type, arg, &self->parse_res[opt_idx]);
}

static cargparse_err_e
_cargparse_handle_short_option(cargparse_t *const self, const char *arg, int *opt_idx) {
    *opt_idx = _cargparse_search_short_option(self, arg[1]);
    if (*opt_idx == -1) {
        fprintf(stderr, "Error: unknown option '-%c'\n", arg[1]);
        return CARGPARSE_ERR_UNKNOWN_OPTION;
    }
    return _cargparse_handle_bool_option(self, *opt_idx, opt_idx);
}

static cargparse_err_e
_cargparse_handle_long_option(cargparse_t *const self, const char *arg, int *opt_idx) {
    *opt_idx = _cargparse_search_long_option(self, arg + 2);
    if (*opt_idx == -1) {
        fprintf(stderr, "Error: unknown option '--%s'\n", arg + 2);
        return CARGPARSE_ERR_UNKNOWN_OPTION;
    }
    return _cargparse_handle_bool_option(self, *opt_idx, opt_idx);
}

cargparse_err_e
cargparse_parse(cargparse_t *const self, const int argc, char **argv) {
    int i, opt_idx, last_pos_i;
    bool after_double_hyphen;
    const char *arg;
    cargparse_err_e ret;

    if (argc == 1) {
        return CARGPARSE_OK;
    }
    if (self == NULL || argv == NULL || argc < 1) {
        fprintf(stderr, "Error: incorrect arguments passed to cargparse_parse\n");
        return CARGPARSE_ERR_NULL_ARGUMENT;
    }

    opt_idx = -1;
    last_pos_i = -1;
    after_double_hyphen = false;

    for (i = 1; i < argc; i++) {
        arg = argv[i];

        if (after_double_hyphen) {
            if ((ret = _cargparse_handle_positional_arg(self, arg, &last_pos_i)) != CARGPARSE_OK) {
                return ret;
            }
            continue;
        }

        switch (_cargparse_get_arg_type(arg)) {
            case CARGPARSE_ARG_POS:
                if (opt_idx == -1) {
                    if ((ret = _cargparse_handle_positional_arg(self, arg, &last_pos_i)) != CARGPARSE_OK) {
                        return ret;
                    }
                } else {
                    if ((ret = _cargparse_handle_option_arg(self, opt_idx, arg)) != CARGPARSE_OK) {
                        return ret;
                    }
                    opt_idx = -1;
                }
                break;
            case CARGPARSE_ARG_SHORT:
                if (opt_idx != -1) {
                    fprintf(stderr, "Error: previous option not set\n");
                    return CARGPARSE_ERR_OPTION_NEEDS_ARG;
                }
                if ((ret = _cargparse_handle_short_option(self, arg, &opt_idx)) != CARGPARSE_OK) {
                    return ret;
                }
                break;
            case CARGPARSE_ARG_LONG:
                if (opt_idx != -1) {
                    fprintf(stderr, "Error: previous option not set\n");
                    return CARGPARSE_ERR_OPTION_NEEDS_ARG;
                }
                if ((ret = _cargparse_handle_long_option(self, arg, &opt_idx)) != CARGPARSE_OK) {
                    return ret;
                }
                break;
            case CARGPARSE_ARG_DOUBLE_HYPHEN:
                if (opt_idx != -1) {
                    fprintf(stderr, "Error: got '--' when previous option not set\n");
                    return CARGPARSE_ERR_OPTION_NEEDS_ARG;
                }
                after_double_hyphen = true;
                opt_idx = -1;
                break;
            case CARGPARSE_ARG_INCORRECT:
                break;
        }
    }

    if (opt_idx != -1) {
        fprintf(stderr, "Error: for last option got but not set\n");
        return CARGPARSE_ERR_OPTION_NEEDS_ARG;
    }

    return CARGPARSE_OK;
}

static int
_cargparse_find_opt(const cargparse_t *const self, const char short_name, const char *long_name) {
    if (short_name != CARGPARSE_NO_SHORT) {
        return _cargparse_search_short_option(self, short_name);
    }
    if (long_name != CARGPARSE_NO_LONG) {
        return _cargparse_search_long_option(self, long_name);
    }
    return -1;
}

static int
_cargparse_get_check_opt(const cargparse_t *const self, const cargparse_option_type_e type,
                         const char short_name, const char *long_name) {
    int opt_idx = _cargparse_find_opt(self, short_name, long_name);
    const char *opt_name = long_name ? long_name : (short_name != -1 ? &short_name : "unknown");

    if (opt_idx == -1) {
        fprintf(stderr, "Error: unknown option '%s'\n", opt_name);
        return -1;
    }
    if (self->options[opt_idx].type != type) {
        fprintf(stderr, "Error: incorrect type for option '%s'. Got=%d, expected=%d\n", opt_name,
                self->options[opt_idx].type, type);
        return -1;
    }
    return opt_idx;
}

static cargparse_err_e
_cargparse_get_bool_value(const cargparse_t *const self, const char short_name, const char *long_name,
                          bool *result) {
    int opt_idx = _cargparse_get_check_opt(self, CARGPARSE_OPTION_TYPE_BOOL, short_name, long_name);
    if (opt_idx == -1) {
        return CARGPARSE_ERR_UNKNOWN_OPTION;
    }
    *result = self->parse_res[opt_idx].is_got;
    return CARGPARSE_OK;
}

static cargparse_err_e
_cargparse_get_string_value(const cargparse_t *self, char short_name, const char *long_name,
                            const char **result, const char *default_value) {
    int opt_idx = _cargparse_get_check_opt(self, CARGPARSE_OPTION_TYPE_STR, short_name, long_name);
    if (opt_idx == -1) {
        return CARGPARSE_ERR_UNKNOWN_OPTION;
    }
    if (!self->parse_res[opt_idx].is_got) {
        *result = default_value;
        return CARGPARSE_DEFAULT_VALUE;
    }
    *result = self->parse_res[opt_idx].valuestr;
    return CARGPARSE_OK;
}

static cargparse_err_e
_cargparse_get_int_value(const cargparse_t *self, char short_name, const char *long_name, int *result,
                         const int default_value) {
    int opt_idx = _cargparse_get_check_opt(self, CARGPARSE_OPTION_TYPE_INT, short_name, long_name);
    if (opt_idx == -1) {
        return CARGPARSE_ERR_UNKNOWN_OPTION;
    }
    if (!self->parse_res[opt_idx].is_got) {
        *result = default_value;
        return CARGPARSE_DEFAULT_VALUE;
    }
    *result = self->parse_res[opt_idx].valueint;
    return CARGPARSE_OK;
}

static cargparse_err_e
_cargparse_get_float_value(const cargparse_t *self, char short_name, const char *long_name, float *result,
                           const float default_value) {
    int opt_idx = _cargparse_get_check_opt(self, CARGPARSE_OPTION_TYPE_FLOAT, short_name, long_name);
    if (opt_idx == -1) {
        return CARGPARSE_ERR_UNKNOWN_OPTION;
    }
    if (!self->parse_res[opt_idx].is_got) {
        *result = default_value;
        return CARGPARSE_DEFAULT_VALUE;
    }
    *result = self->parse_res[opt_idx].valuefloat;
    return CARGPARSE_OK;
}

cargparse_err_e
cargparse_get_bool_long(const cargparse_t *const self, const char *long_name, bool *valuebool) {
    return _cargparse_get_bool_value(self, CARGPARSE_NO_SHORT, long_name, valuebool);
}

cargparse_err_e
cargparse_get_bool_short(const cargparse_t *const self, const char short_name, bool *valuebool) {
    return _cargparse_get_bool_value(self, short_name, CARGPARSE_NO_LONG, valuebool);
}

cargparse_err_e
cargparse_get_str_long(const cargparse_t *const self, const char *long_name, const char **valuestr,
                       const char *default_value) {
    return _cargparse_get_string_value(self, CARGPARSE_NO_SHORT, long_name, valuestr, default_value);
}

cargparse_err_e
cargparse_get_str_short(const cargparse_t *const self, const char short_name, const char **valuestr,
                        const char *default_value) {
    return _cargparse_get_string_value(self, short_name, CARGPARSE_NO_LONG, valuestr, default_value);
}

cargparse_err_e
cargparse_get_int_long(const cargparse_t *const self, const char *long_name, int *valueint,
                       const int default_value) {
    return _cargparse_get_int_value(self, CARGPARSE_NO_SHORT, long_name, valueint, default_value);
}

cargparse_err_e
cargparse_get_int_short(const cargparse_t *const self, const char short_name, int *valueint,
                        const int default_value) {
    return _cargparse_get_int_value(self, short_name, CARGPARSE_NO_LONG, valueint, default_value);
}

cargparse_err_e
cargparse_get_float_long(const cargparse_t *const self, const char *long_name, float *valuefloat,
                         const float default_value) {
    return _cargparse_get_float_value(self, CARGPARSE_NO_SHORT, long_name, valuefloat, default_value);
}

cargparse_err_e
cargparse_get_float_short(const cargparse_t *const self, const char short_name, float *valuefloat,
                          const float default_value) {
    return _cargparse_get_float_value(self, short_name, CARGPARSE_NO_LONG, valuefloat, default_value);
}

cargparse_err_e
cargparse_get_positional(const cargparse_t *const self, const char *long_name, const char **valuestr,
                         const char *default_value) {
    int opt_idx = _cargparse_get_check_opt(self, CARGPARSE_OPTION_TYPE_POS, CARGPARSE_NO_SHORT, long_name);
    if (opt_idx == -1) {
        return CARGPARSE_ERR_UNKNOWN_OPTION;
    }
    if (!self->parse_res[opt_idx].is_got) {
        *valuestr = default_value;
        return CARGPARSE_DEFAULT_VALUE;
    }
    *valuestr = self->parse_res[opt_idx].valuestr;
    return CARGPARSE_OK;
}
