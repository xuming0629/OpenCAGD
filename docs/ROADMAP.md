# OpenCAGD Roadmap

OpenCAGD 同时有两个目标：

1. 作为《非均匀有理 B 样条》学习代码框架；
2. 逐步成长为结构清晰的现代 C++20 CAGD 算法库。

详细学习进度见 `docs/book/STUDY_CHECKLIST.md`。

## Phase 1 — Curve foundation（当前）

- Point arithmetic / numeric helpers
- Power basis / polynomial curve
- Bernstein basis
- Bezier / de Casteljau / subdivision / derivative
- Knot vector / FindSpan / BasisFuns
- B-Spline curve
- NURBS curve
- exact NURBS quarter circle
- C++ embedded Python + Matplotlib

## Phase 2 — Core NURBS algorithms

- DersBasisFuns
- B-Spline / NURBS derivatives
- knot insertion / refinement / removal
- Bezier decomposition
- degree elevation
- interpolation / approximation

## Phase 3 — Tensor-product surfaces

- Bezier surface
- B-Spline surface
- NURBS surface
- partial derivatives / normals / curvature

## Phase 4 — Geometry algorithms

- projection
- intersections
- trimming foundations
- tessellation

## Optional engineering layers

- MPI parallel experiments
- VTK 3D viewer
- benchmark suite
