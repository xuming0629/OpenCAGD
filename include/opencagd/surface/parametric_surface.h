#pragma once

#include <cstddef>
#include <opencagd/core/export.h>
#include <opencagd/geometry/domain.h>
#include <opencagd/geometry/point3d.h>
#include <opencagd/geometry/vector3d.h>

namespace opencagd {
namespace surface {

class OPENCAGD_SURFACE_API ParametricSurface
{
public:
    virtual ~ParametricSurface();

    virtual geometry::SurfaceDomain domain() const = 0;
    virtual geometry::Point3D evaluate(double u, double v) const = 0;
    virtual geometry::Vector3D derivative(
        double u,
        double v,
        std::size_t order_u,
        std::size_t order_v) const = 0;

    geometry::Vector3D derivative_u(double u, double v) const;
    geometry::Vector3D derivative_v(double u, double v) const;
    geometry::Vector3D normal(double u, double v) const;

protected:
    void require_parameter(double u, double v) const;
};

} // namespace surface
} // namespace opencagd
