# OpenCAGD Roadmap

OpenCAGD 同时有两个目标：

1. 作为《非均匀有理 B 样条》学习代码框架；
2. 逐步成长为结构清晰的现代 C++20 CAGD 算法库。

详细学习进度见 `docs/book/STUDY_CHECKLIST.md`。

## Phase 1 — Curve foundation（已完成）

- Point arithmetic / numeric helpers
- Power basis / polynomial curve
- Bernstein basis
- Bezier / de Casteljau / subdivision / derivative
- Knot vector / FindSpan / BasisFuns
- B-Spline curve
- NURBS curve
- exact NURBS quarter circle
- C++ embedded Python + Matplotlib

## Phase 2 — Differential geometry（已完成）

- `DersBasisFuns` / Algorithm A2.3
- B-Spline curve derivatives
- NURBS rational curve derivatives
- unit tangent / curve curvature
- Bezier / B-Spline / NURBS surface derivatives
- surface normal
- first / second fundamental forms
- Gaussian / mean / principal curvatures

## Phase 3 — Core CAD refinement algorithms（下一步）

- knot multiplicity
- knot insertion
- knot refinement / removal
- Bezier decomposition
- degree elevation
- interpolation / approximation

## Phase 4 — Geometry algorithms

- projection
- intersections
- trimming foundations
- tessellation

## Optional engineering layers

- MPI parallel experiments
- VTK 3D viewer
- benchmark suite
