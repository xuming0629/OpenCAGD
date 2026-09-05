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
