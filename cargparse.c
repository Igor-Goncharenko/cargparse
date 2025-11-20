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
    if (!arg || arg[0] == '\0') return CARGPARSE_ARG_INCORRECT;

    if (arg[0] == '-' && arg[1] == '-' && arg[2] == '\0') return CARGPARSE_ARG_DOUBLE_HYPHEN;
    if (arg[0] == '-' && arg[1] == '-' && arg[2] != '\0' && arg[2] != '-') return CARGPARSE_ARG_LONG;
    if (arg[0] == '-' && (arg[1] == '\0' || isdigit(arg[1]))) return CARGPARSE_ARG_POS;
    if (arg[0] == '-') return CARGPARSE_ARG_SHORT;

    return CARGPARSE_ARG_POS;
}

static cargparse_err_e
_cargparse_parse_int(const char *str, long *result) {
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

    *result = val;
    return CARGPARSE_OK;
}

static cargparse_err_e
_cargparse_parse_float(const char *str, double *result) {
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

static bool
_cargparse_is_valid_int(const char *str) {
    long dummy;
    return _cargparse_parse_int(str, &dummy) == CARGPARSE_OK;
}

static bool
_cargparse_is_valid_float(const char *str) {
    double dummy;
    return _cargparse_parse_float(str, &dummy) == CARGPARSE_OK;
}

static cargparse_err_e
_cargparse_set_parse_res(const cargparse_option_type_e type, char **arg_str, cargparse_parse_res_t *parse_res,
                         const cargparse_option_t *opt) {
    switch (type) {
        case CARGPARSE_OPTION_TYPE_BOOL:
            return CARGPARSE_ERR_INVALID_VALUE;
        case CARGPARSE_OPTION_TYPE_STR:
        case CARGPARSE_OPTION_TYPE_POS:
            break;
        case CARGPARSE_OPTION_TYPE_INT:
            if (!_cargparse_is_valid_int(*arg_str)) return CARGPARSE_ERR_INVALID_VALUE;
            break;
        case CARGPARSE_OPTION_TYPE_FLOAT:
            if (!_cargparse_is_valid_float(*arg_str)) return CARGPARSE_ERR_INVALID_VALUE;
            break;
    }
    if (parse_res->nargs == 0) {
        parse_res->valuestr = arg_str;
        parse_res->nargs = 1;
        parse_res->is_got = (opt->nargs <= 1) ? true : false;
    } else {
        parse_res->nargs++;
        parse_res->is_got = (opt->nargs <= parse_res->nargs) ? true : false;
    }
    return CARGPARSE_OK;
}

static cargparse_err_e
_cargparse_handle_positional_arg(cargparse_t *const self, char **arg, int *last_pos_i) {
    if (*last_pos_i == -1 || (self->parse_res[*last_pos_i].is_got &&
                              self->options[*last_pos_i].nargs != CARGPARSE_NARGS_ONE_OR_MORE &&
                              self->options[*last_pos_i].nargs != CARGPARSE_NARGS_ZERO_OR_MORE)) {
        *last_pos_i = _cargparse_get_next_positional_opt(self, *last_pos_i);
    }
    if (*last_pos_i != -1) {
        return _cargparse_set_parse_res(self->options[*last_pos_i].type, arg, &self->parse_res[*last_pos_i],
                                        &self->options[*last_pos_i]);
    } else {
        fprintf(stderr, "Error: Unexpected positional argument '%s'\n", *arg);
        return CARGPARSE_ERR_UNEXPECTED_POSITIONAL;
    }
}

static cargparse_err_e
_cargparse_handle_option_arg(cargparse_t *const self, int opt_idx, char **arg) {
    if (self->parse_res[opt_idx].is_got && self->options[opt_idx].nargs != CARGPARSE_NARGS_ONE_OR_MORE &&
        self->options[opt_idx].nargs != CARGPARSE_NARGS_ZERO_OR_MORE) {
        fprintf(stderr, "Error: option already got\n");
        return CARGPARSE_ERR_OPTION_ALREADY_SET;
    }
    return _cargparse_set_parse_res(self->options[opt_idx].type, arg, &self->parse_res[opt_idx],
                                    &self->options[opt_idx]);
}

static cargparse_err_e
_cargparse_handle_short_option(cargparse_t *const self, const char *arg, int *opt_idx) {
    *opt_idx = _cargparse_search_short_option(self, arg[1]);
    if (*opt_idx == -1) {
        fprintf(stderr, "Error: unknown option '-%c'\n", arg[1]);
        return CARGPARSE_ERR_OPTION_UNKNOWN;
    }
    if (self->options[*opt_idx].type == CARGPARSE_OPTION_TYPE_BOOL) {
        self->parse_res[*opt_idx].is_got = true;
        self->parse_res[*opt_idx].nargs = 1;
        *opt_idx = -1;
    } else if (self->options[*opt_idx].nargs == CARGPARSE_NARGS_ZERO_OR_MORE) {
        self->parse_res[*opt_idx].is_got = true;
    }
    return CARGPARSE_OK;
}

static cargparse_err_e
_cargparse_handle_long_option(cargparse_t *const self, const char *arg, int *opt_idx) {
    *opt_idx = _cargparse_search_long_option(self, arg + 2);
    if (*opt_idx == -1) {
        fprintf(stderr, "Error: unknown option '--%s'\n", arg + 2);
        return CARGPARSE_ERR_OPTION_UNKNOWN;
    }
    if (self->options[*opt_idx].type == CARGPARSE_OPTION_TYPE_BOOL) {
        self->parse_res[*opt_idx].is_got = true;
        self->parse_res[*opt_idx].nargs = 1;
        *opt_idx = -1;
    } else if (self->options[*opt_idx].nargs == CARGPARSE_NARGS_ZERO_OR_MORE) {
        self->parse_res[*opt_idx].is_got = true;
    }
    return CARGPARSE_OK;
}

static cargparse_err_e
_cargparse_handle_mult_short_bool_options(cargparse_t *const self, const char *arg, int *opt_idx) {
    int local_opt_idx, i;
    *opt_idx = -1;

    for (i = 1; arg[i] != '\0'; i++) {
        local_opt_idx = _cargparse_search_short_option(self, arg[i]);
        if (local_opt_idx == -1) {
            fprintf(stderr, "Error: unknown option '-%c'\n", arg[i]);
            return CARGPARSE_ERR_OPTION_UNKNOWN;
        }
        if (self->options[local_opt_idx].type != CARGPARSE_OPTION_TYPE_BOOL) {
            fprintf(stderr, "Error: not bool type for option '-%c' in multiple bool definition\n", arg[i]);
            return CARGPARSE_ERR_NOT_BOOL_IN_MULT_BOOL_DEF;
        }
        self->parse_res[local_opt_idx].is_got = true;
        self->parse_res[local_opt_idx].nargs = 1;
    }

    return CARGPARSE_OK;
}

static bool
_cargparse_check_required_options(const cargparse_t *const self) {
    int i;
    for (i = 0; i < self->n_options; i++) {
        if ((self->options[i].flags & CARGPARSE_FLAG_REQUIRED && !self->parse_res[i].is_got)) {
            return false;
        }
    }
    return true;
}

cargparse_err_e
cargparse_parse(cargparse_t *const self, const int argc, char **argv) {
    int i, opt_idx = -1, last_pos_i = -1;
    bool after_double_hyphen = false;
    char **arg;
    cargparse_err_e ret;
    cargparse_arg_type_e type;

    if (argc == 1) return CARGPARSE_GOT_ZERO_ARGS;
    if (!self) return CARGPARSE_ERR_NULL_PARSER;
    if (!argv) return CARGPARSE_ERR_NULL_ARGUMENT;

    for (i = 1; i < argc; i++) {
        arg = &argv[i];
        type = _cargparse_get_arg_type(*arg);

        if (after_double_hyphen && type != CARGPARSE_ARG_DOUBLE_HYPHEN) {
            type = CARGPARSE_ARG_POS;
        }

        switch (type) {
            case CARGPARSE_ARG_POS:
                if (opt_idx == -1) {
                    if ((ret = _cargparse_handle_positional_arg(self, arg, &last_pos_i)) != CARGPARSE_OK) {
                        return ret;
                    }
                } else {
                    if ((ret = _cargparse_handle_option_arg(self, opt_idx, arg)) != CARGPARSE_OK) {
                        return ret;
                    }
                    if (self->parse_res[opt_idx].is_got &&
                        self->options[opt_idx].nargs != CARGPARSE_NARGS_ONE_OR_MORE &&
                        self->options[opt_idx].nargs != CARGPARSE_NARGS_ZERO_OR_MORE) {
                        opt_idx = -1;
                    }
                }
                break;
            case CARGPARSE_ARG_SHORT:
                if (self->options[opt_idx].nargs == CARGPARSE_NARGS_ONE_OR_MORE ||
                    self->options[opt_idx].nargs == CARGPARSE_NARGS_ZERO_OR_MORE) {
                    opt_idx = -1;
                } else if (opt_idx != -1) {
                    fprintf(stderr, "Error: previous option not set\n");
                    return CARGPARSE_ERR_OPTION_NEEDS_ARG;
                }
                if (strlen(*arg) > 2 &&
                    (ret = _cargparse_handle_mult_short_bool_options(self, *arg, &opt_idx)) != CARGPARSE_OK) {
                    return ret;
                } else if ((ret = _cargparse_handle_short_option(self, *arg, &opt_idx)) != CARGPARSE_OK) {
                    return ret;
                }
                break;
            case CARGPARSE_ARG_LONG:
                if (self->options[opt_idx].nargs == CARGPARSE_NARGS_ONE_OR_MORE ||
                    self->options[opt_idx].nargs == CARGPARSE_NARGS_ZERO_OR_MORE) {
                    opt_idx = -1;
                } else if (opt_idx != -1) {
                    fprintf(stderr, "Error: previous option not set\n");
                    return CARGPARSE_ERR_OPTION_NEEDS_ARG;
                }
                if ((ret = _cargparse_handle_long_option(self, *arg, &opt_idx)) != CARGPARSE_OK) {
                    return ret;
                }
                break;
            case CARGPARSE_ARG_DOUBLE_HYPHEN:
                if (self->options[opt_idx].nargs != CARGPARSE_NARGS_ONE_OR_MORE &&
                    self->options[opt_idx].nargs != CARGPARSE_NARGS_ZERO_OR_MORE && opt_idx != -1) {
                    fprintf(stderr, "Error: got '--' when previous option not set\n");
                    return CARGPARSE_ERR_OPTION_NEEDS_ARG;
                }
                last_pos_i = _cargparse_get_next_positional_opt(self, last_pos_i);
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

    if (!_cargparse_check_required_options(self)) {
        fprintf(stderr, "Error: not all required options set\n");
        return CARGPARSE_ERR_NOT_ALL_REQUIRED_OPTIONS;
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
_cargparse_get_value_generic(const cargparse_t *const self, const cargparse_option_type_e type,
                             const char short_name, const char *long_name, void *result,
                             const void *default_value, const unsigned narg) {
    if (!self) return CARGPARSE_ERR_NULL_PARSER;
    if (long_name == CARGPARSE_NO_LONG && short_name == CARGPARSE_NO_SHORT)
        return CARGPARSE_ERR_INVALID_OPTION;
    if (!result) return CARGPARSE_ERR_NULL_OUTPUT;

    cargparse_err_e ret;

    const int opt_idx = _cargparse_get_check_opt(self, type, short_name, long_name);
    if (opt_idx == -1) {
        return CARGPARSE_ERR_OPTION_UNKNOWN;
    }

    if (!self->parse_res[opt_idx].is_got) {
        switch (type) {
            case CARGPARSE_OPTION_TYPE_BOOL:
                *(bool *)result = false;
                break;
            case CARGPARSE_OPTION_TYPE_INT:
                *(long *)result = *(const long *)default_value;
                break;
            case CARGPARSE_OPTION_TYPE_FLOAT:
                *(double *)result = *(const double *)default_value;
                break;
            case CARGPARSE_OPTION_TYPE_STR:
            case CARGPARSE_OPTION_TYPE_POS:
                *(const char **)result = (const char *)default_value;
                break;
        }
        return CARGPARSE_DEFAULT_VALUE;
    }
    if (self->options[opt_idx].nargs == CARGPARSE_NARGS_ZERO_OR_MORE && self->parse_res[opt_idx].nargs == 0) {
        return CARGPARSE_ZERO_NARGS;
    }
    if (narg + 1 > (unsigned)self->parse_res[opt_idx].nargs) {
        return CARGPARSE_ERR_NARG_OUT_OF_RANGE;
    }

    switch (type) {
        case CARGPARSE_OPTION_TYPE_BOOL:
            *(bool *)result = true;
            break;
        case CARGPARSE_OPTION_TYPE_INT:
            if ((ret = _cargparse_parse_int(*(self->parse_res[opt_idx].valuestr + narg), (long *)result)) !=
                CARGPARSE_OK)
                return ret;
            break;
        case CARGPARSE_OPTION_TYPE_FLOAT:
            if ((ret = _cargparse_parse_float(*(self->parse_res[opt_idx].valuestr + narg),
                                              (double *)result)) != CARGPARSE_OK)
                return ret;
            break;
        case CARGPARSE_OPTION_TYPE_STR:
        case CARGPARSE_OPTION_TYPE_POS:
            *(const char **)result = *(self->parse_res[opt_idx].valuestr + narg);
            break;
    }

    return CARGPARSE_OK;
}

cargparse_err_e
cargparse_get_bool_long(const cargparse_t *const self, const char *long_name, bool *valuebool) {
    static const bool default_bool = false;
    return _cargparse_get_value_generic(self, CARGPARSE_OPTION_TYPE_BOOL, CARGPARSE_NO_SHORT, long_name,
                                        valuebool, &default_bool, 0);
}

cargparse_err_e
cargparse_get_bool_short(const cargparse_t *const self, const char short_name, bool *valuebool) {
    static const bool default_bool = false;
    return _cargparse_get_value_generic(self, CARGPARSE_OPTION_TYPE_BOOL, short_name, CARGPARSE_NO_LONG,
                                        valuebool, &default_bool, 0);
}

cargparse_err_e
cargparse_get_str_long(const cargparse_t *const self, const char *long_name, const char **valuestr,
                       const char *default_value, const unsigned idx) {
    return _cargparse_get_value_generic(self, CARGPARSE_OPTION_TYPE_STR, CARGPARSE_NO_SHORT, long_name,
                                        valuestr, (const void *)default_value, idx);
}

cargparse_err_e
cargparse_get_str_short(const cargparse_t *const self, const char short_name, const char **valuestr,
                        const char *default_value, const unsigned idx) {
    return _cargparse_get_value_generic(self, CARGPARSE_OPTION_TYPE_STR, short_name, CARGPARSE_NO_LONG,
                                        valuestr, (const void *)default_value, idx);
}

cargparse_err_e
cargparse_get_int_long(const cargparse_t *const self, const char *long_name, long *valueint,
                       const long default_value, const unsigned idx) {
    return _cargparse_get_value_generic(self, CARGPARSE_OPTION_TYPE_INT, CARGPARSE_NO_SHORT, long_name,
                                        valueint, &default_value, idx);
}

cargparse_err_e
cargparse_get_int_short(const cargparse_t *const self, const char short_name, long *valueint,
                        const long default_value, const unsigned idx) {
    return _cargparse_get_value_generic(self, CARGPARSE_OPTION_TYPE_INT, short_name, CARGPARSE_NO_LONG,
                                        valueint, &default_value, idx);
}

cargparse_err_e
cargparse_get_float_long(const cargparse_t *const self, const char *long_name, double *valuefloat,
                         const double default_value, const unsigned idx) {
    return _cargparse_get_value_generic(self, CARGPARSE_OPTION_TYPE_FLOAT, CARGPARSE_NO_SHORT, long_name,
                                        valuefloat, &default_value, idx);
}

cargparse_err_e
cargparse_get_float_short(const cargparse_t *const self, const char short_name, double *valuefloat,
                          const double default_value, const unsigned idx) {
    return _cargparse_get_value_generic(self, CARGPARSE_OPTION_TYPE_FLOAT, short_name, CARGPARSE_NO_LONG,
                                        valuefloat, &default_value, idx);
}

cargparse_err_e
cargparse_get_positional(const cargparse_t *const self, const char *long_name, const char **valuestr,
                         const char *default_value, const unsigned idx) {
    return _cargparse_get_value_generic(self, CARGPARSE_OPTION_TYPE_POS, CARGPARSE_NO_SHORT, long_name,
                                        valuestr, (const void *)default_value, idx);
}

static bool
_cargparse_has_option(const cargparse_t *const self, const char short_name, const char *long_name) {
    int opt_idx = _cargparse_find_opt(self, short_name, long_name);
    return (opt_idx == -1) ? false : true;
}

bool
cargparse_has_option_long(const cargparse_t *const self, const char *long_name) {
    return _cargparse_has_option(self, CARGPARSE_NO_SHORT, long_name);
}

bool
cargparse_has_option_short(const cargparse_t *const self, const char short_name) {
    return _cargparse_has_option(self, short_name, CARGPARSE_NO_LONG);
}

static cargparse_err_e
_cargparse_get_arg_count(const cargparse_t *const self, const char short_name, const char *long_name,
                         unsigned *count) {
    if (!self) return CARGPARSE_ERR_NULL_PARSER;
    if (!count) return CARGPARSE_ERR_NULL_OUTPUT;

    int opt_idx = _cargparse_find_opt(self, short_name, long_name);
    if (opt_idx == -1) {
        *count = 0;
        return CARGPARSE_ERR_OPTION_UNKNOWN;
    }
    if (!self->parse_res[opt_idx].is_got) {
        *count = 0;
        return CARGPARSE_OPT_NOT_GOT;
    }

    *count = self->parse_res[opt_idx].nargs;

    return CARGPARSE_OK;
}

cargparse_err_e
cargparse_get_arg_count_long(const cargparse_t *const self, const char *long_name, unsigned *count) {
    return _cargparse_get_arg_count(self, CARGPARSE_NO_SHORT, long_name, count);
}

cargparse_err_e
cargparse_get_arg_count_short(const cargparse_t *const self, const char short_name, unsigned *count) {
    return _cargparse_get_arg_count(self, short_name, CARGPARSE_NO_LONG, count);
}
