#ifndef CARGPARSE__H
#define CARGPARSE__H

#ifdef __cplusplus
extern "C" {
#endif

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
    const char *const *usages;
    const char *description;
    const char *epilog;
    const cargparse_option_t *options;
    int n_options;
} cargparse_t;

#define CARGPARSE_INIT(_usages, _description, _epilog, _options, _n_options) \
    { \
        _usages, \
        _description, \
        _epilog, \
        _options, \
        _n_options, \
    }

#define CARGPARSE_OPTION_INIT(_type, _short_name, _long_name, _help) \
    { \
        _type, \
        _short_name, \
        _long_name, \
        _help, \
    }

void cargparse_print_help(const cargparse_t *const self);

#ifdef __cplusplus
}
#endif

#endif /* CARGPARSE__H */
