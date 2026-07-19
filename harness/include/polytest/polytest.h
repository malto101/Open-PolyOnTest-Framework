/**
 * POT Core — public C ABI
 * Copyright 2026 Dhruv Menon
 * SPDX-License-Identifier: Apache-2.0
 *
 * Size profiles (pick one; default = full):
 *   POT_PROFILE_TINY   — text only; no tags/fixtures/float/longjmp (~1–3 KB)
 *   POT_PROFILE_SMALL  — hierarchy + tags + fixtures + COBS; float off by default
 *   POT_PROFILE_FULL   — everything (floats, tags, hierarchy, COBS, protect, mutex)
 *
 * Feature knobs (override / refine profiles):
 *   POT_MINIMAL_PRINT       — human-readable output (no COBS)
 *   POT_USE_HEAP            — enable pot_register_heap_case (malloc)
 *   POT_USE_SECTION_REGISTRY — place cases in .pot_info (see docs/profiles.md)
 *   POT_SECTION             — override linker section for descriptors
 *   POT_NO_ALIASES          — do not define TEST / ASSERT_* short names
 *   POT_EXCLUDE_FLOAT       — omit float/double asserts
 *   POT_FREESTANDING        — no-stdlib; use pot_set_writer
 *   POT_NO_LONGJMP          — PROTECT/ABORT without setjmp (abort = fail flag)
 *
 * Derived (from pot_profile.h): POT_CFG_HAS_{COBS,TAGS,FIXTURES,FLOAT,
 * PROTECT,MUTEX,EXTENDED_ASSERTS,HEAP}.
 */
#ifndef POT_H
#define POT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "polytest/polytest_profile.h"

#ifndef POT_SECTION
#if defined(__APPLE__) && defined(__MACH__)
#define POT_SECTION __attribute__((section("__DATA,pot")))
#elif defined(__GNUC__) || defined(__clang__)
#define POT_SECTION __attribute__((section(".pot_info")))
#else
#define POT_SECTION
#endif
#endif

typedef void (*pot_fn_t)(void);
typedef void (*pot_fixture_fn_t)(void);

/** Suite: fixtures run once around the whole suite. */
typedef struct pot_suite {
    const char *name;
    pot_fixture_fn_t setup;
    pot_fixture_fn_t teardown;
    const char *const *tags; /* NULL-terminated; may be NULL */
    struct pot_suite *next;
} pot_suite_t;

/** Group: fixtures run before/after each case in the group. */
typedef struct pot_group {
    const char *suite;
    const char *name;
    pot_fixture_fn_t setup;
    pot_fixture_fn_t teardown;
    const char *const *tags;
    struct pot_group *next;
} pot_group_t;

typedef struct pot_case {
    const char *suite;
    const char *group;
    const char *name;
    pot_fn_t fn;
    const char *const *tags;
    struct pot_case *next;
} pot_case_t;

/** Byte sink — board/transport glue provides this (Dependency Inversion). */
typedef void (*pot_write_fn_t)(const void *data, size_t len, void *user);

void pot_set_writer(pot_write_fn_t fn, void *user);

#if POT_CFG_HAS_MUTEX
/** Optional lock hooks for multithreaded host (full profile). NULL = no-op. */
typedef void (*pot_lock_fn_t)(void *user);
void pot_set_locks(pot_lock_fn_t lock, pot_lock_fn_t unlock,
                        void *user);
#endif

void pot_register_suite(pot_suite_t *suite);
void pot_register_group(pot_group_t *group);
void pot_register(pot_case_t *test_case);

#if POT_CFG_HAS_HEAP
/**
 * Heap-allocate a case descriptor and register it.
 * Returns 0 on success, -1 on allocation failure.
 */
int pot_register_heap_case(const char *suite, const char *group,
                                const char *name, pot_fn_t fn);
#endif

int pot_run_all(void);
/** Run cases whose suite, group, or case tags include `tag` (stub on tiny). */
int pot_run_tag(const char *tag);
/** Run cases belonging to `suite` (strcmp on case suite name). */
int pot_run_suite(const char *suite);
/** Run cases in `suite`/`group`. */
int pot_run_group(const char *suite, const char *group);
/** Run a specific test case by name. */
int pot_run_case(const char *suite, const char *group, const char *case_name);
/**
 * Host helper: honor environment routing:
 * - POLY_DISCOVER=1: Print all discovered tests and exit.
 * - POLY_CASE=<name>: Run single test case matching suite/group/case.
 * - POLY_SUITE / POT_SUITE, POLY_GROUP / POT_GROUP, POT_TAG:
 *   Filter executed tests by suite, group, or tag.
 * Otherwise run all. On freestanding targets, always runs all.
 */
int pot_run_from_env(void);

/** Parameterized-test cursor (set by PARAM_TEST / FOR_EACH). */
void pot_set_param(size_t index, const void *param);
void pot_clear_param(void);
size_t pot_param_index(void);
const void *pot_current_param(void);

void pot_ignore(const char *message);
int pot_protect(void);
void pot_abort(void);

void pot_fail(const char *message, const char *file, int line);
void pot_fail_at(const char *file, int line, const char *message);

void pot_assert_true(int cond, const char *expr, const char *msg,
                          const char *file, int line);
void pot_assert_false(int cond, const char *expr, const char *msg,
                           const char *file, int line);
void pot_assert_null(const void *ptr, const char *msg, const char *file,
                          int line);
void pot_assert_not_null(const void *ptr, const char *msg, const char *file,
                              int line);

void pot_assert_int(int64_t expected, int64_t actual, int size,
                         int is_hex, const char *msg, const char *file,
                         int line);
void pot_assert_uint(uint64_t expected, uint64_t actual, int size,
                          int is_hex, const char *msg, const char *file,
                          int line);
void pot_assert_not_equal_int(int64_t expected, int64_t actual,
                                   const char *msg, const char *file, int line);
void pot_assert_greater_than(int64_t threshold, int64_t actual,
                                  const char *msg, const char *file, int line);
void pot_assert_less_than(int64_t threshold, int64_t actual,
                               const char *msg, const char *file, int line);
void pot_assert_int_within(int64_t delta, int64_t expected, int64_t actual,
                                const char *msg, const char *file, int line);

#ifndef POT_EXCLUDE_FLOAT
void pot_assert_float_within(float delta, float expected, float actual,
                                  const char *msg, const char *file, int line);
void pot_assert_double_within(double delta, double expected, double actual,
                                   const char *msg, const char *file, int line);
#endif

#if POT_CFG_HAS_EXTENDED_ASSERTS
void pot_assert_string(const char *expected, const char *actual,
                            const char *msg, const char *file, int line);
void pot_assert_string_len(const char *expected, const char *actual,
                                size_t len, const char *msg, const char *file,
                                int line);
void pot_assert_memory(const void *expected, const void *actual, size_t len,
                            const char *msg, const char *file, int line);
void pot_assert_int_array(const int *expected, const int *actual,
                               size_t num, const char *msg, const char *file,
                               int line);
void pot_assert_uint8_array(const uint8_t *expected, const uint8_t *actual,
                                 size_t num, const char *msg, const char *file,
                                 int line);
void pot_assert_bits(uint32_t mask, uint32_t expected, uint32_t actual,
                          const char *msg, const char *file, int line);
void pot_assert_bits_high(uint32_t mask, uint32_t actual, const char *msg,
                               const char *file, int line);
void pot_assert_bits_low(uint32_t mask, uint32_t actual, const char *msg,
                              const char *file, int line);
#endif

/* -------------------------------------------------------------------------- */
/* Assert macros                                                              */
/* -------------------------------------------------------------------------- */

#define POT_ASSERT_TRUE(cond)                                             \
    pot_assert_true(!!(cond), #cond, NULL, __FILE__, __LINE__)
#define POT_ASSERT_TRUE_MESSAGE(cond, msg)                                \
    pot_assert_true(!!(cond), #cond, (msg), __FILE__, __LINE__)
#define POT_ASSERT_FALSE(cond)                                            \
    pot_assert_false(!!(cond), #cond, NULL, __FILE__, __LINE__)
#define POT_ASSERT_FALSE_MESSAGE(cond, msg)                               \
    pot_assert_false(!!(cond), #cond, (msg), __FILE__, __LINE__)

#define POT_ASSERT_NULL(ptr)                                              \
    pot_assert_null((const void *)(ptr), NULL, __FILE__, __LINE__)
#define POT_ASSERT_NULL_MESSAGE(ptr, msg)                                 \
    pot_assert_null((const void *)(ptr), (msg), __FILE__, __LINE__)
#define POT_ASSERT_NOT_NULL(ptr)                                          \
    pot_assert_not_null((const void *)(ptr), NULL, __FILE__, __LINE__)
#define POT_ASSERT_NOT_NULL_MESSAGE(ptr, msg)                             \
    pot_assert_not_null((const void *)(ptr), (msg), __FILE__, __LINE__)

#define POT_ASSERT_EQUAL_INT(expected, actual)                            \
    pot_assert_int((int64_t)(expected), (int64_t)(actual), (int)sizeof(int), \
                        0, NULL, __FILE__, __LINE__)
#define POT_ASSERT_EQUAL_INT_MESSAGE(expected, actual, msg)               \
    pot_assert_int((int64_t)(expected), (int64_t)(actual), (int)sizeof(int), \
                        0, (msg), __FILE__, __LINE__)
#define POT_ASSERT_EQUAL_INT8(expected, actual)                           \
    pot_assert_int((int64_t)(int8_t)(expected), (int64_t)(int8_t)(actual), \
                        1, 0, NULL, __FILE__, __LINE__)
#define POT_ASSERT_EQUAL_INT16(expected, actual)                          \
    pot_assert_int((int64_t)(int16_t)(expected),                          \
                        (int64_t)(int16_t)(actual), 2, 0, NULL, __FILE__,      \
                        __LINE__)
#define POT_ASSERT_EQUAL_INT32(expected, actual)                          \
    pot_assert_int((int64_t)(int32_t)(expected),                          \
                        (int64_t)(int32_t)(actual), 4, 0, NULL, __FILE__,      \
                        __LINE__)
#define POT_ASSERT_EQUAL_INT64(expected, actual)                          \
    pot_assert_int((int64_t)(expected), (int64_t)(actual), 8, 0, NULL,    \
                        __FILE__, __LINE__)

#define POT_ASSERT_EQUAL_UINT(expected, actual)                           \
    pot_assert_uint((uint64_t)(expected), (uint64_t)(actual),             \
                         (int)sizeof(unsigned), 0, NULL, __FILE__, __LINE__)
#define POT_ASSERT_EQUAL_UINT8(expected, actual)                          \
    pot_assert_uint((uint64_t)(uint8_t)(expected),                        \
                         (uint64_t)(uint8_t)(actual), 1, 0, NULL, __FILE__,    \
                         __LINE__)
#define POT_ASSERT_EQUAL_UINT16(expected, actual)                         \
    pot_assert_uint((uint64_t)(uint16_t)(expected),                       \
                         (uint64_t)(uint16_t)(actual), 2, 0, NULL, __FILE__,   \
                         __LINE__)
#define POT_ASSERT_EQUAL_UINT32(expected, actual)                         \
    pot_assert_uint((uint64_t)(uint32_t)(expected),                       \
                         (uint64_t)(uint32_t)(actual), 4, 0, NULL, __FILE__,   \
                         __LINE__)
#define POT_ASSERT_EQUAL_UINT64(expected, actual)                         \
    pot_assert_uint((uint64_t)(expected), (uint64_t)(actual), 8, 0, NULL, \
                         __FILE__, __LINE__)

#define POT_ASSERT_EQUAL_HEX8(expected, actual)                           \
    pot_assert_uint((uint64_t)(uint8_t)(expected),                        \
                         (uint64_t)(uint8_t)(actual), 1, 1, NULL, __FILE__,    \
                         __LINE__)
#define POT_ASSERT_EQUAL_HEX16(expected, actual)                          \
    pot_assert_uint((uint64_t)(uint16_t)(expected),                       \
                         (uint64_t)(uint16_t)(actual), 2, 1, NULL, __FILE__,   \
                         __LINE__)
#define POT_ASSERT_EQUAL_HEX32(expected, actual)                          \
    pot_assert_uint((uint64_t)(uint32_t)(expected),                       \
                         (uint64_t)(uint32_t)(actual), 4, 1, NULL, __FILE__,   \
                         __LINE__)
#define POT_ASSERT_EQUAL_HEX64(expected, actual)                          \
    pot_assert_uint((uint64_t)(expected), (uint64_t)(actual), 8, 1, NULL, \
                         __FILE__, __LINE__)

#define POT_ASSERT_NOT_EQUAL_INT(expected, actual)                        \
    pot_assert_not_equal_int((int64_t)(expected), (int64_t)(actual), NULL,\
                                  __FILE__, __LINE__)
#define POT_ASSERT_GREATER_THAN(threshold, actual)                        \
    pot_assert_greater_than((int64_t)(threshold), (int64_t)(actual), NULL,\
                                 __FILE__, __LINE__)
#define POT_ASSERT_LESS_THAN(threshold, actual)                           \
    pot_assert_less_than((int64_t)(threshold), (int64_t)(actual), NULL,   \
                              __FILE__, __LINE__)
#define POT_ASSERT_INT_WITHIN(delta, expected, actual)                    \
    pot_assert_int_within((int64_t)(delta), (int64_t)(expected),          \
                               (int64_t)(actual), NULL, __FILE__, __LINE__)
#define POT_ASSERT_INT_WITHIN_MESSAGE(delta, expected, actual, msg)       \
    pot_assert_int_within((int64_t)(delta), (int64_t)(expected),          \
                               (int64_t)(actual), (msg), __FILE__, __LINE__)

#ifndef POT_EXCLUDE_FLOAT
#define POT_ASSERT_FLOAT_WITHIN(delta, expected, actual)                  \
    pot_assert_float_within((float)(delta), (float)(expected),            \
                                 (float)(actual), NULL, __FILE__, __LINE__)
#define POT_ASSERT_EQUAL_FLOAT(expected, actual)                          \
    POT_ASSERT_FLOAT_WITHIN(0.00001f, (expected), (actual))
#define POT_ASSERT_DOUBLE_WITHIN(delta, expected, actual)                 \
    pot_assert_double_within((double)(delta), (double)(expected),         \
                                  (double)(actual), NULL, __FILE__, __LINE__)
#define POT_ASSERT_EQUAL_DOUBLE(expected, actual)                         \
    POT_ASSERT_DOUBLE_WITHIN(1.0e-12, (expected), (actual))
#endif

#if POT_CFG_HAS_EXTENDED_ASSERTS
#define POT_ASSERT_EQUAL_STRING(expected, actual)                         \
    pot_assert_string((expected), (actual), NULL, __FILE__, __LINE__)
#define POT_ASSERT_EQUAL_STRING_MESSAGE(expected, actual, msg)            \
    pot_assert_string((expected), (actual), (msg), __FILE__, __LINE__)
#define POT_ASSERT_EQUAL_STRING_LEN(expected, actual, len)                \
    pot_assert_string_len((expected), (actual), (size_t)(len), NULL,      \
                               __FILE__, __LINE__)
#define POT_ASSERT_EQUAL_MEMORY(expected, actual, len)                    \
    pot_assert_memory((expected), (actual), (size_t)(len), NULL,          \
                           __FILE__, __LINE__)
#define POT_ASSERT_EQUAL_INT_ARRAY(expected, actual, num)                 \
    pot_assert_int_array((expected), (actual), (size_t)(num), NULL,       \
                              __FILE__, __LINE__)
#define POT_ASSERT_EQUAL_UINT8_ARRAY(expected, actual, num)               \
    pot_assert_uint8_array((expected), (actual), (size_t)(num), NULL,     \
                                __FILE__, __LINE__)

#define POT_ASSERT_BITS(mask, expected, actual)                           \
    pot_assert_bits((uint32_t)(mask), (uint32_t)(expected),               \
                         (uint32_t)(actual), NULL, __FILE__, __LINE__)
#define POT_ASSERT_BITS_HIGH(mask, actual)                                \
    pot_assert_bits_high((uint32_t)(mask), (uint32_t)(actual), NULL,      \
                              __FILE__, __LINE__)
#define POT_ASSERT_BITS_LOW(mask, actual)                                 \
    pot_assert_bits_low((uint32_t)(mask), (uint32_t)(actual), NULL,       \
                             __FILE__, __LINE__)
#endif

#define POT_FAIL(msg) pot_fail((msg), __FILE__, __LINE__)
#define POT_FAIL_MESSAGE(msg) POT_FAIL(msg)

#define POT_IGNORE()                                                      \
    do {                                                                       \
        pot_ignore(NULL);                                                 \
        return;                                                                \
    } while (0)
#define POT_IGNORE_MESSAGE(msg)                                           \
    do {                                                                       \
        pot_ignore(msg);                                                  \
        return;                                                                \
    } while (0)

#define POT_PROTECT() (pot_protect())
#define POT_ABORT() pot_abort()

/* -------------------------------------------------------------------------- */
/* Suite / group / case registration                                          */
/* -------------------------------------------------------------------------- */

#if POT_CFG_HAS_FIXTURES

/** Define the shared suite object (use once per suite; SETUP/TEARDOWN/TAGS attach). */
#define POT_SUITE(suite_name)                                             \
    static pot_suite_t pot_suite_##suite_name = {                    \
        #suite_name, NULL, NULL, NULL, NULL};                                  \
    static void pot_suite_base_reg_##suite_name(void)                     \
        __attribute__((constructor));                                          \
    static void pot_suite_base_reg_##suite_name(void) {                   \
        pot_register_suite(&pot_suite_##suite_name);                 \
    }

/**
 * Define suite + setup. Prefer this *or* POT_SUITE (not both) so there is
 * a single pot_suite_##name object. TEARDOWN/TAGS mutate the same object.
 */
#define POT_SUITE_SETUP(suite_name)                                       \
    static void pot_suite_setup_##suite_name(void);                       \
    static pot_suite_t pot_suite_##suite_name = {                    \
        #suite_name, pot_suite_setup_##suite_name, NULL, NULL, NULL};     \
    static void pot_suite_su_reg_##suite_name(void)                       \
        __attribute__((constructor));                                          \
    static void pot_suite_su_reg_##suite_name(void) {                     \
        pot_register_suite(&pot_suite_##suite_name);                 \
    }                                                                          \
    static void pot_suite_setup_##suite_name(void)

#define POT_SUITE_TEARDOWN(suite_name)                                    \
    static void pot_suite_teardown_##suite_name(void);                    \
    static void pot_suite_td_reg_##suite_name(void)                       \
        __attribute__((constructor));                                          \
    static void pot_suite_td_reg_##suite_name(void) {                     \
        pot_suite_##suite_name.teardown =                                 \
            pot_suite_teardown_##suite_name;                              \
        pot_register_suite(&pot_suite_##suite_name);                 \
    }                                                                          \
    static void pot_suite_teardown_##suite_name(void)

#if POT_CFG_HAS_TAGS
#define POT_SUITE_TAGS(suite_name, ...)                                   \
    static const char *const pot_suite_tags_##suite_name[] = {            \
        __VA_ARGS__, NULL};                                                    \
    static void pot_suite_tags_reg_##suite_name(void)                     \
        __attribute__((constructor));                                          \
    static void pot_suite_tags_reg_##suite_name(void) {                   \
        pot_suite_##suite_name.tags = pot_suite_tags_##suite_name;   \
        pot_register_suite(&pot_suite_##suite_name);                 \
    }
#else
#define POT_SUITE_TAGS(suite_name, ...)                                   \
    enum { pot_suite_tags_unused_##suite_name = 0 }
#endif

#define POT_GROUP_SETUP(suite_name, group_name)                           \
    static void pot_group_setup_##suite_name##_##group_name(void);        \
    static pot_group_t pot_group_##suite_name##_##group_name = {     \
        #suite_name,                                                           \
        #group_name,                                                           \
        pot_group_setup_##suite_name##_##group_name,                      \
        NULL,                                                                  \
        NULL,                                                                  \
        NULL};                                                                  \
    static void pot_group_su_reg_##suite_name##_##group_name(void)        \
        __attribute__((constructor));                                          \
    static void pot_group_su_reg_##suite_name##_##group_name(void) {      \
        pot_register_group(                                               \
            &pot_group_##suite_name##_##group_name);                      \
    }                                                                          \
    static void pot_group_setup_##suite_name##_##group_name(void)

#define POT_GROUP_TEARDOWN(suite_name, group_name)                        \
    static void pot_group_teardown_##suite_name##_##group_name(void);     \
    static void pot_group_td_reg_##suite_name##_##group_name(void)        \
        __attribute__((constructor));                                          \
    static void pot_group_td_reg_##suite_name##_##group_name(void) {      \
        pot_group_##suite_name##_##group_name.teardown =                  \
            pot_group_teardown_##suite_name##_##group_name;               \
        pot_register_group(                                               \
            &pot_group_##suite_name##_##group_name);                      \
    }                                                                          \
    static void pot_group_teardown_##suite_name##_##group_name(void)

#if POT_CFG_HAS_TAGS
#define POT_GROUP_TAGS(suite_name, group_name, ...)                       \
    static const char *const pot_group_tags_##suite_name##_##group_name[] \
        = {__VA_ARGS__, NULL};                                                 \
    static void pot_group_tags_reg_##suite_name##_##group_name(void)      \
        __attribute__((constructor));                                          \
    static void pot_group_tags_reg_##suite_name##_##group_name(void) {    \
        pot_group_##suite_name##_##group_name.tags =                      \
            pot_group_tags_##suite_name##_##group_name;                   \
        pot_register_group(                                               \
            &pot_group_##suite_name##_##group_name);                      \
    }
#else
#define POT_GROUP_TAGS(suite_name, group_name, ...)                       \
    enum { pot_group_tags_unused_##suite_name##_##group_name = 0 }
#endif

#else /* !POT_CFG_HAS_FIXTURES — tiny: accept macros, skip registration */

#define POT_SUITE(suite_name)                                             \
    enum { pot_suite_unused_##suite_name = 0 }
#define POT_SUITE_SETUP(suite_name)                                       \
    static void pot_suite_setup_##suite_name(void)
#define POT_SUITE_TEARDOWN(suite_name)                                    \
    static void pot_suite_teardown_##suite_name(void)
#define POT_SUITE_TAGS(suite_name, ...)                                   \
    enum { pot_suite_tags_unused_##suite_name = 0 }
#define POT_GROUP_SETUP(suite_name, group_name)                           \
    static void pot_group_setup_##suite_name##_##group_name(void)
#define POT_GROUP_TEARDOWN(suite_name, group_name)                        \
    static void pot_group_teardown_##suite_name##_##group_name(void)
#define POT_GROUP_TAGS(suite_name, group_name, ...)                       \
    enum { pot_group_tags_unused_##suite_name##_##group_name = 0 }

#endif /* POT_CFG_HAS_FIXTURES */

#ifdef POT_USE_SECTION_REGISTRY
#define POT_TEST(suite_name, group_name, case_name)                       \
    static void pot_body_##suite_name##_##group_name##_##case_name(void); \
    static pot_case_t                                                     \
        pot_desc_##suite_name##_##group_name##_##case_name                \
            POT_SECTION = {#suite_name,                                   \
                                #group_name,                                   \
                                #case_name,                                    \
                                pot_body_##suite_name##_##group_name##_##case_name, \
                                NULL,                                          \
                                NULL};                                          \
    static void pot_body_##suite_name##_##group_name##_##case_name(void)
#else
#define POT_TEST(suite_name, group_name, case_name)                       \
    static void pot_body_##suite_name##_##group_name##_##case_name(void); \
    static pot_case_t                                                     \
        pot_desc_##suite_name##_##group_name##_##case_name = {            \
            #suite_name,                                                       \
            #group_name,                                                       \
            #case_name,                                                        \
            pot_body_##suite_name##_##group_name##_##case_name,           \
            NULL,                                                              \
            NULL};                                                              \
    static void pot_reg_##suite_name##_##group_name##_##case_name(void)   \
        __attribute__((constructor));                                          \
    static void pot_reg_##suite_name##_##group_name##_##case_name(void) { \
        pot_register(                                                     \
            &pot_desc_##suite_name##_##group_name##_##case_name);         \
    }                                                                          \
    static void pot_body_##suite_name##_##group_name##_##case_name(void)
#endif

#if POT_CFG_HAS_TAGS
#define POT_TEST_TAGS(suite_name, group_name, case_name, ...)             \
    static void pot_body_##suite_name##_##group_name##_##case_name(void); \
    static const char *const                                                   \
        pot_case_tags_##suite_name##_##group_name##_##case_name[] = {     \
            __VA_ARGS__, NULL};                                                \
    static pot_case_t                                                     \
        pot_desc_##suite_name##_##group_name##_##case_name = {            \
            #suite_name,                                                       \
            #group_name,                                                       \
            #case_name,                                                        \
            pot_body_##suite_name##_##group_name##_##case_name,           \
            pot_case_tags_##suite_name##_##group_name##_##case_name,      \
            NULL};                                                              \
    static void pot_reg_##suite_name##_##group_name##_##case_name(void)   \
        __attribute__((constructor));                                          \
    static void pot_reg_##suite_name##_##group_name##_##case_name(void) { \
        pot_register(                                                     \
            &pot_desc_##suite_name##_##group_name##_##case_name);         \
    }                                                                          \
    static void pot_body_##suite_name##_##group_name##_##case_name(void)
#else
#define POT_TEST_TAGS(suite_name, group_name, case_name, ...)             \
    POT_TEST(suite_name, group_name, case_name)
#endif

/**
 * Parameterized helper — invoke body once per table row from inside a TEST.
 * Sets the param cursor so failures can append `[param=<index>]`.
 */
#define POT_FOR_EACH(type, var, array)                                    \
    for (size_t _pt_i = 0;                                                     \
         _pt_i < sizeof(array) / sizeof((array)[0]); ++_pt_i)                  \
        for (type var = (array)[_pt_i],                                         \
                 *_pt_once = (pot_set_param(_pt_i, &(var)), &var);        \
             _pt_once;                                                         \
             _pt_once = (pot_clear_param(), (type *)NULL))

/** Typed view of the current PARAM_TEST / FOR_EACH row. */
#define POT_PARAM_AS(type) (*(const type *)pot_current_param())

/**
 * Register one case that runs `table` row-by-row. Body uses PARAM_AS(type).
 * Requires small/full profile (fixtures/hierarchy enabled).
 */
#if POT_CFG_HAS_FIXTURES
#define POT_PARAM_TEST(suite_name, group_name, case_name, type, table)    \
    static void pot_param_impl_##suite_name##_##group_name##_##case_name( \
        void);                                                                 \
    POT_TEST(suite_name, group_name, case_name) {                         \
        size_t _pt_n = sizeof(table) / sizeof((table)[0]);                     \
        size_t _pt_i;                                                          \
        for (_pt_i = 0; _pt_i < _pt_n; ++_pt_i) {                              \
            pot_set_param(_pt_i, &(table)[_pt_i]);                        \
            pot_param_impl_##suite_name##_##group_name##_##case_name();   \
            pot_clear_param();                                            \
        }                                                                      \
    }                                                                          \
    static void pot_param_impl_##suite_name##_##group_name##_##case_name( \
        void)
#else
#define POT_PARAM_TEST(suite_name, group_name, case_name, type, table)    \
    POT_PARAM_TEST_requires_small_or_full_profile(suite_name, group_name, \
                                                       case_name, type, table)
#endif

#ifndef POT_NO_ALIASES
#define TEST POT_TEST
#define TEST_TAGS POT_TEST_TAGS
#define FOR_EACH POT_FOR_EACH
#define PARAM_AS POT_PARAM_AS
#if POT_CFG_HAS_FIXTURES
#define PARAM_TEST POT_PARAM_TEST
#endif
#define ASSERT_TRUE POT_ASSERT_TRUE
#define ASSERT_TRUE_MESSAGE POT_ASSERT_TRUE_MESSAGE
#define ASSERT_FALSE POT_ASSERT_FALSE
#define ASSERT_FALSE_MESSAGE POT_ASSERT_FALSE_MESSAGE
#define ASSERT_EQ POT_ASSERT_EQUAL_INT
#define ASSERT_NE POT_ASSERT_NOT_EQUAL_INT
#define ASSERT_EQUAL_INT POT_ASSERT_EQUAL_INT
#define ASSERT_EQUAL_UINT POT_ASSERT_EQUAL_UINT
#define ASSERT_EQUAL_HEX32 POT_ASSERT_EQUAL_HEX32
#define ASSERT_NULL POT_ASSERT_NULL
#define ASSERT_NOT_NULL POT_ASSERT_NOT_NULL
#if POT_CFG_HAS_EXTENDED_ASSERTS
#define ASSERT_BITS POT_ASSERT_BITS
#define ASSERT_BITS_HIGH POT_ASSERT_BITS_HIGH
#define ASSERT_BITS_LOW POT_ASSERT_BITS_LOW
#define ASSERT_EQUAL_STRING POT_ASSERT_EQUAL_STRING
#define ASSERT_EQUAL_MEMORY POT_ASSERT_EQUAL_MEMORY
#endif
#define ASSERT_GREATER_THAN POT_ASSERT_GREATER_THAN
#define ASSERT_LESS_THAN POT_ASSERT_LESS_THAN
#define ASSERT_INT_WITHIN POT_ASSERT_INT_WITHIN
#define FAIL POT_FAIL
#define IGNORE POT_IGNORE
#define IGNORE_MESSAGE POT_IGNORE_MESSAGE
#define TEST_PROTECT POT_PROTECT
#define TEST_ABORT POT_ABORT
#ifndef POT_EXCLUDE_FLOAT
#define ASSERT_FLOAT_WITHIN POT_ASSERT_FLOAT_WITHIN
#define ASSERT_EQUAL_FLOAT POT_ASSERT_EQUAL_FLOAT
#define ASSERT_DOUBLE_WITHIN POT_ASSERT_DOUBLE_WITHIN
#define ASSERT_EQUAL_DOUBLE POT_ASSERT_EQUAL_DOUBLE
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* POT_H */
