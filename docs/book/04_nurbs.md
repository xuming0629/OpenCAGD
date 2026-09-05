# 学习阶段 04：NURBS 曲线

## 1. 有理 B-Spline

NURBS 曲线：

\[
\mathbf C(u)=
\frac{\sum_{i=0}^{n}N_{i,p}(u)w_i\mathbf P_i}
     {\sum_{i=0}^{n}N_{i,p}(u)w_i}.
\]

定义有理基函数

\[
R_{i,p}(u)=
\frac{N_{i,p}(u)w_i}
     {\sum_jN_{j,p}(u)w_j},
\]

则

\[
\mathbf C(u)=\sum_iR_{i,p}(u)\mathbf P_i.
\]

对应 API：

```cpp
auto R = curve.rational_basis_functions(u);
```

可以直接验证 `sum(R_i) = 1`，帮助把“B-Spline 基函数”和“有理基函数”区分开。

## 2. 为什么权重重要

当所有权重相等时，NURBS 退化为普通 B-Spline。改变某个权重会改变该控制点对曲线的“吸引程度”。

## 3. 第一个关键算例：精确 1/4 圆

二次 NURBS：

```text
P0 = (1, 0), w0 = 1
P1 = (1, 1), w1 = sqrt(2)/2
P2 = (0, 1), w2 = 1
U  = [0,0,0,1,1,1]
```

可以精确表示单位圆第一象限，因此任意参数点都满足

\[
x^2+y^2=1.
\]

这也是理解“NURBS 为什么成为 CAD 标准曲线表示”的重要入口：多项式参数曲线无法精确表示圆，但有理多项式可以。

## 代码入口

- `include/opencagd/curve/nurbs_curve.hpp`
- `examples/book/ch04_nurbs/00_quarter_circle.cpp`
- `tests/curve/test_nurbs.cpp`
- `examples/visualization/03_plot_nurbs_circle.cpp`
