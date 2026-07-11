/**
 * Host C smoke tests — suite/group fixtures, tags, asserts, IGNORE, PROTECT.
 */
#include "polytest/polytest.h"

#include <stdlib.h>

static int add(int a, int b) { return a + b; }

static int g_suite_ready;
static int g_group_value;

POLYTEST_SUITE_SETUP(Math) { g_suite_ready = 1; }
POLYTEST_SUITE_TEARDOWN(Math) { g_suite_ready = 0; }
POLYTEST_SUITE_TAGS(Math, "host", "smoke");

POLYTEST_GROUP_SETUP(Math, Basic) { g_group_value = 40; }
POLYTEST_GROUP_TEARDOWN(Math, Basic) { g_group_value = 0; }
POLYTEST_GROUP_TAGS(Math, Basic, "unit");

TEST(Math, Basic, AddPositive) {
    ASSERT_EQ(5, add(2, 3));
#if POLYTEST_CFG_HAS_FIXTURES
    ASSERT_TRUE(g_suite_ready);
#endif
}

TEST(Math, Basic, AddZero) {
    ASSERT_EQ(2, add(2, 0));
}

TEST(Math, Basic, UsesGroupSetup) {
#if POLYTEST_CFG_HAS_FIXTURES
    ASSERT_EQ(42, g_group_value + 2);
#else
    ASSERT_EQ(2, add(1, 1));
#endif
}

TEST(Math, Basic, TypedAndBits) {
    ASSERT_EQUAL_HEX32(0xA5u, 0xA5u);
#if POLYTEST_CFG_HAS_EXTENDED_ASSERTS
    ASSERT_BITS(0x0Fu, 0x05u, 0x15u);
    ASSERT_BITS_HIGH(0x01u, 0xF1u);
    ASSERT_BITS_LOW(0x02u, 0xF1u);
#endif
    ASSERT_GREATER_THAN(3, 10);
    ASSERT_INT_WITHIN(2, 10, 11);
#if POLYTEST_CFG_HAS_EXTENDED_ASSERTS
    ASSERT_EQUAL_STRING("hi", "hi");
#endif
#ifndef POLYTEST_EXCLUDE_FLOAT
    ASSERT_EQUAL_FLOAT(1.0f, 1.0f);
#endif
}

TEST_TAGS(Math, Basic, SkipMe, "skipdemo") {
    IGNORE_MESSAGE("demonstrating IGNORE");
    ASSERT_TRUE(0);
}

TEST(Math, Basic, ProtectRegion) {
    int entered = 0;
    if (TEST_PROTECT()) {
        entered = 1;
        /* TEST_ABORT() would longjmp here and mark the case failed. */
    }
    ASSERT_TRUE(entered);
}

TEST(Expect, Pointers, NotNull) {
    int x = 1;
    ASSERT_NOT_NULL(&x);
}

int main(void) {
    return polytest_run_all();
}
