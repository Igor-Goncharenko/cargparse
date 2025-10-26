#ifndef CARGPARSE__H
#define CARGPARSE__H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef enum {
    CARGPARSE_OPTION_BOOL = 0,
    CARGPARSE_OPTION_INT,
    CARGPARSE_OPTION_STRING,
} cargparse_option_type_e;

typedef struct {
    const cargparse_option_type_e type;
    const char short_name;
    const char *long_name;
    const char *help;
} cargparse_option_t;

typedef struct {
    int valueint;
    char *valuestr;
    bool valuebool;
} cargparse_parse_res_t;

typedef struct {
    const char *usages;
    const char *description;
    const char *epilog;
    const cargparse_option_t *options;
    cargparse_parse_res_t *parse_res;
    const int n_options;
} cargparse_t;

#define CARGPARSE_INIT(_name, _usages, _description, _epilog, ...) \
    const cargparse_option_t _##_name##_options[] = { __VA_ARGS__ }; \
    cargparse_parse_res_t _##_name##_parse_res[sizeof(_##_name##_options) / sizeof(cargparse_option_t)] = {0}; \
    cargparse_t _name = { \
        _usages, \
        _description, \
        _epilog, \
        _##_name##_options, \
        _##_name##_parse_res, \
        sizeof(_##_name##_options) / sizeof(cargparse_option_t) \
    };

#define CARGPARSE_OPTION_INIT(_type, _short_name, _long_name, _help) \
    { \
        _type, \
        _short_name, \
        _long_name, \
        _help, \
    }

void cargparse_print_help(const cargparse_t *const self);

int cargparse_parse(cargparse_t *const self, const int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif /* CARGPARSE__H */
