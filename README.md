# PolyTest

**PolyTest** is an open-source, Apache-2.0 embedded-first test framework with a
SOLID plugin architecture. It targets hobby MCUs, RTOS apps, kernels, and host
libraries (C / C++ / Rust) without locking you into Zephyr or Pigweed.

> Not affiliated with the unrelated crates.io crate [`polytest`](https://crates.io/crates/polytest).
> This project's CLI crate is published conceptually as **`open-polytest`** (binary: `polytest`).

## Progressive enhancement

| Workflow | Name | Audience | Status |
|----------|------|----------|--------|
| Auto-register | Linker/ctor test discovery | Everyone | **v0.1** |
| Core stream | Boot → run → stream results | Hobby / MCU / host | **v0.1** |
| Size profiles | tiny / small / full | MCU flash budgets | **v0.1** |
| FFF mocks | Header-only fakes | Host unit tests | **v0.1** |
| Command | Boot → listen → host RPC | HIL / RTOS | v2 |
| HIL conductor | Main DUT + aux stimulator | Multi-board HIL | v2.x |

## Quick start (two-file drop-in)

```bash
python3 scripts/amalgamate.py
# copy dist/polytest.h dist/polytest.c into your project
```

```c
#define POLYTEST_PROFILE_TINY
#define POLYTEST_MINIMAL_PRINT
#include "polytest.h"

TEST(Math, Basic, Add) { ASSERT_EQ(4, 2 + 2); }

int main(void) { return polytest_run_all(); }
```

Compile `polytest.c` with your existing Makefile/CMake — no PolyTest build system required.

## Size profiles

| Profile | Define | Use when |
|---------|--------|----------|
| tiny | `POLYTEST_PROFILE_TINY` | Tight MCU (~1–3 KB); text only |
| small | `POLYTEST_PROFILE_SMALL` | Tags + fixtures + COBS; no float by default |
| full | `POLYTEST_PROFILE_FULL` / default | Host CI, floats, mutex hooks |

Details: [docs/profiles.md](docs/profiles.md).

## Host example + CLI

```bash
# Build smoke tests (human-readable)
cmake -S examples/host_c -B build/host_c -DPOLYTEST_PROFILE=full
cmake --build build/host_c
./build/host_c/host_c_tests

# Structured COBS + CLI reporters (optional --tag / --suite / --group)
cmake -S examples/host_c -B build/host_c -DPOLYTEST_MINIMAL_PRINT=OFF -DPOLYTEST_PROFILE=full
cmake --build build/host_c
cargo run -p open-polytest -- run --target host --config examples/host_c/polytest.toml
cargo run -p open-polytest -- run --target host --config examples/host_c/polytest.toml --tag smoke
```

Parameterized tables use `PARAM_TEST` / `PARAM_AS` (small/full). See [docs/quickstart.md](docs/quickstart.md).

## Mocking (FFF)

```bash
cmake -S examples/host_fff -B build/host_fff && cmake --build build/host_fff
./build/host_fff/host_fff_tests
```

See [docs/mocking.md](docs/mocking.md).

## QEMU Cortex-M33 (on-target v1)

```bash
# Needs arm-none-eabi-gcc + qemu-system-arm
cargo run -p open-polytest -- run --target qemu_m33 \
  --config examples/qemu_m33_smoke/polytest.toml
```

See [examples/qemu_m33_smoke/README.md](examples/qemu_m33_smoke/README.md).

## Plugin architecture

Compose via `polytest.toml`:

- **Transport** — `stdio`, `uart` (v1); `usb_cdc`, `hci` (v2)
- **Codec** — `cobs`, `text` (v1); `nanopb` (v2)
- **Board** — `host`, `qemu_m33` (v1); `pico2w` (v2)
- **Reporter** — `console`, `junit`, `json`
- **Extension** — `core_stream` (v1); `command`, `hil_conductor` (later)

See [docs/architecture.md](docs/architecture.md) and [docs/plugins.md](docs/plugins.md).

## License

Apache-2.0 — see [LICENSE](LICENSE) and [NOTICE](NOTICE).
