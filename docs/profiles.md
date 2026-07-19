# Size profiles

PolyOnTest Core can strip features at compile time so the same harness fits a
hobby MCU (tiny) or a host CI binary (full).

## Choosing a profile

```mermaid
flowchart TD
  start([Pick a profile]) --> flash{Flash / RAM tight?}
  flash -->|Very tight ~1–3 KB| tiny[POT_PROFILE_TINY]
  flash -->|Moderate MCU| small[POT_PROFILE_SMALL]
  flash -->|Host CI or roomy target| full[POT_PROFILE_FULL]
  tiny --> textOnly[Text path only — no tags / fixtures / float]
  small --> knobsSmall{Need float asserts?}
  knobsSmall -->|No| smallDefault[Float off by default]
  knobsSmall -->|Yes| enableFloat[Do not set EXCLUDE_FLOAT carefully]
  full --> knobsFull{Multithreaded host?}
  knobsFull -->|Yes| mutex[pot_set_locks]
  knobsFull -->|No| fullDefault[Default full features]
  textOnly --> ortho[Orthogonal knobs below]
  smallDefault --> ortho
  enableFloat --> ortho
  mutex --> ortho
  fullDefault --> ortho
```

| Profile | Define | Typical size | Features |
|---------|--------|--------------|----------|
| **tiny** | `POT_PROFILE_TINY` | ~1–3 KB text | Text output only; no tags; no suite/group fixtures; no float; no longjmp |
| **small** | `POT_PROFILE_SMALL` | mid | Hierarchy + tags + fixtures + COBS (unless `POT_MINIMAL_PRINT`); float off by default; protect/abort OK |
| **full** | `POT_PROFILE_FULL` or unset | largest | Floats (unless `POT_EXCLUDE_FLOAT`), tags, hierarchy, COBS, protect, optional mutex hooks |

Derived macros (from `polytest_profile.h`):

- `POT_CFG_HAS_COBS`
- `POT_CFG_HAS_TAGS`
- `POT_CFG_HAS_FIXTURES`
- `POT_CFG_HAS_FLOAT`
- `POT_CFG_HAS_PROTECT`
- `POT_CFG_HAS_MUTEX`
- `POT_CFG_HAS_EXTENDED_ASSERTS` (string/memory/bits/arrays; off in tiny)
- `POT_CFG_HAS_HEAP` (when `POT_USE_HEAP`)

## CMake

```bash
cmake -S examples/host_c -B build/host_tiny \
  -DPOT_PROFILE=tiny -DPOT_MINIMAL_PRINT=ON
cmake -S examples/qemu_m33_smoke -B build/qemu_tiny \
  -DCMAKE_TOOLCHAIN_FILE=$PWD/examples/qemu_m33_smoke/toolchain-arm-none-eabi.cmake \
  -DPOT_PROFILE=tiny
```

Or `include(cmake/PolyOnTest.cmake)` after setting `POT_PROFILE`.

## Orthogonal knobs

| Knob | Effect |
|------|--------|
| `POT_MINIMAL_PRINT` | Force text path (no COBS) |
| `POT_EXCLUDE_FLOAT` | Drop float/double asserts |
| `POT_NO_LONGJMP` | PROTECT always succeeds; ABORT only sets fail |
| `POT_USE_HEAP` | Enable `pot_register_heap_case` |
| `POT_USE_SECTION_REGISTRY` | Place cases in `.pot_info` |
| `POT_FREESTANDING` | No stdio; set writer yourself |

!!! tip "Freestanding"
    On bare metal, call `pot_set_writer` before `pot_run_*` so events
    reach your UART / semihosting sink.

## Mutex hooks (full)

```c
void pot_set_locks(pot_lock_fn_t lock, pot_lock_fn_t unlock, void *user);
```

When set, Core wraps assert fail-flag updates and writer emit. No-ops if NULL.
Intended for multithreaded **host** runners under the full profile.

## Heap registration

```c
#define POT_USE_HEAP
int pot_register_heap_case(const char *suite, const char *group,
                                const char *name, pot_fn_t fn);
```

Static ctor lists remain the default.

## Section registry (GNU ld)

Default discovery uses `__attribute__((constructor))`. For section-based
registration:

1. Compile with `-DPOT_USE_SECTION_REGISTRY`
2. Keep the section in the linker script:

```ld
.pot_info : {
  PROVIDE(__start_pot_info = .);
  KEEP(*(.pot_info))
  PROVIDE(__stop_pot_info = .);
} > FLASH
```

3. At run start, Core walks `__start_pot_info` … `__stop_pot_info`
   (GNU/Clang non-Apple). Host builds can keep the ctor path and omit the script.

## Size table (reference)

See [`examples/profile_sizes/README.md`](https://github.com/malto101/Open-PolyTest-Framework/blob/main/examples/profile_sizes/README.md)
for measured QEMU/host sizes after a local build.
