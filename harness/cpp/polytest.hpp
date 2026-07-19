#ifndef POT_HPP
#define POT_HPP

/**
 * Thin C++ sugar over the POT C ABI.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Keep TEST / ASSERT_* as the C macros (embedded-normal). This header wraps
 * runners and optional lock hooks.
 */

#include "polytest/polytest.h"

namespace polytest {

inline int run_all() { return ::pot_run_all(); }

inline int run_tag(const char *tag) { return ::pot_run_tag(tag); }

inline int run_suite(const char *suite) { return ::pot_run_suite(suite); }

inline int run_group(const char *suite, const char *group) {
  return ::pot_run_group(suite, group);
}

/** Honor POT_TAG / POT_SUITE / POT_GROUP from the environment. */
inline int run_from_env() { return ::pot_run_from_env(); }

inline void set_writer(pot_write_fn_t fn, void *user) {
  ::pot_set_writer(fn, user);
}

#if POT_CFG_HAS_MUTEX
inline void set_locks(pot_lock_fn_t lock, pot_lock_fn_t unlock,
                      void *user) {
  ::pot_set_locks(lock, unlock, user);
}
#endif

} // namespace polytest

#endif
