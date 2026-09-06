# OpenCAGD 学习进度表

## A. 数学基础

- [x] Point / 向量线性组合
- [x] 幂基
- [x] 向量值多项式曲线
- [x] 参数曲线一阶/二阶导数、单位切向与曲率
- [ ] 完整 Frenet frame / torsion

## B. Bezier

- [x] Bernstein 基函数
- [x] 单位分解
- [x] Bezier 曲线
- [x] de Casteljau 求值
- [x] Bezier → 幂基
- [x] 一阶导曲线
- [x] de Casteljau subdivision 基础 API
- [ ] 次数提升
- [ ] 多段 Bezier 与连续性

## C. B-Spline

- [x] KnotVector
- [x] FindSpan
- [x] BasisFuns
- [x] B-Spline 曲线
- [x] DersBasisFuns
- [x] 曲线导数
- [ ] 节点插入
- [ ] 节点细化
- [ ] Bezier 分解
- [ ] 次数提升

## D. NURBS

- [x] NURBS 曲线
- [x] 二次 NURBS 精确圆弧
- [ ] 齐次坐标实现
- [x] Rational basis 显式 API
- [x] NURBS 导数
- [ ] 完整圆 / 椭圆 / 圆锥曲线

## E. Surface

- [x] Bezier surface
- [x] B-Spline surface
- [x] NURBS surface
- [x] 偏导数 / 法向
- [x] Gaussian / Mean / Principal 曲率

## F. 几何算法

- [ ] Global interpolation
- [ ] Least-squares approximation
- [ ] Point projection
- [ ] Curve-curve intersection
- [ ] Curve-surface intersection
- [ ] Tessellation

完成一项的标准：**公式看懂 + C++ API + example + unit test + 必要时 plot**。
