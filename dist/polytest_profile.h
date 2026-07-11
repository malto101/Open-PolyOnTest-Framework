/* Copied from harness — also inlined into polytest.h. */
/**
 * PolyTest compile-time size profiles
 * Copyright 2026 Dhruv Menon
 * SPDX-License-Identifier: Apache-2.0
 *
 * Select one of:
 *   POLYTEST_PROFILE_TINY   — ~1–3 KB: text only, no tags/fixtures/float/longjmp
 *   POLYTEST_PROFILE_SMALL  — hierarchy + tags + fixtures + COBS; no float by default
 *   POLYTEST_PROFILE_FULL   — everything (default when no profile define is set)
 *
 * Or leave unset for FULL. Explicit feature knobs (POLYTEST_MINIMAL_PRINT,
 * POLYTEST_EXCLUDE_FLOAT, …) still override after the profile is applied.
 *
 * Derived feature macros (1 = enabled):
 *   POLYTEST_CFG_HAS_COBS
 *   POLYTEST_CFG_HAS_TAGS
 *   POLYTEST_CFG_HAS_FIXTURES
 *   POLYTEST_CFG_HAS_FLOAT
 *   POLYTEST_CFG_HAS_PROTECT
 *   POLYTEST_CFG_HAS_MUTEX
 *   POLYTEST_CFG_HAS_EXTENDED_ASSERTS
 *   POLYTEST_CFG_HAS_HEAP
 */
#ifndef POLYTEST_PROFILE_H
#define POLYTEST_PROFILE_H

/* Resolve which profile is active (exactly one). */
#if defined(POLYTEST_PROFILE_TINY)
#define POLYTEST_PROFILE_ACTIVE_TINY 1
#elif defined(POLYTEST_PROFILE_SMALL)
#define POLYTEST_PROFILE_ACTIVE_SMALL 1
#elif defined(POLYTEST_PROFILE_FULL)
#define POLYTEST_PROFILE_ACTIVE_FULL 1
#else
#define POLYTEST_PROFILE_ACTIVE_FULL 1
#endif

/* -------------------------------------------------------------------------- */
/* tiny                                                                        */
/* -------------------------------------------------------------------------- */
#if defined(POLYTEST_PROFILE_ACTIVE_TINY)

#ifndef POLYTEST_MINIMAL_PRINT
#define POLYTEST_MINIMAL_PRINT
#endif
#ifndef POLYTEST_EXCLUDE_FLOAT
#define POLYTEST_EXCLUDE_FLOAT
#endif
#ifndef POLYTEST_NO_LONGJMP
#define POLYTEST_NO_LONGJMP
#endif

#define POLYTEST_CFG_HAS_COBS 0
#define POLYTEST_CFG_HAS_TAGS 0
#define POLYTEST_CFG_HAS_FIXTURES 0
#define POLYTEST_CFG_HAS_FLOAT 0
#define POLYTEST_CFG_HAS_PROTECT 0
#define POLYTEST_CFG_HAS_MUTEX 0
#define POLYTEST_CFG_HAS_EXTENDED_ASSERTS 0

/* -------------------------------------------------------------------------- */
/* small                                                                       */
/* -------------------------------------------------------------------------- */
#elif defined(POLYTEST_PROFILE_ACTIVE_SMALL)

#ifndef POLYTEST_EXCLUDE_FLOAT
#define POLYTEST_EXCLUDE_FLOAT
#endif

#if defined(POLYTEST_MINIMAL_PRINT)
#define POLYTEST_CFG_HAS_COBS 0
#else
#define POLYTEST_CFG_HAS_COBS 1
#endif
#define POLYTEST_CFG_HAS_TAGS 1
#define POLYTEST_CFG_HAS_FIXTURES 1
#if defined(POLYTEST_EXCLUDE_FLOAT)
#define POLYTEST_CFG_HAS_FLOAT 0
#else
#define POLYTEST_CFG_HAS_FLOAT 1
#endif
#if defined(POLYTEST_NO_LONGJMP)
#define POLYTEST_CFG_HAS_PROTECT 0
#else
#define POLYTEST_CFG_HAS_PROTECT 1
#endif
#define POLYTEST_CFG_HAS_MUTEX 0
#define POLYTEST_CFG_HAS_EXTENDED_ASSERTS 1

/* -------------------------------------------------------------------------- */
/* full (default)                                                              */
/* -------------------------------------------------------------------------- */
#else /* POLYTEST_PROFILE_ACTIVE_FULL */

#if defined(POLYTEST_MINIMAL_PRINT)
#define POLYTEST_CFG_HAS_COBS 0
#else
#define POLYTEST_CFG_HAS_COBS 1
#endif
#define POLYTEST_CFG_HAS_TAGS 1
#define POLYTEST_CFG_HAS_FIXTURES 1
#if defined(POLYTEST_EXCLUDE_FLOAT)
#define POLYTEST_CFG_HAS_FLOAT 0
#else
#define POLYTEST_CFG_HAS_FLOAT 1
#endif
#if defined(POLYTEST_NO_LONGJMP)
#define POLYTEST_CFG_HAS_PROTECT 0
#else
#define POLYTEST_CFG_HAS_PROTECT 1
#endif
#define POLYTEST_CFG_HAS_MUTEX 1
#define POLYTEST_CFG_HAS_EXTENDED_ASSERTS 1

#endif /* profiles */

/* Heap registration is orthogonal to size profiles. */
#if defined(POLYTEST_USE_HEAP)
#define POLYTEST_CFG_HAS_HEAP 1
#else
#define POLYTEST_CFG_HAS_HEAP 0
#endif

/* Sync float exclusion with CFG when profile already stripped floats. */
#if !POLYTEST_CFG_HAS_FLOAT && !defined(POLYTEST_EXCLUDE_FLOAT)
#define POLYTEST_EXCLUDE_FLOAT
#endif

/* Sync longjmp strip when protect is disabled. */
#if !POLYTEST_CFG_HAS_PROTECT && !defined(POLYTEST_NO_LONGJMP)
#define POLYTEST_NO_LONGJMP
#endif

/* Tiny always forces text path even if user forgot MINIMAL_PRINT. */
#if !POLYTEST_CFG_HAS_COBS && !defined(POLYTEST_MINIMAL_PRINT)
#define POLYTEST_MINIMAL_PRINT
#endif

#endif /* POLYTEST_PROFILE_H */
