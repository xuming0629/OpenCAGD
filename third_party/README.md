# Third-party dependencies

This repository currently carries prebuilt **macOS Apple-Silicon (arm64)**
dependencies used by the local OpenCAGD development environment:

- `googletest/` — GoogleTest 1.18.0 installed package
- `openmpi/` — Open MPI 5.0.10 installed package

They are consumed by CMake through `OPENCAGD_USE_BUNDLED_DEPS=ON`.

For Linux or Windows, configure with:

```bash
cmake -S . -B build -DOPENCAGD_USE_BUNDLED_DEPS=OFF
```

and provide native system MPI and GoogleTest packages.
