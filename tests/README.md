# OpenCAGD Tests

All files under `tests/` are GoogleTest unit/property tests. They verify mathematical invariants and API behavior; they are not teaching demos.

Build output is grouped by module under `build/<preset>/tests/<module>/`.
Run all discovered tests with:

```bash
ctest --preset debug --output-on-failure
```
