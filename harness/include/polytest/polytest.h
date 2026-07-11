/**
 * PolyTest Core — public C ABI
 * Copyright 2026 Dhruv Menon
 * SPDX-License-Identifier: Apache-2.0
 *
 * Size profiles (pick one; default = full):
 *   POLYTEST_PROFILE_TINY   — text only; no tags/fixtures/float/longjmp (~1–3 KB)
 *   POLYTEST_PROFILE_SMALL  — hierarchy + tags + fixtures + COBS; float off by default
 *   POLYTEST_PROFILE_FULL   — everything (floats, tags, hierarchy, COBS, protect, mutex)
 *
 * Feature knobs (override / refine profiles):
 *   POLYTEST_MINIMAL_PRINT       — human-readable output (no COBS)
 *   POLYTEST_USE_HEAP            — enable polytest_register_heap_case (malloc)
 *   POLYTEST_USE_SECTION_REGISTRY — place cases in .polytest_info (see docs/profiles.md)
 *   POLYTEST_SECTION             — override linker section for descriptors
 *   POLYTEST_NO_ALIASES          — do not define TEST / ASSERT_* short names
 *   POLYTEST_EXCLUDE_FLOAT       — omit float/double asserts
 *   POLYTEST_FREESTANDING        — no-stdlib; use polytest_set_writer
 *   POLYTEST_NO_LONGJMP          — PROTECT/ABORT without setjmp (abort = fail flag)
 *
 * Derived (from polytest_profile.h): POLYTEST_CFG_HAS_{COBS,TAGS,FIXTURES,FLOAT,
 * PROTECT,MUTEX,EXTENDED_ASSERTS,HEAP}.
 */
#ifndef POLYTEST_H
#define POLYTEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "polytest/polytest_profile.h"

#ifndef POLYTEST_SECTION
#if defined(__APPLE__) && defined(__MACH__)
#define POLYTEST_SECTION __attribute__((section("__DATA,polytest")))
#elif defined(__GNUC__) || defined(__clang__)
#define POLYTEST_SECTION __attribute__((section(".polytest_info")))
#else
#define POLYTEST_SECTION
#endif
#endif

typedef void (*polytest_fn_t)(void);
typedef void (*polytest_fixture_fn_t)(void);

/** Suite: fixtures run once around the whole suite. */
typedef struct polytest_suite {
    const char *name;
    polytest_fixture_fn_t setup;
    polytest_fixture_fn_t teardown;
    const char *const *tags; /* NULL-terminated; may be NULL */
    struct polytest_suite *next;
} polytest_suite_t;

/** Group: fixtures run before/after each case in the group. */
typedef struct polytest_group {
    const char *suite;
    const char *name;
    polytest_fixture_fn_t setup;
    polytest_fixture_fn_t teardown;
    const char *const *tags;
    struct polytest_group *next;
} polytest_group_t;

typedef struct polytest_case {
    const char *suite;
    const char *group;
    const char *name;
    polytest_fn_t fn;
    const char *const *tags;
    struct polytest_case *next;
} polytest_case_t;

/** Byte sink — board/transport glue provides this (Dependency Inversion). */
typedef void (*polytest_write_fn_t)(const void *data, size_t len, void *user);

void polytest_set_writer(polytest_write_fn_t fn, void *user);

#if POLYTEST_CFG_HAS_MUTEX
/** Optional lock hooks for multithreaded host (full profile). NULL = no-op. */
typedef void (*polytest_lock_fn_t)(void *user);
void polytest_set_locks(polytest_lock_fn_t lock, polytest_lock_fn_t unlock,
                        void *user);
#endif

void polytest_register_suite(polytest_suite_t *suite);
void polytest_register_group(polytest_group_t *group);
void polytest_register(polytest_case_t *test_case);

#if POLYTEST_CFG_HAS_HEAP
/**
 * Heap-allocate a case descriptor and register it.
 * Returns 0 on success, -1 on allocation failure.
 */
int polytest_register_heap_case(const char *suite, const char *group,
                                const char *name, polytest_fn_t fn);
#endif

int polytest_run_all(void);
/** Run cases whose suite, group, or case tags include `tag` (stub on tiny). */
int polytest_run_tag(const char *tag);

void polytest_ignore(const char *message);
int polytest_protect(void);
void polytest_abort(void);

void polytest_fail(const char *message, const char *file, int line);
void polytest_fail_at(const char *file, int line, const char *message);

void polytest_assert_true(int cond, const char *expr, const char *msg,
                          const char *file, int line);
void polytest_assert_false(int cond, const char *expr, const char *msg,
                           const char *file, int line);
void polytest_assert_null(const void *ptr, const char *msg, const char *file,
                          int line);
void polytest_assert_not_null(const void *ptr, const char *msg, const char *file,
                              int line);

void polytest_assert_int(int64_t expected, int64_t actual, int size,
                         int is_hex, const char *msg, const char *file,
                         int line);
void polytest_assert_uint(uint64_t expected, uint64_t actual, int size,
                          int is_hex, const char *msg, const char *file,
                          int line);
void polytest_assert_not_equal_int(int64_t expected, int64_t actual,
                                   const char *msg, const char *file, int line);
void polytest_assert_greater_than(int64_t threshold, int64_t actual,
                                  const char *msg, const char *file, int line);
void polytest_assert_less_than(int64_t threshold, int64_t actual,
                               const char *msg, const char *file, int line);
void polytest_assert_int_within(int64_t delta, int64_t expected, int64_t actual,
                                const char *msg, const char *file, int line);

#ifndef POLYTEST_EXCLUDE_FLOAT
void polytest_assert_float_within(float delta, float expected, float actual,
                                  const char *msg, const char *file, int line);
void polytest_assert_double_within(double delta, double expected, double actual,
                                   const char *msg, const char *file, int line);
#endif

#if POLYTEST_CFG_HAS_EXTENDED_ASSERTS
void polytest_assert_string(const char *expected, const char *actual,
                            const char *msg, const char *file, int line);
void polytest_assert_string_len(const char *expected, const char *actual,
                                size_t len, const char *msg, const char *file,
                                int line);
void polytest_assert_memory(const void *expected, const void *actual, size_t len,
                            const char *msg, const char *file, int line);
void polytest_assert_int_array(const int *expected, const int *actual,
                               size_t num, const char *msg, const char *file,
                               int line);
void polytest_assert_uint8_array(const uint8_t *expected, const uint8_t *actual,
                                 size_t num, const char *msg, const char *file,
                                 int line);
void polytest_assert_bits(uint32_t mask, uint32_t expected, uint32_t actual,
                          const char *msg, const char *file, int line);
void polytest_assert_bits_high(uint32_t mask, uint32_t actual, const char *msg,
                               const char *file, int line);
void polytest_assert_bits_low(uint32_t mask, uint32_t actual, const char *msg,
                              const char *file, int line);
#endif

/* -------------------------------------------------------------------------- */
/* Assert macros                                                              */
/* -------------------------------------------------------------------------- */

#define POLYTEST_ASSERT_TRUE(cond)                                             \
    polytest_assert_true(!!(cond), #cond, NULL, __FILE__, __LINE__)
#define POLYTEST_ASSERT_TRUE_MESSAGE(cond, msg)                                \
    polytest_assert_true(!!(cond), #cond, (msg), __FILE__, __LINE__)
#define POLYTEST_ASSERT_FALSE(cond)                                            \
    polytest_assert_false(!!(cond), #cond, NULL, __FILE__, __LINE__)
#define POLYTEST_ASSERT_FALSE_MESSAGE(cond, msg)                               \
    polytest_assert_false(!!(cond), #cond, (msg), __FILE__, __LINE__)

#define POLYTEST_ASSERT_NULL(ptr)                                              \
    polytest_assert_null((const void *)(ptr), NULL, __FILE__, __LINE__)
#define POLYTEST_ASSERT_NULL_MESSAGE(ptr, msg)                                 \
    polytest_assert_null((const void *)(ptr), (msg), __FILE__, __LINE__)
#define POLYTEST_ASSERT_NOT_NULL(ptr)                                          \
    polytest_assert_not_null((const void *)(ptr), NULL, __FILE__, __LINE__)
#define POLYTEST_ASSERT_NOT_NULL_MESSAGE(ptr, msg)                             \
    polytest_assert_not_null((const void *)(ptr), (msg), __FILE__, __LINE__)

#define POLYTEST_ASSERT_EQUAL_INT(expected, actual)                            \
    polytest_assert_int((int64_t)(expected), (int64_t)(actual), (int)sizeof(int), \
                        0, NULL, __FILE__, __LINE__)
#define POLYTEST_ASSERT_EQUAL_INT_MESSAGE(expected, actual, msg)               \
    polytest_assert_int((int64_t)(expected), (int64_t)(actual), (int)sizeof(int), \
                        0, (msg), __FILE__, __LINE__)
#define POLYTEST_ASSERT_EQUAL_INT8(expected, actual)                           \
    polytest_assert_int((int64_t)(int8_t)(expected), (int64_t)(int8_t)(actual), \
                        1, 0, NULL, __FILE__, __LINE__)
#define POLYTEST_ASSERT_EQUAL_INT16(expected, actual)                          \
    polytest_assert_int((int64_t)(int16_t)(expected),                          \
                        (int64_t)(int16_t)(actual), 2, 0, NULL, __FILE__,      \
                        __LINE__)
#define POLYTEST_ASSERT_EQUAL_INT32(expected, actual)                          \
    polytest_assert_int((int64_t)(int32_t)(expected),                          \
                        (int64_t)(int32_t)(actual), 4, 0, NULL, __FILE__,      \
                        __LINE__)
#define POLYTEST_ASSERT_EQUAL_INT64(expected, actual)                          \
    polytest_assert_int((int64_t)(expected), (int64_t)(actual), 8, 0, NULL,    \
                        __FILE__, __LINE__)

#define POLYTEST_ASSERT_EQUAL_UINT(expected, actual)                           \
    polytest_assert_uint((uint64_t)(expected), (uint64_t)(actual),             \
                         (int)sizeof(unsigned), 0, NULL, __FILE__, __LINE__)
#define POLYTEST_ASSERT_EQUAL_UINT8(expected, actual)                          \
    polytest_assert_uint((uint64_t)(uint8_t)(expected),                        \
                         (uint64_t)(uint8_t)(actual), 1, 0, NULL, __FILE__,    \
                         __LINE__)
#define POLYTEST_ASSERT_EQUAL_UINT16(expected, actual)                         \
    polytest_assert_uint((uint64_t)(uint16_t)(expected),                       \
                         (uint64_t)(uint16_t)(actual), 2, 0, NULL, __FILE__,   \
                         __LINE__)
#define POLYTEST_ASSERT_EQUAL_UINT32(expected, actual)                         \
    polytest_assert_uint((uint64_t)(uint32_t)(expected),                       \
                         (uint64_t)(uint32_t)(actual), 4, 0, NULL, __FILE__,   \
                         __LINE__)
#define POLYTEST_ASSERT_EQUAL_UINT64(expected, actual)                         \
    polytest_assert_uint((uint64_t)(expected), (uint64_t)(actual), 8, 0, NULL, \
                         __FILE__, __LINE__)

#define POLYTEST_ASSERT_EQUAL_HEX8(expected, actual)                           \
    polytest_assert_uint((uint64_t)(uint8_t)(expected),                        \
                         (uint64_t)(uint8_t)(actual), 1, 1, NULL, __FILE__,    \
                         __LINE__)
#define POLYTEST_ASSERT_EQUAL_HEX16(expected, actual)                          \
    polytest_assert_uint((uint64_t)(uint16_t)(expected),                       \
                         (uint64_t)(uint16_t)(actual), 2, 1, NULL, __FILE__,   \
                         __LINE__)
#define POLYTEST_ASSERT_EQUAL_HEX32(expected, actual)                          \
    polytest_assert_uint((uint64_t)(uint32_t)(expected),                       \
                         (uint64_t)(uint32_t)(actual), 4, 1, NULL, __FILE__,   \
                         __LINE__)
#define POLYTEST_ASSERT_EQUAL_HEX64(expected, actual)                          \
    polytest_assert_uint((uint64_t)(expected), (uint64_t)(actual), 8, 1, NULL, \
                         __FILE__, __LINE__)

#define POLYTEST_ASSERT_NOT_EQUAL_INT(expected, actual)                        \
    polytest_assert_not_equal_int((int64_t)(expected), (int64_t)(actual), NULL,\
                                  __FILE__, __LINE__)
#define POLYTEST_ASSERT_GREATER_THAN(threshold, actual)                        \
    polytest_assert_greater_than((int64_t)(threshold), (int64_t)(actual), NULL,\
                                 __FILE__, __LINE__)
#define POLYTEST_ASSERT_LESS_THAN(threshold, actual)                           \
    polytest_assert_less_than((int64_t)(threshold), (int64_t)(actual), NULL,   \
                              __FILE__, __LINE__)
#define POLYTEST_ASSERT_INT_WITHIN(delta, expected, actual)                    \
    polytest_assert_int_within((int64_t)(delta), (int64_t)(expected),          \
                               (int64_t)(actual), NULL, __FILE__, __LINE__)
#define POLYTEST_ASSERT_INT_WITHIN_MESSAGE(delta, expected, actual, msg)       \
    polytest_assert_int_within((int64_t)(delta), (int64_t)(expected),          \
                               (int64_t)(actual), (msg), __FILE__, __LINE__)

#ifndef POLYTEST_EXCLUDE_FLOAT
#define POLYTEST_ASSERT_FLOAT_WITHIN(delta, expected, actual)                  \
    polytest_assert_float_within((float)(delta), (float)(expected),            \
                                 (float)(actual), NULL, __FILE__, __LINE__)
#define POLYTEST_ASSERT_EQUAL_FLOAT(expected, actual)                          \
    POLYTEST_ASSERT_FLOAT_WITHIN(0.00001f, (expected), (actual))
#define POLYTEST_ASSERT_DOUBLE_WITHIN(delta, expected, actual)                 \
    polytest_assert_double_within((double)(delta), (double)(expected),         \
                                  (double)(actual), NULL, __FILE__, __LINE__)
#define POLYTEST_ASSERT_EQUAL_DOUBLE(expected, actual)                         \
    POLYTEST_ASSERT_DOUBLE_WITHIN(1.0e-12, (expected), (actual))
#endif

#if POLYTEST_CFG_HAS_EXTENDED_ASSERTS
#define POLYTEST_ASSERT_EQUAL_STRING(expected, actual)                         \
    polytest_assert_string((expected), (actual), NULL, __FILE__, __LINE__)
#define POLYTEST_ASSERT_EQUAL_STRING_MESSAGE(expected, actual, msg)            \
    polytest_assert_string((expected), (actual), (msg), __FILE__, __LINE__)
#define POLYTEST_ASSERT_EQUAL_STRING_LEN(expected, actual, len)                \
    polytest_assert_string_len((expected), (actual), (size_t)(len), NULL,      \
                               __FILE__, __LINE__)
#define POLYTEST_ASSERT_EQUAL_MEMORY(expected, actual, len)                    \
    polytest_assert_memory((expected), (actual), (size_t)(len), NULL,          \
                           __FILE__, __LINE__)
#define POLYTEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, num)                 \
    polytest_assert_int_array((expected), (actual), (size_t)(num), NULL,       \
                              __FILE__, __LINE__)
#define POLYTEST_ASSERT_EQUAL_UINT8_ARRAY(expected, actual, num)               \
    polytest_assert_uint8_array((expected), (actual), (size_t)(num), NULL,     \
                                __FILE__, __LINE__)

#define POLYTEST_ASSERT_BITS(mask, expected, actual)                           \
    polytest_assert_bits((uint32_t)(mask), (uint32_t)(expected),               \
                         (uint32_t)(actual), NULL, __FILE__, __LINE__)
#define POLYTEST_ASSERT_BITS_HIGH(mask, actual)                                \
    polytest_assert_bits_high((uint32_t)(mask), (uint32_t)(actual), NULL,      \
                              __FILE__, __LINE__)
#define POLYTEST_ASSERT_BITS_LOW(mask, actual)                                 \
    polytest_assert_bits_low((uint32_t)(mask), (uint32_t)(actual), NULL,       \
                             __FILE__, __LINE__)
#endif

#define POLYTEST_FAIL(msg) polytest_fail((msg), __FILE__, __LINE__)
#define POLYTEST_FAIL_MESSAGE(msg) POLYTEST_FAIL(msg)

#define POLYTEST_IGNORE()                                                      \
    do {                                                                       \
        polytest_ignore(NULL);                                                 \
        return;                                                                \
    } while (0)
#define POLYTEST_IGNORE_MESSAGE(msg)                                           \
    do {                                                                       \
        polytest_ignore(msg);                                                  \
        return;                                                                \
    } while (0)

#define POLYTEST_PROTECT() (polytest_protect())
#define POLYTEST_ABORT() polytest_abort()

/* -------------------------------------------------------------------------- */
/* Suite / group / case registration                                          */
/* -------------------------------------------------------------------------- */

#if POLYTEST_CFG_HAS_FIXTURES

/** Define the shared suite object (use once per suite; SETUP/TEARDOWN/TAGS attach). */
#define POLYTEST_SUITE(suite_name)                                             \
    static polytest_suite_t polytest_suite_##suite_name = {                    \
        #suite_name, NULL, NULL, NULL, NULL};                                  \
    static void polytest_suite_base_reg_##suite_name(void)                     \
        __attribute__((constructor));                                          \
    static void polytest_suite_base_reg_##suite_name(void) {                   \
        polytest_register_suite(&polytest_suite_##suite_name);                 \
    }

/**
 * Define suite + setup. Prefer this *or* POLYTEST_SUITE (not both) so there is
 * a single polytest_suite_##name object. TEARDOWN/TAGS mutate the same object.
 */
#define POLYTEST_SUITE_SETUP(suite_name)                                       \
    static void polytest_suite_setup_##suite_name(void);                       \
    static polytest_suite_t polytest_suite_##suite_name = {                    \
        #suite_name, polytest_suite_setup_##suite_name, NULL, NULL, NULL};     \
    static void polytest_suite_su_reg_##suite_name(void)                       \
        __attribute__((constructor));                                          \
    static void polytest_suite_su_reg_##suite_name(void) {                     \
        polytest_register_suite(&polytest_suite_##suite_name);                 \
    }                                                                          \
    static void polytest_suite_setup_##suite_name(void)

#define POLYTEST_SUITE_TEARDOWN(suite_name)                                    \
    static void polytest_suite_teardown_##suite_name(void);                    \
    static void polytest_suite_td_reg_##suite_name(void)                       \
        __attribute__((constructor));                                          \
    static void polytest_suite_td_reg_##suite_name(void) {                     \
        polytest_suite_##suite_name.teardown =                                 \
            polytest_suite_teardown_##suite_name;                              \
        polytest_register_suite(&polytest_suite_##suite_name);                 \
    }                                                                          \
    static void polytest_suite_teardown_##suite_name(void)

#if POLYTEST_CFG_HAS_TAGS
#define POLYTEST_SUITE_TAGS(suite_name, ...)                                   \
    static const char *const polytest_suite_tags_##suite_name[] = {            \
        __VA_ARGS__, NULL};                                                    \
    static void polytest_suite_tags_reg_##suite_name(void)                     \
        __attribute__((constructor));                                          \
    static void polytest_suite_tags_reg_##suite_name(void) {                   \
        polytest_suite_##suite_name.tags = polytest_suite_tags_##suite_name;   \
        polytest_register_suite(&polytest_suite_##suite_name);                 \
    }
#else
#define POLYTEST_SUITE_TAGS(suite_name, ...)                                   \
    enum { polytest_suite_tags_unused_##suite_name = 0 }
#endif

#define POLYTEST_GROUP_SETUP(suite_name, group_name)                           \
    static void polytest_group_setup_##suite_name##_##group_name(void);        \
    static polytest_group_t polytest_group_##suite_name##_##group_name = {     \
        #suite_name,                                                           \
        #group_name,                                                           \
        polytest_group_setup_##suite_name##_##group_name,                      \
        NULL,                                                                  \
        NULL,                                                                  \
        NULL};                                                                  \
    static void polytest_group_su_reg_##suite_name##_##group_name(void)        \
        __attribute__((constructor));                                          \
    static void polytest_group_su_reg_##suite_name##_##group_name(void) {      \
        polytest_register_group(                                               \
            &polytest_group_##suite_name##_##group_name);                      \
    }                                                                          \
    static void polytest_group_setup_##suite_name##_##group_name(void)

#define POLYTEST_GROUP_TEARDOWN(suite_name, group_name)                        \
    static void polytest_group_teardown_##suite_name##_##group_name(void);     \
    static void polytest_group_td_reg_##suite_name##_##group_name(void)        \
        __attribute__((constructor));                                          \
    static void polytest_group_td_reg_##suite_name##_##group_name(void) {      \
        polytest_group_##suite_name##_##group_name.teardown =                  \
            polytest_group_teardown_##suite_name##_##group_name;               \
        polytest_register_group(                                               \
            &polytest_group_##suite_name##_##group_name);                      \
    }                                                                          \
    static void polytest_group_teardown_##suite_name##_##group_name(void)

#if POLYTEST_CFG_HAS_TAGS
#define POLYTEST_GROUP_TAGS(suite_name, group_name, ...)                       \
    static const char *const polytest_group_tags_##suite_name##_##group_name[] \
        = {__VA_ARGS__, NULL};                                                 \
    static void polytest_group_tags_reg_##suite_name##_##group_name(void)      \
        __attribute__((constructor));                                          \
    static void polytest_group_tags_reg_##suite_name##_##group_name(void) {    \
        polytest_group_##suite_name##_##group_name.tags =                      \
            polytest_group_tags_##suite_name##_##group_name;                   \
        polytest_register_group(                                               \
            &polytest_group_##suite_name##_##group_name);                      \
    }
#else
#define POLYTEST_GROUP_TAGS(suite_name, group_name, ...)                       \
    enum { polytest_group_tags_unused_##suite_name##_##group_name = 0 }
#endif

#else /* !POLYTEST_CFG_HAS_FIXTURES — tiny: accept macros, skip registration */

#define POLYTEST_SUITE(suite_name)                                             \
    enum { polytest_suite_unused_##suite_name = 0 }
#define POLYTEST_SUITE_SETUP(suite_name)                                       \
    static void polytest_suite_setup_##suite_name(void)
#define POLYTEST_SUITE_TEARDOWN(suite_name)                                    \
    static void polytest_suite_teardown_##suite_name(void)
#define POLYTEST_SUITE_TAGS(suite_name, ...)                                   \
    enum { polytest_suite_tags_unused_##suite_name = 0 }
#define POLYTEST_GROUP_SETUP(suite_name, group_name)                           \
    static void polytest_group_setup_##suite_name##_##group_name(void)
#define POLYTEST_GROUP_TEARDOWN(suite_name, group_name)                        \
    static void polytest_group_teardown_##suite_name##_##group_name(void)
#define POLYTEST_GROUP_TAGS(suite_name, group_name, ...)                       \
    enum { polytest_group_tags_unused_##suite_name##_##group_name = 0 }

#endif /* POLYTEST_CFG_HAS_FIXTURES */

#ifdef POLYTEST_USE_SECTION_REGISTRY
#define POLYTEST_TEST(suite_name, group_name, case_name)                       \
    static void polytest_body_##suite_name##_##group_name##_##case_name(void); \
    static polytest_case_t                                                     \
        polytest_desc_##suite_name##_##group_name##_##case_name                \
            POLYTEST_SECTION = {#suite_name,                                   \
                                #group_name,                                   \
                                #case_name,                                    \
                                polytest_body_##suite_name##_##group_name##_##case_name, \
                                NULL,                                          \
                                NULL};                                          \
    static void polytest_body_##suite_name##_##group_name##_##case_name(void)
#else
#define POLYTEST_TEST(suite_name, group_name, case_name)                       \
    static void polytest_body_##suite_name##_##group_name##_##case_name(void); \
    static polytest_case_t                                                     \
        polytest_desc_##suite_name##_##group_name##_##case_name = {            \
            #suite_name,                                                       \
            #group_name,                                                       \
            #case_name,                                                        \
            polytest_body_##suite_name##_##group_name##_##case_name,           \
            NULL,                                                              \
            NULL};                                                              \
    static void polytest_reg_##suite_name##_##group_name##_##case_name(void)   \
        __attribute__((constructor));                                          \
    static void polytest_reg_##suite_name##_##group_name##_##case_name(void) { \
        polytest_register(                                                     \
            &polytest_desc_##suite_name##_##group_name##_##case_name);         \
    }                                                                          \
    static void polytest_body_##suite_name##_##group_name##_##case_name(void)
#endif

#if POLYTEST_CFG_HAS_TAGS
#define POLYTEST_TEST_TAGS(suite_name, group_name, case_name, ...)             \
    static void polytest_body_##suite_name##_##group_name##_##case_name(void); \
    static const char *const                                                   \
        polytest_case_tags_##suite_name##_##group_name##_##case_name[] = {     \
            __VA_ARGS__, NULL};                                                \
    static polytest_case_t                                                     \
        polytest_desc_##suite_name##_##group_name##_##case_name = {            \
            #suite_name,                                                       \
            #group_name,                                                       \
            #case_name,                                                        \
            polytest_body_##suite_name##_##group_name##_##case_name,           \
            polytest_case_tags_##suite_name##_##group_name##_##case_name,      \
            NULL};                                                              \
    static void polytest_reg_##suite_name##_##group_name##_##case_name(void)   \
        __attribute__((constructor));                                          \
    static void polytest_reg_##suite_name##_##group_name##_##case_name(void) { \
        polytest_register(                                                     \
            &polytest_desc_##suite_name##_##group_name##_##case_name);         \
    }                                                                          \
    static void polytest_body_##suite_name##_##group_name##_##case_name(void)
#else
#define POLYTEST_TEST_TAGS(suite_name, group_name, case_name, ...)             \
    POLYTEST_TEST(suite_name, group_name, case_name)
#endif

/**
 * Parameterized helper — invoke body once per table row from inside a TEST.
 */
#define POLYTEST_FOR_EACH(type, var, array)                                    \
    for (size_t _pt_i = 0;                                                     \
         _pt_i < sizeof(array) / sizeof((array)[0]); ++_pt_i)                  \
        for (type var = (array)[_pt_i], *_pt_once = &var; _pt_once;             \
             _pt_once = NULL)

#ifndef POLYTEST_NO_ALIASES
#define TEST POLYTEST_TEST
#define TEST_TAGS POLYTEST_TEST_TAGS
#define ASSERT_TRUE POLYTEST_ASSERT_TRUE
#define ASSERT_TRUE_MESSAGE POLYTEST_ASSERT_TRUE_MESSAGE
#define ASSERT_FALSE POLYTEST_ASSERT_FALSE
#define ASSERT_FALSE_MESSAGE POLYTEST_ASSERT_FALSE_MESSAGE
#define ASSERT_EQ POLYTEST_ASSERT_EQUAL_INT
#define ASSERT_NE POLYTEST_ASSERT_NOT_EQUAL_INT
#define ASSERT_EQUAL_INT POLYTEST_ASSERT_EQUAL_INT
#define ASSERT_EQUAL_UINT POLYTEST_ASSERT_EQUAL_UINT
#define ASSERT_EQUAL_HEX32 POLYTEST_ASSERT_EQUAL_HEX32
#define ASSERT_NULL POLYTEST_ASSERT_NULL
#define ASSERT_NOT_NULL POLYTEST_ASSERT_NOT_NULL
#if POLYTEST_CFG_HAS_EXTENDED_ASSERTS
#define ASSERT_BITS POLYTEST_ASSERT_BITS
#define ASSERT_BITS_HIGH POLYTEST_ASSERT_BITS_HIGH
#define ASSERT_BITS_LOW POLYTEST_ASSERT_BITS_LOW
#define ASSERT_EQUAL_STRING POLYTEST_ASSERT_EQUAL_STRING
#define ASSERT_EQUAL_MEMORY POLYTEST_ASSERT_EQUAL_MEMORY
#endif
#define ASSERT_GREATER_THAN POLYTEST_ASSERT_GREATER_THAN
#define ASSERT_LESS_THAN POLYTEST_ASSERT_LESS_THAN
#define ASSERT_INT_WITHIN POLYTEST_ASSERT_INT_WITHIN
#define FAIL POLYTEST_FAIL
#define IGNORE POLYTEST_IGNORE
#define IGNORE_MESSAGE POLYTEST_IGNORE_MESSAGE
#define TEST_PROTECT POLYTEST_PROTECT
#define TEST_ABORT POLYTEST_ABORT
#ifndef POLYTEST_EXCLUDE_FLOAT
#define ASSERT_FLOAT_WITHIN POLYTEST_ASSERT_FLOAT_WITHIN
#define ASSERT_EQUAL_FLOAT POLYTEST_ASSERT_EQUAL_FLOAT
#define ASSERT_DOUBLE_WITHIN POLYTEST_ASSERT_DOUBLE_WITHIN
#define ASSERT_EQUAL_DOUBLE POLYTEST_ASSERT_EQUAL_DOUBLE
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* POLYTEST_H */
