#ifndef TEST_CORE_H
#define TEST_CORE_H

#include <stdio.h>
#include <string.h>

#define STATIC_ASSERT(_cond, _msg) \
    typedef char _static_assert##_msg[(_cond) ? 1 : -1] __attribute__((unused))

#define SAME_TYPE(a, b) __builtin_types_compatible_p(__typeof__(a), __typeof__(b))

#define TEST(_cond, _msg) \
    do { \
        if (!(_cond)) { \
            printf("FAIL: %s\n", (_msg)); \
            return 1; \
        } \
    } while (0)

#define TEST_EQ(_a, _b, _msg) \
    do { \
        STATIC_ASSERT(SAME_TYPE((_a), (_b)), different_types); \
        __typeof__(_a) _a_val = (_a); \
        __typeof__(_b) _b_val = (_b); \
        TEST((_a_val) == (_b_val), (_msg)); \
    } while (0)

#define TEST_EQ_STR(_a, _b, _msg) \
    TEST(strcmp((_a), (_b)) == 0, (_msg))

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

extern int total_tests;
extern int passed_tests;
extern int failed_tests;

void print_test_summary(void);

#endif /* TEST_CORE_H */
