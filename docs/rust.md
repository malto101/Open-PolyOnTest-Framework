# Rust adapter (`polytest-rs`)

The [`polytest-rs`](../crates/polytest-rs) crate is a thin FFI-facing adapter.
Default is `#![no_std]`; enable the `std` feature for host helpers.

## Link the C harness

Build/link `polytest_core.c` + `polytest_assert.c` (or `dist/polytest.c`) into
your Rust binary or a `cc` build script, then call:

```rust
extern "C" {
    fn polytest_run_all() -> i32;
}

fn main() {
    let code = unsafe { polytest_run_all() };
    std::process::exit(code);
}
```

Register cases from C (ctors / `POLYTEST_TEST`) or via
`polytest_register` / `polytest_register_heap_case` if you expose those symbols.

`polytest-rs` re-exports the FFI declarations behind the `ffi` module when the
crate is linked:

```rust
use polytest_rs::ffi::polytest_run_all;
```

## Features

| Feature | Default | Meaning |
|---------|---------|---------|
| (none) | `no_std` | Version marker + FFI signatures only |
| `std` | off | Enables `std` for host binaries |

A `#[polytest::test]` proc-macro is **future work** and not required for v1.

## Example sketch

See comments in `crates/polytest-rs/src/lib.rs`. A full `examples/host_rust`
binary can wrap the same pattern once a `build.rs` compiles the amalgam.
