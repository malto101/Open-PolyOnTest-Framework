# Quickstart

## Hobby / MCU (no CLI)

1. `python3 scripts/amalgamate.py`
2. Copy `dist/polytest.h` and `dist/polytest.c` into your project
3. `#define POLYTEST_PROFILE_TINY` (or `SMALL` / `FULL`) and optionally `POLYTEST_MINIMAL_PRINT`
4. Write `TEST` / `ASSERT_*` cases and call `polytest_run_all()`
5. Read PASS/FAIL on serial or stdout

See [profiles.md](profiles.md) for size trade-offs.

## Host + CLI

```bash
cmake -S examples/host_c -B build/host_c -DPOLYTEST_MINIMAL_PRINT=OFF -DPOLYTEST_PROFILE=full
cmake --build build/host_c
cargo run -p open-polytest -- run --target host --config examples/host_c/polytest.toml
```

Produces `report.xml` and `report.json` plus a console summary.

Tiny host smoke:

```bash
cmake -S examples/host_c -B build/host_tiny -DPOLYTEST_PROFILE=tiny -DPOLYTEST_MINIMAL_PRINT=ON
cmake --build build/host_tiny && ./build/host_tiny/host_c_tests
```

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
