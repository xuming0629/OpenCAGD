# OpenCAGD Architecture

OpenCAGD keeps **library architecture** and **book-learning order** separate.

## Core library

```text
include/opencagd/
├── core/           project/version facilities
├── math/           numerical and polynomial helpers
├── geometry/       Point and geometric primitives
├── curve/          Bezier, B-Spline, NURBS
├── surface/        future tensor-product surfaces
├── algorithm/      future refinement/interpolation/intersection
├── parallel/       optional MPI helpers
└── visualization/  optional adapters
```

Core dependency direction:

```text
visualization     parallel
      \             /
       \           /
        algorithm
           |
     curve / surface
           |
        geometry
           |
          math
           |
          core
```

`OpenCAGD::OpenCAGD` never depends on MPI or Python.

## Learning layer

```text
docs/book/             formula + geometric meaning + progress
examples/book/         one minimal executable per concept
examples/visualization visual verification
tests/curve/           invariants and regression tests
```

A concept graduates into the library only when its mathematics and expected invariants are clear.

## Build targets

- `OpenCAGD::OpenCAGD`: core geometry/CAGD library;
- `OpenCAGD::Plotting`: optional embedded CPython + Matplotlib adapter;
- `OpenCAGD::MPI`: optional MPI dependency target.

## Versioning

`VERSION` remains the only manually edited project version. CMake generates `opencagd/core/version.hpp` in the build tree.
