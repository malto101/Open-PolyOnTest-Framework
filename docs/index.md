# PolyTest

Embedded-first unit and on-target tests for C, C++, and Rust — from a two-file
MCU drop-in to host CI — without a vendor RTOS or build-system lock-in.

## Start here

- [Quickstart](quickstart.md) — hobby printf, host CLI, QEMU
- [Architecture](architecture.md) — SOLID plugins + progressive enhancement
- [Profiles](profiles.md) — tiny / small / full size knobs
- [CLI](cli.md) — `polytest run`, filters, toml schema
- [Tags](tags.md) — suite / group / case tags and filtering
- [Mocking](mocking.md) — FFF fakes
- [C++](cpp.md) / [Rust](rust.md) — language adapters

## Progressive enhancement

| Workflow | Status |
|----------|--------|
| Auto-register + Core stream | v0.1 |
| Size profiles + FFF mocks | v0.1 |
| Command mode / Pico / HIL | v2+ |
