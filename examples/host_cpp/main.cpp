#include "polytest.hpp"

#include <cstdio>

TEST(CppSugar, Smoke, Equals) {
    ASSERT_EQ(4, 2 + 2);
}

int main() {
    std::printf("polytest C++ adapter smoke\n");
    return polytest::run_all();
}
