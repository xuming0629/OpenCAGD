#pragma once

#include <cstddef>
#include <vector>
#include <opencagd/core/export.h>
#include <opencagd/curve/knot_vector.h>
#include <opencagd/surface/parametric_surface.h>

namespace opencagd {
namespace surface {

class OPENCAGD_SURFACE_API BSplineSurface : public ParametricSurface
{
public:
    typedef std::vector<std::vector<geometry::Point3D> > ControlNet;

    BSplineSurface(
        std::size_t degree_u,
        const std::vector<double>& knots_u,
        std::size_t degree_v,
        const std::vector<double>& knots_v,
        const ControlNet& control_points);

    std::size_t degree_u() const;
    std::size_t degree_v() const;
    const curve::KnotVector& knot_vector_u() const;
    const curve::KnotVector& knot_vector_v() const;
    const ControlNet& control_points() const;

    geometry::SurfaceDomain domain() const override;
    geometry::Point3D evaluate(double u, double v) const override;
    geometry::Vector3D derivative(
        double u,
        double v,
        std::size_t order_u,
        std::size_t order_v) const override;

private:
    void validate() const;

    curve::KnotVector knot_u_;
    curve::KnotVector knot_v_;
    ControlNet control_points_;
};

} // namespace surface
} // namespace opencagd
