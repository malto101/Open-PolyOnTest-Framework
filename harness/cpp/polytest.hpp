#ifndef POLYTEST_HPP
#define POLYTEST_HPP

/**
 * Thin C++ sugar over the PolyTest C ABI.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "polytest/polytest.h"

namespace polytest {

inline int run_all() { return ::polytest_run_all(); }

inline int run_tag(const char *tag) { return ::polytest_run_tag(tag); }

inline void set_writer(polytest_write_fn_t fn, void *user) {
  ::polytest_set_writer(fn, user);
}

} // namespace polytest

#endif
