# 从原模板版迁移到 C++14 编译库版

## 1. Point

旧：

```cpp
Point<double, 2>
Point<double, 3>
Point2d
Point3d
```

新：

```cpp
opencagd::geometry::Point3D
opencagd::geometry::Vector3D
```

二维点使用 `Point3D(x, y, 0.0)`。

## 2. Curve

旧：

```cpp
BezierCurve<2>
BezierCurve<3>
BSplineCurve<3>
NURBSCurve<3>
```

新：

```cpp
BezierCurve
BSplineCurve
NURBSCurve
```

三者都继承：

```cpp
ParametricCurve
```

旧 Bézier 的 `derivative()` 返回导数曲线；新统一为：

```cpp
Vector3D derivative(double u, std::size_t order = 1) const;
BezierCurve derivative_curve() const;
```

两个语义不再混用。

## 3. Surface

旧模板：

```cpp
BezierSurface<3>
BSplineSurface<3>
NURBSSurface<3>
```

新：

```cpp
BezierSurface
BSplineSurface
NURBSSurface
```

三者都继承 `ParametricSurface`。

## 4. `.hpp` -> `.h + .cpp`

公开声明位于：

```text
include/opencagd/**/*.h
```

实现位于：

```text
src/**/*.cpp
```

调用方只编译自己的代码并链接 OpenCAGD 库，不再重复实例化模板算法。

## 5. C++ 标准

新版本只要求 C++14，不使用：

- C++17 nested namespace syntax；
- `[[nodiscard]]`；
- `std::remove_cvref_t`；
- structured bindings；
- C++20 Concepts；
- `std::span` 等高版本设施。
