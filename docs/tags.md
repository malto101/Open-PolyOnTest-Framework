# Tags and filters

## Declaring tags

Tags attach at suite, group, or case level (small/full profiles):

```c
POLYTEST_SUITE_TAGS(Math, "host", "smoke");
POLYTEST_GROUP_TAGS(Math, Basic, "unit");
TEST_TAGS(Math, Basic, SkipMe, "skipdemo") { IGNORE(); }
```

A case matches a tag filter if **any** of its own tags, its group tags, or its
suite tags contain the requested string.

!!! warning "Profile gated"
    Tags require `POLYTEST_CFG_HAS_TAGS` (small/full). The tiny profile has no
    tag filtering — use separate binaries or hard-coded runner entry points.

## Running a subset

### In-process

```c
polytest_run_tag("smoke");
polytest_run_suite("Math");
polytest_run_group("Math", "Basic");
polytest_run_from_env();  // preferred for host binaries
```

### Environment (host)

| Variable | Effect |
|----------|--------|
| `POLYTEST_TAG` | `polytest_run_tag` |
| `POLYTEST_SUITE` + `POLYTEST_GROUP` | `polytest_run_group` |
| `POLYTEST_SUITE` alone | `polytest_run_suite` |

Priority: tag → suite+group → suite → all.

```bash
POLYTEST_TAG=unit ./build/host_c/host_c_tests
```

### CLI

```bash
cargo run -p open-polytest -- run --target host \
  --config examples/host_c/polytest.toml --tag smoke
```

See [CLI](cli.md). Filters are **host-only** in v0.1.
