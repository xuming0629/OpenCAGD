# 学习阶段 01：幂基与参数多项式曲线

## 1. 标量幂基

n 次多项式写成

\[
p(t)=\sum_{i=0}^{n} a_i t^i.
\]

对应 OpenCAGD：

```cpp
auto basis = opencagd::math::power_basis(n, t);
```

## 2. 向量值参数曲线

二维或三维 CAGD 曲线把标量系数换成向量系数：

\[
\mathbf C(t)=\sum_{i=0}^{n}\mathbf a_i t^i.
\]

`PolynomialCurve` 使用 Horner 法求值：

\[
\mathbf C(t)=\mathbf a_0+t(\mathbf a_1+t(\cdots+t\mathbf a_n)).
\]

Horner 法避免显式重复计算高次幂，是后续理解“基函数表示”和“稳定求值算法”之间区别的起点。

## 代码入口

- `include/opencagd/math/power_basis.hpp`
- `include/opencagd/curve/polynomial_curve.hpp`
- `examples/book/ch01_foundations/`
