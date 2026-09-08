#pragma once

#include <opencagd/core/export.h>
#include <opencagd/geometry/point3d.h>
#include <opencagd/geometry/vector3d.h>
#include <opencagd/surface/parametric_surface.h>

namespace opencagd {
namespace surface {

struct OPENCAGD_SURFACE_API SurfaceDifferentialGeometry
{
    geometry::Point3D point;
    geometry::Vector3D du;
    geometry::Vector3D dv;
    geometry::Vector3D duu;
    geometry::Vector3D duv;
    geometry::Vector3D dvv;
    geometry::Vector3D normal;

    double E;
    double F;
    double G;
    double e;
    double f;
    double g;
    double gaussian_curvature;
    double mean_curvature;
    double principal_curvature_max;
    double principal_curvature_min;

    SurfaceDifferentialGeometry();
};

OPENCAGD_SURFACE_API SurfaceDifferentialGeometry differential_geometry(
    const ParametricSurface& surface,
    double u,
    double v);

} // namespace surface
} // namespace opencagd
