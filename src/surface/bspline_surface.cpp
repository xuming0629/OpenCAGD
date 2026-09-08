#include <opencagd/surface/bspline_surface.h>
#include <opencagd/curve/bspline_basis.h>

#include <stdexcept>

namespace opencagd {
namespace surface {

BSplineSurface::BSplineSurface(
    std::size_t degree_u,
    const std::vector<double>& knots_u,
    std::size_t degree_v,
    const std::vector<double>& knots_v,
    const ControlNet& control_points)
    : knot_u_(degree_u, knots_u),
      knot_v_(degree_v, knots_v),
      control_points_(control_points)
{
    validate();
}

void BSplineSurface::validate() const
{
    const std::size_t rows = knot_u_.num_basis_functions();
    const std::size_t cols = knot_v_.num_basis_functions();
    if (control_points_.size() != rows)
        throw std::invalid_argument("B-spline surface U control-point count mismatch");
    for (std::size_t i = 0; i < control_points_.size(); ++i)
        if (control_points_[i].size() != cols)
            throw std::invalid_argument("B-spline surface V control-point count mismatch");
}

std::size_t BSplineSurface::degree_u() const { return knot_u_.degree(); }
std::size_t BSplineSurface::degree_v() const { return knot_v_.degree(); }
const curve::KnotVector& BSplineSurface::knot_vector_u() const { return knot_u_; }
const curve::KnotVector& BSplineSurface::knot_vector_v() const { return knot_v_; }
const BSplineSurface::ControlNet& BSplineSurface::control_points() const { return control_points_; }

geometry::SurfaceDomain BSplineSurface::domain() const
{
    return geometry::SurfaceDomain(knot_u_.domain(), knot_v_.domain());
}

geometry::Point3D BSplineSurface::evaluate(double u, double v) const
{
    require_parameter(u, v);
    const std::size_t span_u = knot_u_.find_span(u);
    const std::size_t span_v = knot_v_.find_span(v);
    const std::vector<double> Nu = curve::basis_functions(knot_u_, span_u, u);
    const std::vector<double> Nv = curve::basis_functions(knot_v_, span_v, v);
    const std::size_t first_u = span_u - degree_u();
    const std::size_t first_v = span_v - degree_v();

    geometry::Vector3D result;
    for (std::size_t a = 0; a <= degree_u(); ++a)
        for (std::size_t b = 0; b <= degree_v(); ++b)
            result += control_points_[first_u + a][first_v + b].as_vector() * (Nu[a] * Nv[b]);

    return geometry::Point3D(result.x(), result.y(), result.z());
}

geometry::Vector3D BSplineSurface::derivative(
    double u,
    double v,
    std::size_t order_u,
    std::size_t order_v) const
{
    require_parameter(u, v);
    if (order_u == 0 && order_v == 0)
        throw std::invalid_argument("BSplineSurface::derivative requires a non-zero derivative order");
    if (order_u > degree_u() || order_v > degree_v())
        return geometry::Vector3D();

    const std::size_t span_u = knot_u_.find_span(u);
    const std::size_t span_v = knot_v_.find_span(v);
    const std::vector<std::vector<double> > Nu =
        curve::basis_function_derivatives(knot_u_, span_u, u, order_u);
    const std::vector<std::vector<double> > Nv =
        curve::basis_function_derivatives(knot_v_, span_v, v, order_v);
    const std::size_t first_u = span_u - degree_u();
    const std::size_t first_v = span_v - degree_v();

    geometry::Vector3D result;
    for (std::size_t a = 0; a <= degree_u(); ++a)
        for (std::size_t b = 0; b <= degree_v(); ++b)
            result += control_points_[first_u + a][first_v + b].as_vector() *
                      (Nu[order_u][a] * Nv[order_v][b]);
    return result;
}

} // namespace surface
} // namespace opencagd
