# OpenCAGD

**OpenCAGD** 是一个以 C++20 实现 CAGD / B-Spline / NURBS 算法的学习与研究框架。

当前阶段的核心目标很明确：**一边学习《非均匀有理 B 样条》，一边把书中的公式逐步实现为可测试、可绘图、可继续扩展的现代 C++ 几何库。**

项目版本只来自 [`VERSION`](./VERSION)。

## 当前已经形成的学习链路

```text
幂基 / Polynomial Curve
          ↓
Bernstein Basis
          ↓
Bezier Curve ── de Casteljau
          ↓
Knot Vector ── FindSpan
          ↓
B-Spline Basis / BasisFuns
          ↓
DersBasisFuns / Curve Derivatives
          ↓
B-Spline Curve Differential Geometry
          ↓
Rational Basis / NURBS Curve
          ↓
NURBS Derivatives / Tangent / Curvature
          ↓
Tensor-product Surface
          ↓
Surface Partials / Normal / Curvature
          ↓
Knot Insertion / Refinement / CAD Algorithms
```

已经实现到 **B-Spline/NURBS 曲线微分几何 + Bézier/B-Spline/NURBS 曲面微分几何**。

## 为什么这样组织

正式库按领域组织：

```text
include/opencagd/
├── math/
├── geometry/
├── curve/
├── surface/
├── algorithm/
├── parallel/
└── visualization/
```

教材学习按顺序组织：

```text
docs/book/
examples/book/
tests/curve/
examples/visualization/
```

这样“读书顺序”和“工程架构”互不冲突。

## 当前核心实现

- `Point<T, Dim>`：点/向量线性运算；
- Power basis / vector-valued polynomial curve；
- Bernstein basis；
- Bezier curve；
- de Casteljau evaluation；
- Bezier derivative / subdivision；
- Bezier → power basis；
- `KnotVector`；
- `FindSpan`；
- iterative `BasisFuns`；
- B-Spline curve；
- `DersBasisFuns`（The NURBS Book Algorithm A2.3）；
- B-Spline curve derivatives / tangent / curvature；
- NURBS curve；
- NURBS rational derivatives / tangent / curvature；
- exact quadratic NURBS quarter circle；
- tensor-product Bezier / B-Spline / NURBS surfaces；
- exact surface partial derivatives；
- surface normal / first & second fundamental forms；
- Gaussian / mean / principal curvatures；
- optional C++ → embedded CPython → Matplotlib plotting；
- optional MPI layer；
- GoogleTest regression tests。

## 学习入口

先读：

```text
docs/book/README.md
docs/book/01_foundations.md
docs/book/02_bernstein_bezier.md
docs/book/03_bspline.md
docs/book/04_nurbs.md
docs/book/STUDY_CHECKLIST.md
```

再运行对应例子：

```text
examples/book/ch01_foundations/
examples/book/ch02_bezier/
examples/book/ch03_bspline/
examples/book/ch04_nurbs/
```

## macOS M5 构建

建议先进入你用于 OpenCAGD 的 Conda 环境，使 CMake 找到同一套 Python：

```bash
conda activate pycax
python -m pip install matplotlib
```

然后：

```bash
cmake --preset debug -DPython3_EXECUTABLE="$(which python)"
cmake --build --preset debug --parallel
ctest --preset debug --output-on-failure
```

## 一次运行当前学习关键算例

```bash
./scripts/run_learning.sh
```

也可以单独运行：

```bash
./build/debug/bin/opencagd_ch02_bernstein
./build/debug/bin/opencagd_ch02_bezier
./build/debug/bin/opencagd_ch02_de_casteljau
./build/debug/bin/opencagd_ch03_find_span
./build/debug/bin/opencagd_ch03_bspline_basis
./build/debug/bin/opencagd_ch03_bspline_curve
./build/debug/bin/opencagd_ch03_basis_derivatives
./build/debug/bin/opencagd_ch03_curve_derivatives
./build/debug/bin/opencagd_ch04_nurbs_circle
./build/debug/bin/opencagd_ch04_nurbs_derivatives
./build/debug/bin/opencagd_ch05_surface_derivatives
./build/debug/bin/opencagd_ch05_nurbs_cylinder_geometry
```

## C++ 调用 Python Matplotlib

绘图不是通过 `system("python ...")` 启动外部脚本，而是独立目标：

```text
OpenCAGD C++
    ↓
Python C API / embedded CPython
    ↓
matplotlib.pyplot
```

核心库：

```text
OpenCAGD::OpenCAGD
```

完全不依赖 Python。

绘图适配层：

```text
OpenCAGD::Plotting
```

只有找到 Python Development 时才创建。

绘图示例：

```bash
mkdir -p outputs
./build/debug/bin/opencagd_plot_bernstein
./build/debug/bin/opencagd_plot_bezier
./build/debug/bin/opencagd_plot_bspline_basis
./build/debug/bin/opencagd_plot_nurbs_circle
```

输出到 `outputs/`。

## 下一步学习

当前最合适的下一组算法不是直接做 Surface，而是继续完成 B-Spline/NURBS 曲线核心：

```text
Knot Insertion
    ↓
Knot Refinement
    ↓
Bezier Decomposition
    ↓
Degree Elevation
```

这条链完成后，再进入 tensor-product NURBS surface 会非常顺。
