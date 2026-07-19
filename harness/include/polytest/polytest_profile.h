/**
 * POT compile-time size profiles
 * Copyright 2026 Dhruv Menon
 * SPDX-License-Identifier: Apache-2.0
 *
 * Select one of:
 *   POT_PROFILE_TINY   — ~1–3 KB: text only, no tags/fixtures/float/longjmp
 *   POT_PROFILE_SMALL  — hierarchy + tags + fixtures + COBS; no float by default
 *   POT_PROFILE_FULL   — everything (default when no profile define is set)
 *
 * Or leave unset for FULL. Explicit feature knobs (POT_MINIMAL_PRINT,
 * POT_EXCLUDE_FLOAT, …) still override after the profile is applied.
 *
 * Derived feature macros (1 = enabled):
 *   POT_CFG_HAS_COBS
 *   POT_CFG_HAS_TAGS
 *   POT_CFG_HAS_FIXTURES
 *   POT_CFG_HAS_FLOAT
 *   POT_CFG_HAS_PROTECT
 *   POT_CFG_HAS_MUTEX
 *   POT_CFG_HAS_EXTENDED_ASSERTS
 *   POT_CFG_HAS_HEAP
 */
#ifndef POT_PROFILE_H
#define POT_PROFILE_H

/* Resolve which profile is active (exactly one). */
#if defined(POT_PROFILE_TINY)
#define POT_PROFILE_ACTIVE_TINY 1
#elif defined(POT_PROFILE_SMALL)
#define POT_PROFILE_ACTIVE_SMALL 1
#elif defined(POT_PROFILE_FULL)
#define POT_PROFILE_ACTIVE_FULL 1
#else
#define POT_PROFILE_ACTIVE_FULL 1
#endif

/* -------------------------------------------------------------------------- */
/* tiny                                                                        */
/* -------------------------------------------------------------------------- */
#if defined(POT_PROFILE_ACTIVE_TINY)

#ifndef POT_MINIMAL_PRINT
#define POT_MINIMAL_PRINT
#endif
#ifndef POT_EXCLUDE_FLOAT
#define POT_EXCLUDE_FLOAT
#endif
#ifndef POT_NO_LONGJMP
#define POT_NO_LONGJMP
#endif

#define POT_CFG_HAS_COBS 0
#define POT_CFG_HAS_TAGS 0
#define POT_CFG_HAS_FIXTURES 0
#define POT_CFG_HAS_FLOAT 0
#define POT_CFG_HAS_PROTECT 0
#define POT_CFG_HAS_MUTEX 0
#define POT_CFG_HAS_EXTENDED_ASSERTS 0

/* -------------------------------------------------------------------------- */
/* small                                                                       */
/* -------------------------------------------------------------------------- */
#elif defined(POT_PROFILE_ACTIVE_SMALL)

#ifndef POT_EXCLUDE_FLOAT
#define POT_EXCLUDE_FLOAT
#endif

#if defined(POT_MINIMAL_PRINT)
#define POT_CFG_HAS_COBS 0
#else
#define POT_CFG_HAS_COBS 1
#endif
#define POT_CFG_HAS_TAGS 1
#define POT_CFG_HAS_FIXTURES 1
#if defined(POT_EXCLUDE_FLOAT)
#define POT_CFG_HAS_FLOAT 0
#else
#define POT_CFG_HAS_FLOAT 1
#endif
#if defined(POT_NO_LONGJMP)
#define POT_CFG_HAS_PROTECT 0
#else
#define POT_CFG_HAS_PROTECT 1
#endif
#define POT_CFG_HAS_MUTEX 0
#define POT_CFG_HAS_EXTENDED_ASSERTS 1

/* -------------------------------------------------------------------------- */
/* full (default)                                                              */
/* -------------------------------------------------------------------------- */
#else /* POT_PROFILE_ACTIVE_FULL */

#if defined(POT_MINIMAL_PRINT)
#define POT_CFG_HAS_COBS 0
#else
#define POT_CFG_HAS_COBS 1
#endif
#define POT_CFG_HAS_TAGS 1
#define POT_CFG_HAS_FIXTURES 1
#if defined(POT_EXCLUDE_FLOAT)
#define POT_CFG_HAS_FLOAT 0
#else
#define POT_CFG_HAS_FLOAT 1
#endif
#if defined(POT_NO_LONGJMP)
#define POT_CFG_HAS_PROTECT 0
#else
#define POT_CFG_HAS_PROTECT 1
#endif
#define POT_CFG_HAS_MUTEX 1
#define POT_CFG_HAS_EXTENDED_ASSERTS 1

#endif /* profiles */

/* Heap registration is orthogonal to size profiles. */
#if defined(POT_USE_HEAP)
#define POT_CFG_HAS_HEAP 1
#else
#define POT_CFG_HAS_HEAP 0
#endif

/* Sync float exclusion with CFG when profile already stripped floats. */
#if !POT_CFG_HAS_FLOAT && !defined(POT_EXCLUDE_FLOAT)
#define POT_EXCLUDE_FLOAT
#endif

/* Sync longjmp strip when protect is disabled. */
#if !POT_CFG_HAS_PROTECT && !defined(POT_NO_LONGJMP)
#define POT_NO_LONGJMP
#endif

/* Tiny always forces text path even if user forgot MINIMAL_PRINT. */
#if !POT_CFG_HAS_COBS && !defined(POT_MINIMAL_PRINT)
#define POT_MINIMAL_PRINT
#endif

#endif /* POT_PROFILE_H */
