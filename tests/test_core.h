#ifndef TEST_CORE_H
#define TEST_CORE_H

#include <stdio.h>
#include <string.h>

#include "../cargparse.h"

#define STATIC_ASSERT(_cond, _msg) \
    typedef char _static_assert##_msg[(_cond) ? 1 : -1] __attribute__((unused))

#define SAME_TYPE(a, b) __builtin_types_compatible_p(__typeof__(a), __typeof__(b))

#define TEST(_cond) \
    do { \
        if (!(_cond)) { \
            printf("FAIL: %s:%s:%d\n", __FILE__, __func__, __LINE__); \
            return 1; \
        } \
    } while (0)

#define TEST_EQ(_a, _b) \
    do { \
        STATIC_ASSERT(SAME_TYPE((_a), (_b)), different_types); \
        __typeof__(_a) _a_val = (_a); \
        __typeof__(_b) _b_val = (_b); \
        TEST((_a_val) == (_b_val)); \
    } while (0)

#define TEST_IS_NULL(_a) \
    TEST((_a) == NULL)

#define TEST_IS_NOT_NULL(_a) \
    TEST((_a) != NULL)

#define TEST_EQ_STR(_a, _b) \
    do { \
        if ((_a) != NULL && (_b) != NULL) { \
            TEST(strcmp((_a), (_b)) == 0); \
        } else { \
            TEST_IS_NULL((_a)); \
            TEST_IS_NULL((_b)); \
        } \
    } while (0)


#define RUN_TEST(_test_func) \
    do { \
        int res = _test_func(); \
        total_tests++; \
        if (res == 0) { \
            printf("PASS: %s\n", #_test_func); \
            passed_tests++; \
        } else { \
            failed_tests++; \
        } \
    } while (0)

#define CARGPARSE_PARSE_RES_CLEANUP(_ap) \
    do { \
        STATIC_ASSERT(SAME_TYPE((_ap), cargparse_t*), cargparse_type); \
        memset((_ap)->parse_res, 0, sizeof(cargparse_parse_res_t) * (_ap)->n_options); \
    } while (0)

#define TEST_PARSE_ERROR(_ap, _expected_result, ...) \
    do { \
        char *_argv[] = { "program", __VA_ARGS__ }; \
        int _argc = sizeof(_argv) / sizeof(char*); \
        int _result = cargparse_parse((_ap), _argc, _argv); \
        CARGPARSE_PARSE_RES_CLEANUP((_ap)); \
        TEST(_result == (_expected_result)); \
    } while (0)

extern int total_tests;
extern int passed_tests;
extern int failed_tests;

void print_test_summary(void);

int cmp_options(const cargparse_option_t *opt1, const cargparse_option_t *opt2);

int cmp_parse_res(const cargparse_parse_res_t *pr1, const cargparse_parse_res_t *pr2);

#endif /* TEST_CORE_H */
