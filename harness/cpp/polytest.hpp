#ifndef POLYTEST_HPP
#define POLYTEST_HPP

/**
 * Thin C++ sugar over the PolyTest C ABI.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Keep TEST / ASSERT_* as the C macros (embedded-normal). This header wraps
 * runners and optional lock hooks.
 */

#include "polytest/polytest.h"

namespace polytest {

inline int run_all() { return ::polytest_run_all(); }

inline int run_tag(const char *tag) { return ::polytest_run_tag(tag); }

inline int run_suite(const char *suite) { return ::polytest_run_suite(suite); }

inline int run_group(const char *suite, const char *group) {
  return ::polytest_run_group(suite, group);
}

/** Honor POLYTEST_TAG / POLYTEST_SUITE / POLYTEST_GROUP from the environment. */
inline int run_from_env() { return ::polytest_run_from_env(); }

inline void set_writer(polytest_write_fn_t fn, void *user) {
  ::polytest_set_writer(fn, user);
}

#if POLYTEST_CFG_HAS_MUTEX
inline void set_locks(polytest_lock_fn_t lock, polytest_lock_fn_t unlock,
                      void *user) {
  ::polytest_set_locks(lock, unlock, user);
}
#endif

} // namespace polytest

#endif
