# OpenCAGD NURBS Book Learning Track

这个目录是 OpenCAGD 的“教材学习层”。核心原则：

> 数学公式先理解，最小例子验证，再进入正式库 API，最后用单元测试和图形确认几何性质。

教材版本、中文译本页码可能不同，因此这里使用“学习阶段”编号，不强行绑定某一版书的章号。

## 学习闭环

每个主题按固定顺序推进：

```text
教材公式
   ↓
docs/book：公式、符号、几何意义
   ↓
include/opencagd：可复用 C++20 实现
   ↓
examples/book：最小可运行例子
   ↓
tests：数学不变量 / 几何性质
   ↓
examples/visualization：Matplotlib 图形验证
```

建议按文档顺序阅读：

```text
01_foundations.md
02_bernstein_bezier.md
03_bspline.md
04_nurbs.md
05_surfaces.md
```

## 当前已经实现

| 学习阶段 | 数学主题 | OpenCAGD 实现 | 例子 |
|---|---|---|---|
| 01 | 幂基、多项式参数曲线 | `math/power_basis.hpp`, `curve/polynomial_curve.hpp` | `ch01_foundations` |
| 02 | Bernstein 基、Bezier 曲线 | `curve/bernstein.hpp`, `curve/bezier_curve.hpp` | `ch02_bezier` |
| 02 | de Casteljau | `BezierCurve::evaluate_de_casteljau` | `02_de_casteljau.cpp` |
| 02 | Bezier → 幂基 | `curve/bezier_conversion.hpp` | `03_bezier_power_conversion.cpp` |
| 03 | 节点矢量、FindSpan | `curve/knot_vector.hpp` | `00_find_span.cpp` |
| 03 | B-Spline 基函数 | `curve/bspline_basis.hpp` | `01_bspline_basis.cpp` |
| 03 | B-Spline 曲线 | `curve/bspline_curve.hpp` | `02_bspline_curve.cpp` |
| 03 | B-Spline 基函数导数 | `curve/bspline_derivatives.hpp` | `03_basis_derivatives.cpp` |
| 03 | 曲线导数 / 切线 / 曲率 | `BSplineCurve::derivatives/tangent/curvature` | `04_curve_derivatives.cpp` |
| 04 | 有理 B-Spline / NURBS | `curve/nurbs_curve.hpp` | `00_quarter_circle.cpp` |
| 04 | NURBS 有理导数 / 曲率 | `NURBSCurve::derivatives/tangent/curvature` | `02_curve_derivatives.cpp` |
| 04 | 圆锥曲线精确表示 | 二次 NURBS 1/4 圆 | `opencagd_plot_nurbs_circle` |
| 05 | Tensor-product 曲面 | `surface/*_surface.hpp` | `ch05_surfaces` |
| 05 | 曲面偏导 / 法向 / 曲率 | `surface/differential_geometry.hpp` | `00_surface_derivatives.cpp` |

## 下一阶段

按依赖关系继续，而不是一次性全部写完：

1. Knot Multiplicity；
2. Knot Insertion / Knot Refinement；
3. Bezier Decomposition；
4. Degree Elevation；
5. Global Interpolation / Approximation；
6. Projection / Closest Point；
7. Curve/Curve、Curve/Surface Intersection；
8. Adaptive Tessellation / Trimmed Surface。

每完成一项，都同步补 `docs + example + test + plot`。
