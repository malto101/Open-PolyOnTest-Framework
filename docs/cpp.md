# C++ adapter

PolyTest’s C++ surface is a thin wrapper over the C ABI. Keep writing
`TEST` / `ASSERT_*` with the C macros; call runners through `namespace polytest`.

## Include

```cpp
#include "polytest.hpp"   // finds harness/cpp + polytest/polytest.h
```

## Runners

```cpp
polytest::run_all();
polytest::run_tag("smoke");
polytest::run_suite("Math");
polytest::run_group("Math", "Basic");
polytest::run_from_env();  // POLYTEST_TAG / SUITE / GROUP
```

On the full profile, `polytest::set_locks(...)` forwards to `polytest_set_locks`.

## Example

```bash
cmake -S examples/host_cpp -B build/host_cpp -DPOLYTEST_PROFILE=full
cmake --build build/host_cpp
./build/host_cpp/host_cpp_tests

# Tag filter via env (or CLI — see cli.md)
POLYTEST_TAG=unit ./build/host_cpp/host_cpp_tests
```

`PARAM_TEST` works the same as in C when using the small/full profile.
