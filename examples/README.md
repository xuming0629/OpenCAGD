# OpenCAGD Examples

`examples/` contains runnable learning/demo programs only. Every `.cpp` is built as an executable.
Unit tests do **not** belong here; all tests live under `tests/` and use GoogleTest.

## Source layout

- `core/` — version, Point3D/Vector3D, power basis
- `curve/` — Bernstein, Bezier, B-Spline, NURBS, runtime polymorphism
- `surface/` — Bezier/B-Spline/NURBS surfaces and differential geometry
- `linalg/` — Eigen-backed linear solve and least squares
- `visualization/` — book-oriented visual scenes shared by Plot and VTK backends
- `plot/` — direct Matplotlib backend usage
- `vtk/` — direct VTK backend usage
- `parallel/` — MPI examples

## Build output

Executables are grouped by module:

```text
build/<preset>/examples/core/
build/<preset>/examples/curve/
build/<preset>/examples/surface/
build/<preset>/examples/linalg/
build/<preset>/examples/plot/
build/<preset>/examples/vtk/
build/<preset>/examples/parallel/
```

Generated figures/screenshots are written to:

```text
build/<preset>/examples/output/plot/
build/<preset>/examples/output/vtk/
```
