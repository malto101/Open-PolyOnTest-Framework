/**
 * C test cases linked into the host_rust example binary.
 *
 * `pot_host_rust_link_anchor` forces the object file into the final link
 * so constructor-based TEST registration is not dead-stripped from the archive.
 */
#include "polytest/polytest.h"

int pot_host_rust_link_anchor(void) { return 0; }

TEST(RustHost, Basic, Add) { ASSERT_EQ(4, 2 + 2); }

TEST_TAGS(RustHost, Basic, Tagged, "rust", "unit") { ASSERT_EQ(7, 3 + 4); }
