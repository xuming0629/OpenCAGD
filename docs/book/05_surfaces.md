# 学习阶段 05：Tensor-product 曲面与微分几何

## 1. Tensor-product B-Spline 曲面

\[
\mathbf S(u,v)=\sum_i\sum_jN_{i,p}(u)M_{j,q}(v)\mathbf P_{ij}.
\]

混合偏导为：

\[
\mathbf S^{(k,l)}(u,v)=
\sum_i\sum_jN_{i,p}^{(k)}(u)M_{j,q}^{(l)}(v)\mathbf P_{ij}.
\]

OpenCAGD 的 Bezier、B-Spline、NURBS 三类曲面现在统一提供：

```cpp
surface.derivative(u, v, order_u, order_v);
surface.partial_u(u, v);
surface.partial_v(u, v);
```

## 2. 曲面法向

\[
\mathbf n=
\frac{\mathbf S_u\times\mathbf S_v}
     {\|\mathbf S_u\times\mathbf S_v\|}.
\]

对应：

```cpp
auto n = opencagd::surface::normal(surface, u, v);
```

## 3. 第一基本形式

\[
E=S_u\cdot S_u,
\qquad
F=S_u\cdot S_v,
\qquad
G=S_v\cdot S_v.
\]

## 4. 第二基本形式

\[
e=n\cdot S_{uu},
\qquad
f=n\cdot S_{uv},
\qquad
g=n\cdot S_{vv}.
\]

## 5. Gaussian / Mean curvature

\[
K=\frac{eg-f^2}{EG-F^2},
\]

\[
H=\frac{Eg-2Ff+Ge}{2(EG-F^2)}.
\]

主曲率：

\[
k_{1,2}=H\pm\sqrt{H^2-K}.
\]

统一接口：

```cpp
auto dg = opencagd::surface::differential_geometry(surface, u, v);
```

其中 `dg` 包含：

- `du`, `dv`；
- `duu`, `duv`, `dvv`；
- `normal`；
- `E,F,G,e,f,g`；
- `gaussian_curvature`；
- `mean_curvature`；
- 两个 principal curvatures。

推荐验证算例：

1. 平面：`K=0, H=0`；
2. 单位圆柱：`K=0, |H|=1/2`；
3. 单位球（下一阶段可加入）：`K=1, |H|=1`。
