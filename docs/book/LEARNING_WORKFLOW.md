# 每读一个新算法时怎么加入 OpenCAGD

以后学习这本书时，每个新主题都按下面 6 步推进。这样项目不会变成“看懂了公式但代码散掉”的笔记仓库。

## 1. 先写数学定义

在 `docs/book/` 记录：

- 符号；
- 参数范围；
- 输入/输出；
- 公式；
- 几何意义；
- 特殊情况。

## 2. 先写最直接的参考实现

如果教材同时给出递归定义和高效算法，优先保留容易对应公式的版本，确认理解后再实现高效版本。

例如 B-Spline 学习顺序：

```text
Cox-de Boor 定义
      ↓
理解局部支撑
      ↓
FindSpan
      ↓
BasisFuns
```

## 3. 正式 API 放入领域目录

不要创建 `chapter3_algorithm.hpp` 之类的教材式核心 API。

应该放成：

```text
include/opencagd/curve/bspline_basis.hpp
include/opencagd/curve/bspline_curve.hpp
```

这样未来脱离教材后，OpenCAGD 仍然是一套正常的几何库。

## 4. 创建一个最小 example

每个 example 只回答一个问题。

例如：

- `00_find_span.cpp`：只看 span；
- `01_bspline_basis.cpp`：只看基函数；
- `02_bspline_curve.cpp`：再组合成曲线。

不要一开始就在一个 demo 中同时做十几个算法。

## 5. 把数学性质变成测试

优先测试“不变量”，而不是只比较几个硬编码数字：

- Bernstein/B-Spline/NURBS 基函数单位分解；
- Bezier 端点性质；
- clamped B-Spline 端点插值；
- de Casteljau 与 Bernstein 求值一致；
- NURBS 圆弧满足 `x*x + y*y == 1`；
- 节点插入前后曲线几何形状不变。

## 6. 需要几何直觉时再画图

核心算法不依赖 Python。

```text
OpenCAGD::OpenCAGD      <- 数学/几何核心
OpenCAGD::Plotting      <- CPython + Matplotlib 可选层
```

图形是验证工具，不应该成为算法正确运行的前提。

## Definition of Done

一个学习主题只有同时满足下面几项才算“真正完成”：

```text
[ ] 能自己解释公式
[ ] 知道几何意义
[ ] 有 C++20 实现
[ ] 有最小 example
[ ] 有数学性质测试
[ ] 需要时有 plot
[ ] README / checklist 已更新
```
