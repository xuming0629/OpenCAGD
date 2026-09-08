# OpenCAGD Visualization Examples

这组 examples 用同一份数学代码同时服务于两套后端：

```text
Curve / Surface algorithms
        ↓
Visualization data
        ├── MatplotlibRenderer
        └── VTKRenderer
```

因此后续按教材增加一个例子，不需要再复制一份 VTK 版或 Plot 版。

## 1. 编译 Plot 后端

Linux / macOS：

```bash
cmake --preset debug-plot
cmake --build --preset debug-plot -j
```

无桌面环境：

```bash
cmake -S . -B build/headless \
  -DCMAKE_BUILD_TYPE=Debug \
  -DOPENCAGD_ENABLE_PLOT=ON \
  -DOPENCAGD_EXAMPLE_SHOW=OFF
cmake --build build/headless -j
```

输出程序位于：

```text
build/debug-plot/examples/
```

PNG 位于：

```text
build/debug-plot/examples/output/plot/
```

## 2. 编译 VTK 后端

```bash
cmake --preset debug-vtk
cmake --build --preset debug-vtk -j
```

PNG 位于：

```text
build/debug-vtk/examples/output/vtk/
```

你上传的 `third_party/vtk-9.4` 是 macOS arm64 安装包；Linux/Windows 需要提供本机 VTK 9.4。

## 3. 同一源文件生成两个程序

例如：

```text
examples/visualization/06_nurbs_circle.cpp
```

当 Plot 与 VTK 都启用时会构建：

```text
opencagd_plot_06_nurbs_circle
opencagd_vtk_06_nurbs_circle
```

例子本身只构造：

```cpp
opencagd::visualization::Curve2DData data;
opencagd::visualization::RenderOptions options;
```

后端由 CMake 选择。

## 4. 推荐学习顺序

### 00 Bernstein basis

观察非负性、分割统一性、对称性。

### 01 Bézier curve

控制多边形 + Bézier 曲线。

### 02 De Casteljau construction

通过 `BezierCurve::construction_levels(t)` 绘制每一级线性插值。

### 03 Bézier subdivision

在 `t=0.5` 分成左右两段，并验证连接点一致。

### 04 B-Spline basis

观察局部支撑、partition of unity、knot span。

### 05 B-Spline curve

观察控制点局部影响。

### 06 Exact NURBS circle

使用 `sqrt(2)/2` 权重精确表示圆。

### 07 Bézier surface

双三次 Bézier 曲面 + 控制网。

### 08 B-Spline surface

张量积 B-Spline 曲面 + 控制网。

### 09 NURBS cylinder + normals

NURBS 精确四分之一圆柱 + 由 `Surface::normal()` 计算的法向矢量。

## 5. Backend-neutral API

```cpp
opencagd::visualization::Surface3DData data =
    opencagd::visualization::make_surface_3d_data(grid, control_net);

opencagd::visualization::append_surface_normals(
    data, surface, 6, 4, 0.2);
```

Plot：

```cpp
opencagd::plot::MatplotlibRenderer renderer;
renderer.render(data, options);
```

VTK：

```cpp
opencagd::vtk::VTKRenderer renderer;
renderer.render(data, options);
```

Visualization 层不包含 Python 或 VTK 头文件。

## 6. 交互与保存

`RenderOptions::show` 控制是否打开交互窗口：

```cpp
opencagd::visualization::RenderOptions options;
options.show = true;
```

例子统一通过 CMake 选项控制：

```bash
-DOPENCAGD_EXAMPLE_SHOW=OFF
```

这对服务器和 CI 很方便。
