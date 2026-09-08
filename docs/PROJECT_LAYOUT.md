# OpenCAGD project layout

OpenCAGD keeps production libraries, runnable examples, and GoogleTest tests separate.

```text
OpenCAGD/
├── include/opencagd/        # public C++14 API
├── src/                     # .cpp implementations and module libraries
│   ├── core/
│   ├── curve/
│   ├── surface/
│   ├── linalg/
│   ├── visualization/
│   ├── plot/
│   ├── vtk/
│   └── parallel/
├── examples/                # runnable programs only
│   ├── core/
│   ├── curve/
│   ├── surface/
│   ├── linalg/
│   ├── visualization/
│   ├── plot/
│   ├── vtk/
│   └── parallel/
└── tests/                   # GoogleTest only
    ├── core/
    ├── curve/
    ├── surface/
    ├── linalg/
    ├── visualization/
    ├── plot/
    ├── vtk/
    └── parallel/
```

## Build tree

No executable is emitted into the source tree.

```text
build/<preset>/
├── lib/                     # OpenCAGD libraries
├── examples/
│   ├── core/
│   ├── curve/
│   ├── surface/
│   ├── linalg/
│   ├── visualization/
│   ├── plot/
│   ├── vtk/
│   ├── parallel/
│   └── output/
│       ├── plot/
│       └── vtk/
└── tests/
    ├── core/
    ├── curve/
    ├── surface/
    ├── linalg/
    ├── visualization/
    ├── plot/
    ├── vtk/
    └── parallel/
```

## Testing rule

`tests/` is GoogleTest-only. OpenCAGD targets C++14, so the supported GoogleTest line is 1.14.x. The previously supplied GoogleTest 1.18 package requires C++17 and should not be used for this C++14 test configuration.

## Visualization rule

`OpenCAGD::Visualization` contains backend-neutral scene data and builders. It does not know Matplotlib or VTK.

```text
Curve / Surface
      ↓
Visualization data/builders
      ├── OpenCAGD::Plot  -> Matplotlib/Python
      └── OpenCAGD::VTK   -> VTK 9.4
```

Book-oriented visualization sources under `examples/visualization/` are compiled once for each enabled renderer backend, so the mathematical scene setup is not duplicated.
