# Quickstart

## Hobby / MCU (no CLI)

1. `python3 scripts/amalgamate.py`
2. Copy `dist/polytest.h` and `dist/polytest.c` into your project
3. `#define POLYTEST_PROFILE_TINY` (or `SMALL` / `FULL`) and optionally `POLYTEST_MINIMAL_PRINT`
4. Write `TEST` / `ASSERT_*` cases and call `polytest_run_all()` (or `polytest_run_from_env()` on host)
5. Read PASS/FAIL on serial or stdout

See [profiles.md](profiles.md) for size trade-offs.

### Parameterized cases (small/full)

```c
typedef struct { int a, b, sum; } row_t;
static const row_t k_rows[] = {
    {1, 1, 2},
    {2, 3, 5},
};

PARAM_TEST(Math, Basic, AddTable, row_t, k_rows) {
    const row_t row = PARAM_AS(row_t);
    ASSERT_EQ(row.sum, row.a + row.b);
}
```

Failures append `[param=<index>]`. Inside a normal `TEST`, `FOR_EACH(type, var, array)` also sets the param cursor.

## Host + CLI

```bash
cmake -S examples/host_c -B build/host_c -DPOLYTEST_MINIMAL_PRINT=OFF -DPOLYTEST_PROFILE=full
cmake --build build/host_c
cargo run -p open-polytest -- run --target host --config examples/host_c/polytest.toml
```

Produces `report.xml` and `report.json` plus a console summary.

Filter by tag (host):

```bash
cargo run -p open-polytest -- run --target host \
  --config examples/host_c/polytest.toml --tag smoke
```

Tiny host smoke:

```bash
cmake -S examples/host_c -B build/host_tiny -DPOLYTEST_PROFILE=tiny -DPOLYTEST_MINIMAL_PRINT=ON
cmake --build build/host_tiny && ./build/host_tiny/host_c_tests
```

## C++ / Rust

- C++: [cpp.md](cpp.md) — `examples/host_cpp`
- Rust: [rust.md](rust.md) — `examples/host_rust`

## Mocking (FFF)

```bash
cmake -S examples/host_fff -B build/host_fff
cmake --build build/host_fff && ./build/host_fff/host_fff_tests
```

See [mocking.md](mocking.md).

## QEMU Cortex-M33

```bash
cargo run -p open-polytest -- run --target qemu_m33 \
  --config examples/qemu_m33_smoke/polytest.toml
```

Requires `arm-none-eabi-gcc` and `qemu-system-arm`. Build with
`-DPOLYTEST_PROFILE=tiny` or `small` to compare firmware size.

Typical loop: edit → fast host check → QEMU in CI → (later) desk hardware.
