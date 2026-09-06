#pragma once

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include <opencagd/geometry/point.hpp>
#include <opencagd/math/numeric.hpp>

namespace opencagd::surface
{

/**
 * @brief 三维参数曲面在一点处的一、二阶微分几何量。
 *
 * 一阶偏导：
 *
 *      S_u, S_v
 *
 * 二阶偏导：
 *
 *      S_uu, S_uv, S_vv
 *
 * 第一基本形式：
 *
 *      E = S_u · S_u
 *      F = S_u · S_v
 *      G = S_v · S_v
 *
 * 第二基本形式：
 *
 *      e = n · S_uu
 *      f = n · S_uv
 *      g = n · S_vv
 *
 * Gaussian 曲率：
 *
 *      K = (eg - f²) / (EG - F²)
 *
 * Mean 曲率：
 *
 *      H = (E g - 2 F f + G e) / (2(EG - F²))
 */
struct SurfaceDifferentialGeometry
{
    Point3d point{};

    Point3d du{};
    Point3d dv{};

    Point3d duu{};
    Point3d duv{};
    Point3d dvv{};

    Point3d normal{};

    double E{};
    double F{};
    double G{};

    double e{};
    double f{};
    double g{};

    double gaussian_curvature{};
    double mean_curvature{};

    double principal_curvature_max{};
    double principal_curvature_min{};
};

/**
 * @brief 计算任意具有 derivative(u,v,ku,kv) 接口的三维参数曲面微分几何量。
 *
 * 可直接用于：
 *
 *      BezierSurface3d
 *      BSplineSurface3d
 *      NURBSSurface3d
 */
template <typename Surface>
[[nodiscard]]
SurfaceDifferentialGeometry differential_geometry(
    const Surface& surface,
    double u,
    double v)
{
    using point_type = typename Surface::point_type;

    static_assert(
        point_type::dimension == 3,
        "Surface differential geometry currently requires a 3D surface");

    SurfaceDifferentialGeometry result;

    result.point = surface.evaluate(u, v);

    result.du = surface.derivative(u, v, 1, 0);
    result.dv = surface.derivative(u, v, 0, 1);

    result.duu = surface.derivative(u, v, 2, 0);
    result.duv = surface.derivative(u, v, 1, 1);
    result.dvv = surface.derivative(u, v, 0, 2);

    const Point3d normal_vector = cross(result.du, result.dv);
    const double normal_length = normal_vector.norm();

    if (normal_length <= math::default_tolerance)
    {
        throw std::runtime_error(
            "Surface normal is undefined because Su and Sv are linearly dependent");
    }

    result.normal = normal_vector / normal_length;

    result.E = dot(result.du, result.du);
    result.F = dot(result.du, result.dv);
    result.G = dot(result.dv, result.dv);

    result.e = dot(result.normal, result.duu);
    result.f = dot(result.normal, result.duv);
    result.g = dot(result.normal, result.dvv);

    const double metric_determinant =
        result.E * result.G - result.F * result.F;

    if (std::abs(metric_determinant) <= math::default_tolerance)
    {
        throw std::runtime_error(
            "Surface curvature is undefined at a singular parameterization");
    }

    result.gaussian_curvature =
        (result.e * result.g - result.f * result.f) /
        metric_determinant;

    result.mean_curvature =
        (result.E * result.g -
         2.0 * result.F * result.f +
         result.G * result.e) /
        (2.0 * metric_determinant);

    const double discriminant = std::max(
        0.0,
        result.mean_curvature * result.mean_curvature -
            result.gaussian_curvature);

    const double root = std::sqrt(discriminant);

    result.principal_curvature_max =
        result.mean_curvature + root;

    result.principal_curvature_min =
        result.mean_curvature - root;

    return result;
}

/**
 * @brief 只计算单位曲面法向量的便捷函数。
 */
template <typename Surface>
[[nodiscard]]
Point3d normal(
    const Surface& surface,
    double u,
    double v)
{
    const auto du = surface.derivative(u, v, 1, 0);
    const auto dv = surface.derivative(u, v, 0, 1);

    const Point3d value = cross(du, dv);
    const double length = value.norm();

    if (length <= math::default_tolerance)
    {
        throw std::runtime_error(
            "Surface normal is undefined because Su and Sv are linearly dependent");
    }

    return value / length;
}

} // namespace opencagd::surface
