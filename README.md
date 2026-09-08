# OpenCAGD

OpenCAGD is a **C++14, `.h + .cpp`, compiled-library CAGD/NURBS learning framework**. The project is organized so that mathematical geometry stays independent from visualization, MPI and third-party implementation details.

The long-term goal is to read a CAGD/NURBS textbook and implement each topic as a reusable library feature, with matching numerical tests and visual examples.

## Design goals

- C++14 core; no C++20 Concepts or header-only template architecture in OpenCAGD's public geometry API.
- Traditional compiled modules with clear namespaces and library boundaries.
- Linux, macOS and Windows CMake support.
- Curve/surface algorithms never depend on Matplotlib or VTK.
- One backend-neutral visualization data layer, with independent Plot and VTK renderers.
- Eigen 5.x hidden behind a non-template OpenCAGD linear-algebra API.
- MPI isolated in an optional parallel module.
- Examples and tests are emitted into the build tree, not mixed with source files.

## Module layout

```text
OpenCAGD
├── Core                 OpenCAGD::Core
├── Curve                OpenCAGD::Curve
├── Surface              OpenCAGD::Surface
├── LinearAlgebra        OpenCAGD::LinearAlgebra   (optional, Eigen 5)
├── Visualization        OpenCAGD::Visualization   (backend neutral)
├── Plot                 OpenCAGD::Plot            (optional, Python/Matplotlib)
├── VTK                  OpenCAGD::VTK             (optional, VTK 9.4)
└── Parallel             OpenCAGD::Parallel        (optional, MPI)
```

Dependency direction:

```text
Core
 ├────────────► Curve ────────────► Surface
 ├────────────► LinearAlgebra       │
 └────────────► Parallel            ▼
                              Visualization
                                │        │
                                ▼        ▼
                              Plot      VTK
```

`Plot` and `VTK` are peers. Neither backend owns sampling, NURBS evaluation, derivative calculation or surface-normal computation.

See [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md).

## Source tree

```text
include/opencagd/
├── core/
├── geometry/
├── math/
├── curve/
├── surface/
├── linalg/
├── visualization/
├── plot/
├── vtk/
└── parallel/

src/
├── core/
├── curve/
├── surface/
├── linalg/
├── visualization/
├── plot/
├── vtk/
└── parallel/

examples/
├── basic/
├── curve/
├── surface/
├── linalg/
├── visualization/
└── parallel/

tests/
├── core/
├── curve/
├── surface/
├── linalg/
├── visualization/
├── plot/
└── vtk/
```

Each major module has its own `CMakeLists.txt` and compiles into its own library.

## Build output layout

For a preset such as `debug-plot`:

```text
build/debug-plot/
├── lib/
│   ├── libopencagd_core.*
│   ├── libopencagd_curve.*
│   ├── libopencagd_surface.*
│   ├── libopencagd_linalg.*
│   ├── libopencagd_visualization.*
│   └── libopencagd_plot.*
├── examples/
│   ├── opencagd_hello
│   ├── opencagd_curve_demo
│   ├── opencagd_plot_00_bernstein_basis
│   └── output/
│       ├── plot/
│       └── vtk/
└── tests/
    ├── opencagd_test_core
    ├── opencagd_test_curve
    └── ...
```

This is intentional: **source directories contain source; build directories contain programs and generated images.**

## Quick start: Linux/macOS

Core + Eigen + tests/examples:

```bash
cmake --preset debug
cmake --build --preset debug -j
ctest --preset debug
```

Matplotlib backend:

```bash
cmake --preset debug-plot
cmake --build --preset debug-plot -j
ctest --preset debug-plot

./build/debug-plot/examples/opencagd_plot_06_nurbs_circle
```

By default visual examples open an interactive window and also save PNG output. For CI/headless servers:

```bash
cmake -S . -B build/headless \
  -DCMAKE_BUILD_TYPE=Debug \
  -DOPENCAGD_ENABLE_PLOT=ON \
  -DOPENCAGD_EXAMPLE_SHOW=OFF
```

VTK backend:

```bash
cmake --preset debug-vtk
cmake --build --preset debug-vtk -j
./build/debug-vtk/examples/opencagd_vtk_09_nurbs_cylinder_normals
```

All optional modules:

```bash
cmake --preset debug-all
cmake --build --preset debug-all -j
```

## Windows / Visual Studio 2022

```powershell
cmake --preset msvc-debug
cmake --build --preset msvc-debug
ctest --preset msvc-debug
```

For VTK on Windows, point `VTK_DIR` at a Windows VTK 9.4 build/install. The VTK package included in this snapshot is a macOS arm64 binary package and cannot be linked on Windows.

## Third-party dependencies

The supplied `third_party/` layout is:

```text
third_party/
├── eigen-5.0/
├── googletest/
├── openmpi/
└── vtk-9.4/
```

Important details:

- **Eigen 5.x**: portable headers are used by `OpenCAGD::LinearAlgebra`. Eigen types remain private to `.cpp` files.
- **OpenMPI**: the supplied binary tree is macOS arm64. On Linux/Windows, CMake uses `find_package(MPI)` unless you explicitly provide another bundle.
- **VTK 9.4**: the supplied binary tree is macOS arm64 (the included headers report VTK 9.4.2). On Linux/Windows, provide a native VTK 9.4 package.
- **GoogleTest 1.18** in the supplied binary tree advertises C++17. To keep this repository strictly C++14, OpenCAGD's built-in tests use a tiny C++14 test harness instead of forcing the whole test build to C++17.

See [`third_party/README.md`](third_party/README.md).

## Eigen integration without exposing Eigen

Public code remains ordinary C++14:

```cpp
opencagd::linalg::DenseMatrix a(2, 2);
a(0,0) = 2.0; a(0,1) = 1.0;
a(1,0) = 1.0; a(1,1) = 3.0;

std::vector<double> x =
    opencagd::linalg::solve_linear_system(a, {1.0, 2.0});
```

Internally the `.cpp` implementation uses Eigen 5. A consumer of OpenCAGD does not need to include Eigen headers.

## Plot and VTK use the same scene data

```cpp
opencagd::visualization::Surface3DData data =
    opencagd::visualization::make_surface_3d_data(grid, control_net);

opencagd::visualization::append_surface_normals(
    data, surface, 6, 4, 0.2);
```

Matplotlib:

```cpp
opencagd::plot::MatplotlibRenderer renderer;
renderer.render(data, options);
```

VTK:

```cpp
opencagd::vtk::VTKRenderer renderer;
renderer.render(data, options);
```

A program that links both backends can also select a renderer at runtime through `opencagd::visualization::Renderer` without modifying the geometry code.

## Visualization learning examples

The same source files are built for each enabled renderer:

| Example | Topic |
|---|---|
| `00_bernstein_basis` | Bernstein basis |
| `01_bezier_curve` | Bézier curve + control polygon |
| `02_bezier_de_casteljau` | De Casteljau construction levels |
| `03_bezier_subdivision` | Bézier subdivision |
| `04_bspline_basis` | B-Spline basis / partition of unity |
| `05_bspline_curve` | Cubic B-Spline curve |
| `06_nurbs_circle` | Exact quadratic NURBS unit circle |
| `07_bezier_surface` | Bicubic Bézier surface + control net |
| `08_bspline_surface` | Tensor-product B-Spline surface |
| `09_nurbs_cylinder_normals` | Exact NURBS quarter cylinder + normals |
| `10_backend_switch` | Runtime selection through `visualization::Renderer` (built when both backends are enabled) |

With both backends enabled, CMake generates both:

```text
opencagd_plot_06_nurbs_circle
opencagd_vtk_06_nurbs_circle
```

from the same `06_nurbs_circle.cpp`.

## Tests

Tests cover mathematical properties rather than only hard-coded sample values:

- Bézier evaluation vs De Casteljau.
- Bézier subdivision continuity.
- B-Spline partition of unity.
- Bézier-to-power-basis equivalence.
- Exact NURBS circle radius and curvature.
- Plane normal / Gaussian and mean curvature.
- Exact NURBS cylinder normal and Gaussian curvature.
- Backend-neutral surface normal glyph generation.
- Eigen-backed linear solve and least-squares solve.
- Matplotlib backend smoke rendering when enabled.
- VTK backend construction/link test when VTK is enabled.

## Install and consume

```bash
cmake -S . -B build/install-build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=$HOME/.local
cmake --build build/install-build -j
cmake --install build/install-build
```

Consumer:

```cmake
find_package(OpenCAGD REQUIRED)

target_link_libraries(my_app PRIVATE
    OpenCAGD::Curve
    OpenCAGD::Surface
    OpenCAGD::Visualization)
```

Optional targets are linked only when needed:

```cmake
OpenCAGD::LinearAlgebra
OpenCAGD::Plot
OpenCAGD::VTK
OpenCAGD::Parallel
```

## Next library directions

The current architecture is intended to grow without changing the module boundaries. Good next textbook-driven additions are:

1. knot insertion / refinement,
2. degree elevation,
3. Bézier extraction,
4. interpolation and approximation using `LinearAlgebra`,
5. Frenet frame / torsion / arc length,
6. surface tangent plane and principal directions,
7. curvature-field visualization,
8. trimmed NURBS surfaces,
9. topology/B-Rep layer,
10. VTK-based interactive inspection and later CAE/CAD visualization.

See [`docs/LEARNING_ROADMAP.md`](docs/LEARNING_ROADMAP.md).


## Examples and tests policy

- `examples/` contains module-oriented runnable programs; every example builds to an executable under `build/<preset>/examples/<module>/`.
- `tests/` contains GoogleTest tests only; test executables are under `build/<preset>/tests/<module>/` and are discovered by CTest.
- Plot and VTK are independent renderer backends. Shared CAGD visualization scenes are compiled once per enabled backend without coupling geometry algorithms to either renderer.

See `examples/README.md` and `tests/README.md`.
