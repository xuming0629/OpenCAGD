
## Unreleased

- Standardized all tests on GoogleTest + CTest discovery.
- Grouped test binaries under `build/<preset>/tests/<module>/`.
- Expanded module examples for Core, Curve, Surface, LinearAlgebra, Plot, VTK and MPI.
- Grouped example binaries under `build/<preset>/examples/<module>/`.
- Kept backend-neutral visualization scenes reusable by both Matplotlib and VTK.
# Changelog

## 1.0.0 - C++14 architecture refresh

- Refactored OpenCAGD into compiled C++14 `.h + .cpp` modules.
- Added `ParametricCurve` and `ParametricSurface` runtime-polymorphic interfaces.
- Kept Curve and Surface mathematics independent from render backends.
- Added backend-neutral `OpenCAGD::Visualization` data and `Renderer` interface.
- Moved Python runtime and Matplotlib implementation into independent `OpenCAGD::Plot`.
- Added independent `OpenCAGD::VTK` renderer backend for VTK 9.4.
- Added `OpenCAGD::LinearAlgebra`; Eigen 5 is used only inside `.cpp` implementation files.
- Preserved optional `OpenCAGD::Parallel` MPI module.
- Added bundled dependency discovery policy for Eigen/OpenMPI/VTK.
- Added explicit warning/documentation that supplied MPI/VTK binaries are macOS arm64.
- Added CMake presets for Linux/macOS and Visual Studio 2022.
- Reorganized examples and tests into module-local CMake files.
- Example binaries now go to `build/<preset>/examples`.
- Test binaries now go to `build/<preset>/tests`.
- Visualization output goes to `build/<preset>/examples/output/{plot,vtk}`.
- The same visualization example source is built against either Plot or VTK.
- Added mathematical property tests for Bézier, B-Spline, NURBS, surfaces and visualization data.
- Added Eigen-backed linear-system and least-squares tests.
- Added Matplotlib backend smoke test.
- Added `BezierCurve::construction_levels()` for textbook De Casteljau visualization.
- Verified C++14 core/Eigen build, tests, Plot backend, static/shared libraries and installed-package consumption on Linux.
- Verified VTK and MPI source compatibility at C++14 syntax level against the supplied headers; their supplied binary packages are macOS arm64 and therefore cannot be linked in the Linux validation environment.
