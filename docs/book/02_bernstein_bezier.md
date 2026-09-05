# 学习阶段 02：Bernstein 基与 Bezier 曲线

## 1. Bernstein 基函数

\[
B_i^n(t)=\binom{n}{i}t^i(1-t)^{n-i},\qquad 0\le t\le1.
\]

重要性质：

- 非负性：\(B_i^n(t)\ge0\)；
- 单位分解：\(\sum_i B_i^n(t)=1\)；
- 端点性质；
- 对称性；
- 凸包性质的基础。

OpenCAGD：`curve/bernstein.hpp`。

## 2. Bezier 曲线

\[
\mathbf C(t)=\sum_{i=0}^{n}B_i^n(t)\mathbf P_i.
\]

控制点 \(\mathbf P_i\) 不再是幂基系数，而具有直接几何意义。

OpenCAGD：`BezierCurve<Dim>::evaluate(t)`。

## 3. de Casteljau

递推形式：

\[
\mathbf P_i^{(r)}(t)
=(1-t)\mathbf P_i^{(r-1)}(t)
+t\mathbf P_{i+1}^{(r-1)}(t).
\]

最终：

\[
\mathbf C(t)=\mathbf P_0^{(n)}(t).
\]

OpenCAGD 同时保留 Bernstein 直接求值和 de Casteljau，学习时可以逐点比较二者结果。

## 4. 同一曲线的不同基表示

`to_power_basis()` 把 Bezier 的 Bernstein 表示展开为幂基表示。这个例子用于建立一个重要认识：

> “曲线本身”与“选择哪组基函数描述曲线”是两件事。

## 代码入口

- `include/opencagd/curve/bernstein.hpp`
- `include/opencagd/curve/bezier_curve.hpp`
- `include/opencagd/curve/bezier_conversion.hpp`
- `examples/book/ch02_bezier/`
- `tests/curve/test_bezier.cpp`
