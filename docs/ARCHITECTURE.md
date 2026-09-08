# OpenCAGD architecture

OpenCAGD keeps mathematical geometry independent from presentation backends.

```text
Core
  ├─ geometry primitives
  └─ basic math
       │
       ├──────────────► Curve
       │                 │
       │                 └────► Surface
       │                           │
       └────► LinearAlgebra        │
             (Eigen private)       ▼
                            Visualization
                            backend-neutral data
                              │          │
                              ▼          ▼
                            Plot        VTK
                         Matplotlib   VTK 9.4

Core ───────────────────────────────► Parallel (MPI)
```

## Dependency rule

1. `Core`, `Curve`, `Surface` never include Python, Matplotlib, VTK, MPI, or Eigen headers in their public interfaces.
2. `Visualization` contains only backend-neutral render data and the `Renderer` interface.
3. `Plot` and `VTK` consume exactly the same `Visualization` data objects.
4. `LinearAlgebra` uses Eigen only inside `.cpp` files; public APIs remain normal C++14 classes and `std::vector`.
5. `Parallel` is optional and isolated.

This makes adding another backend (OpenGL, Qt, WebGPU, etc.) a new library rather than a rewrite of geometry code.

## Build tree

```text
build/<preset>/
├── lib/              # OpenCAGD libraries
├── examples/         # example executables
│   └── output/
│       ├── plot/     # Matplotlib PNG output
│       └── vtk/      # VTK PNG output
├── tests/            # test executables
└── third_party/      # build products such as GoogleTest
```

## Renderer switching

The geometry and scene data are identical:

```cpp
opencagd::visualization::Surface3DData data =
    opencagd::visualization::make_surface_3d_data(grid, control_net);

// Matplotlib
opencagd::plot::MatplotlibRenderer plot;
plot.render(data, options);

// VTK
opencagd::vtk::VTKRenderer vtk;
vtk.render(data, options);
```

No surface evaluation, sampling, normal calculation, or NURBS algorithm is duplicated in a renderer.
