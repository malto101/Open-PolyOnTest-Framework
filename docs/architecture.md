# PolyTest architecture

## Dependency rule

Outer plugins depend inward. **Core never imports** a concrete UART, USB, board, or reporter.

```
polytest-cli  (composition root)
    → polytest-plugin-api (traits)
        → polytest-protocol (events)
polytest-builtins implements the traits
harness/c is the on-target domain (C11, no_std-friendly)
```

## SOLID

| Principle | Application |
|-----------|-------------|
| S | One plugin kind per concern |
| O | Add HCI/nanopb/Pico as plugins without editing Core |
| L | Any `Transport` / `Codec` is interchangeable |
| I | Separate traits — not one mega-plugin |
| D | CLI depends on traits; `polytest.toml` selects impls |

## On-target vs host

| Side | Mechanism |
|------|-----------|
| Host | Rust traits + in-tree builtins |
| Target | Compile-time hooks (`polytest_set_writer`, `POLYTEST_SECTION`) — no dlopen |

## PTWP

Structured results use COBS-framed PTWP payloads (`codec.cobs`). Hobbyists can use `POLYTEST_MINIMAL_PRINT` / `codec.text` instead.

## Size profiles

Compile-time profiles (`POLYTEST_PROFILE_TINY` / `SMALL` / `FULL`) map to
`POLYTEST_CFG_HAS_*` feature macros. See [profiles.md](profiles.md).

## Section registry

Optional `POLYTEST_USE_SECTION_REGISTRY` + GNU ld `__start_polytest_info` walk;
ctor registration remains the default. Linker snippet in [profiles.md](profiles.md).
