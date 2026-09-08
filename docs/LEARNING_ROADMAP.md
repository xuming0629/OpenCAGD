# OpenCAGD textbook learning roadmap

The project should grow in three synchronized tracks:

```text
mathematical derivation
        ↓
library implementation (.h/.cpp)
        ↓
property tests + visual example
```

## Curve track

1. Power basis and polynomial curves
2. Bernstein basis
3. Bézier evaluation
4. De Casteljau construction
5. Bézier subdivision
6. Bézier derivatives, tangent, speed, curvature
7. Knot vectors
8. B-Spline basis (Cox-de Boor)
9. B-Spline derivatives
10. Knot insertion and refinement
11. Degree elevation
12. NURBS homogeneous/rational representation
13. Exact conics and circles
14. Interpolation / approximation using `OpenCAGD::LinearAlgebra`
15. Arc length, Frenet frame and torsion

Every new algorithm should add:

- one focused header/source pair,
- mathematical invariants in `tests/curve`,
- one visual example that is automatically available in both Plot and VTK backends.

## Surface track

1. Tensor-product Bézier surface
2. Tensor-product B-Spline surface
3. NURBS surface
4. partial derivatives `S_u`, `S_v`, `S_uu`, `S_uv`, `S_vv`
5. tangent plane and normal
6. first fundamental form `(E,F,G)`
7. second fundamental form `(e,f,g)`
8. Gaussian / mean curvature
9. principal curvatures and directions
10. surface refinement
11. interpolation / fitting
12. trimming and topology/B-Rep

## Visualization track

The mathematics layer should never know which renderer is active.

```text
Surface/Curve
   ↓ sample/compute
Visualization data
   ├─ Plot backend
   └─ VTK backend
```

Future backends can be added as sibling libraries without editing Curve or Surface.
