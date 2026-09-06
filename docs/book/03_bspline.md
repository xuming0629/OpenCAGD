# 学习阶段 03：B-Spline 基函数与曲线

## 1. 节点矢量

\[
U=\{u_0,u_1,\ldots,u_m\},\qquad u_i\le u_{i+1}.
\]

对于 p 次 B-Spline，控制点数量、节点数量满足

\[
m=n+p+1,
\]

其中控制点编号为 \(0,\ldots,n\)。

OpenCAGD 的 `KnotVector` 负责：

- 非递减校验；
- 参数域；
- 基函数数量；
- `find_span(u)`。

## 2. Cox-de Boor

零次基函数：

\[
N_{i,0}(u)=
\begin{cases}
1,&u_i\le u<u_{i+1},\\
0,&\text{otherwise}.
\end{cases}
\]

递推：

\[
N_{i,p}(u)=
\frac{u-u_i}{u_{i+p}-u_i}N_{i,p-1}(u)
+
\frac{u_{i+p+1}-u}{u_{i+p+1}-u_{i+1}}N_{i+1,p-1}(u).
\]

代码没有使用低效的全递归，而是实现教材常见的 `FindSpan + BasisFuns` 局部迭代结构：一个参数点最多只有 \(p+1\) 个非零基函数。

## 3. B-Spline 曲线

\[
\mathbf C(u)=\sum_{i=0}^{n}N_{i,p}(u)\mathbf P_i.
\]

局部支撑意味着修改一个控制点只影响有限参数区间，这是 B-Spline 相比单段高次 Bezier 的核心优势之一。

## 当前应重点验证

- 基函数非负；
- 单位分解；
- 局部支撑；
- clamped B-Spline 的端点插值；
- 改变内部控制点只局部改变曲线。

## 代码入口

- `include/opencagd/curve/knot_vector.hpp`
- `include/opencagd/curve/bspline_basis.hpp`
- `include/opencagd/curve/bspline_curve.hpp`
- `examples/book/ch03_bspline/`
- `tests/curve/test_bspline.cpp`

## 4. DersBasisFuns：基函数导数

OpenCAGD 现在实现了《The NURBS Book》Algorithm A2.3：`DersBasisFuns`：

```cpp
auto ders = basis_function_derivatives(knots, span, u, 2);
```

其中：

```text
ders[0][j] = N_j,p(u)
ders[1][j] = N'_j,p(u)
ders[2][j] = N''_j,p(u)
```

单位分解求导后给出非常重要的验证恒等式：

\[
\sum_i N_{i,p}(u)=1,
\qquad
\sum_i N'_{i,p}(u)=0,
\qquad
\sum_i N''_{i,p}(u)=0.
\]

## 5. B-Spline 曲线导数、切线与曲率

\[
\mathbf C^{(k)}(u)=\sum_iN_{i,p}^{(k)}(u)\mathbf P_i.
\]

对应：

```cpp
auto ders = curve.derivatives(u, 2);
auto d1 = curve.derivative(u, 1);
auto T = curve.tangent(u);
double kappa = curve.curvature(u);
```

二维和三维曲线统一使用 Gram determinant 形式计算曲率：

\[
\kappa=\frac{\sqrt{\|C'\|^2\|C''\|^2-(C'\cdot C'')^2}}{\|C'\|^3}.
\]
