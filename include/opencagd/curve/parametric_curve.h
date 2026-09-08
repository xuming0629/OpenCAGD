#pragma once

#include <cstddef>
#include <opencagd/core/export.h>
#include <opencagd/geometry/domain.h>
#include <opencagd/geometry/point3d.h>
#include <opencagd/geometry/vector3d.h>

namespace opencagd {
namespace curve {

class OPENCAGD_CURVE_API ParametricCurve
{
public:
    virtual ~ParametricCurve();

    virtual geometry::Interval domain() const = 0;
    virtual geometry::Point3D evaluate(double u) const = 0;
    virtual geometry::Vector3D derivative(double u, std::size_t order = 1) const = 0;

    double speed(double u) const;
    geometry::Vector3D tangent(double u) const;
    double curvature(double u) const;

protected:
    void require_parameter(double u) const;
};

} // namespace curve
} // namespace opencagd
