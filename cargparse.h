#ifndef CARGPARSE_H
#define CARGPARSE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef enum {
    CARGPARSE_OPTION_TYPE_POS = 0,
    CARGPARSE_OPTION_TYPE_BOOL,
    CARGPARSE_OPTION_TYPE_INT,
    CARGPARSE_OPTION_TYPE_FLOAT,
    CARGPARSE_OPTION_TYPE_STR,
} cargparse_option_type_e;

typedef enum {
    CARGPARSE_FLAG_NONE = 0,
    CARGPARSE_FLAG_REQUIRED = 1 << 0,
} cargparse_option_flag_e;

typedef enum {
    CARGPARSE_OK = 0,
    CARGPARSE_DEFAULT_VALUE,
    CARGPARSE_GOT_ZERO_ARGS,

    CARGPARSE_ERR_NULL_PARSER,
    CARGPARSE_ERR_NULL_OUTPUT,
    CARGPARSE_ERR_NULL_ARGUMENT,

    CARGPARSE_ERR_OPTION_UNKNOWN,
    CARGPARSE_ERR_OPTION_NEEDS_ARG,
    CARGPARSE_ERR_OPTION_ALREADY_SET,
    CARGPARSE_ERR_OPTION_INCORRECT_TYPE,
    CARGPARSE_ERR_PREVIOUS_OPTION_NOT_SET,
    CARGPARSE_ERR_INVALID_OPTION,

    CARGPARSE_ERR_NOT_ALL_REQUIRED_OPTIONS,
    CARGPARSE_ERR_UNEXPECTED_POSITIONAL,
    CARGPARSE_ERR_INVALID_VALUE,
    CARGPARSE_ERR_NOT_BOOL_IN_MULT_BOOL_DEF,

    CARGPARSE_ERR_NARG_OUT_OF_RANGE,
} cargparse_err_e;

typedef struct {
    const cargparse_option_type_e type;
    const char short_name;
    const char *long_name;
    const char *help;
    const int flags;
    const int nargs;
} cargparse_option_t;

typedef struct {
    bool is_got;
    char **valuestr;
    int nargs;
} cargparse_parse_res_t;

typedef struct {
    const char *usages;
    const char *description;
    const char *epilog;
    const cargparse_option_t *options;
    cargparse_parse_res_t *parse_res;
    const int n_options;
} cargparse_t;

#define CARGPARSE_NARGS_ONE_OR_MORE (-111)
#define CARGPARSE_NARGS_ZERO_OR_MORE (-222)

#define CARGPARSE_NO_SHORT (-1)
#define CARGPARSE_NO_LONG (NULL)

#define CARGPARSE_INIT(_name, _usages, _description, _epilog, ...)                                          \
    const cargparse_option_t _##_name##_options[] = {__VA_ARGS__};                                          \
    cargparse_parse_res_t _##_name##_parse_res[sizeof(_##_name##_options) / sizeof(cargparse_option_t)] = { \
        0};                                                                                                 \
    cargparse_t _name = {_usages,                                                                           \
                         _description,                                                                      \
                         _epilog,                                                                           \
                         _##_name##_options,                                                                \
                         _##_name##_parse_res,                                                              \
                         sizeof(_##_name##_options) / sizeof(cargparse_option_t)};

#define CARGPARSE_OPTION_INIT(_type, _short_name, _long_name, _help, _flags, _nargs) \
    {                                                                                \
        _type, _short_name, _long_name, _help, _flags, _nargs,                       \
    }

#define CARGPARSE_OPTION_INT(_short_name, _long_name, _help, _flags, _nargs) \
    CARGPARSE_OPTION_INIT(CARGPARSE_OPTION_TYPE_INT, _short_name, _long_name, _help, _flags, _nargs)

#define CARGPARSE_OPTION_FLOAT(_short_name, _long_name, _help, _flags, _nargs) \
    CARGPARSE_OPTION_INIT(CARGPARSE_OPTION_TYPE_FLOAT, _short_name, _long_name, _help, _flags, _nargs)

#define CARGPARSE_OPTION_BOOL(_short_name, _long_name, _help, _flags) \
    CARGPARSE_OPTION_INIT(CARGPARSE_OPTION_TYPE_BOOL, _short_name, _long_name, _help, _flags, 1)

#define CARGPARSE_OPTION_STRING(_short_name, _long_name, _help, _flags, _nargs) \
    CARGPARSE_OPTION_INIT(CARGPARSE_OPTION_TYPE_STR, _short_name, _long_name, _help, _flags, _nargs)

#define CARGPARSE_OPTION_POSITIONAL(_long_name, _help, _flags) \
    CARGPARSE_OPTION_INIT(CARGPARSE_OPTION_TYPE_POS, CARGPARSE_NO_SHORT, _long_name, _help, _flags, 1)

void
cargparse_print_help(const cargparse_t *const self);

cargparse_err_e
cargparse_parse(cargparse_t *const self, const int argc, char **argv);

cargparse_err_e
cargparse_get_bool_long(const cargparse_t *const self, const char *long_name, bool *valuebool);

cargparse_err_e
cargparse_get_bool_short(const cargparse_t *const self, const char short_name, bool *valuebool);

cargparse_err_e
cargparse_get_str_long(const cargparse_t *const self, const char *long_name, const char **valuestr,
                       const char *default_value, const unsigned idx);

cargparse_err_e
cargparse_get_str_short(const cargparse_t *const self, const char short_name, const char **valuestr,
                        const char *default_value, const unsigned idx);

cargparse_err_e
cargparse_get_int_long(const cargparse_t *const self, const char *long_name, long *valueint,
                       const long default_value, const unsigned idx);

cargparse_err_e
cargparse_get_int_short(const cargparse_t *const self, const char short_name, long *valueint,
                        const long default_value, const unsigned idx);

cargparse_err_e
cargparse_get_float_long(const cargparse_t *const self, const char *long_name, double *valuefloat,
                         const double default_value, const unsigned idx);

cargparse_err_e
cargparse_get_float_short(const cargparse_t *const self, const char short_name, double *valuefloat,
                          const double default_value, const unsigned idx);

cargparse_err_e
cargparse_get_positional(const cargparse_t *const self, const char *long_name, const char **valuestr,
                         const char *default_value);

#ifdef __cplusplus
}
#endif

#endif /* CARGPARSE_H */
