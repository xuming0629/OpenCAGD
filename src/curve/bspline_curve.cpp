#include <opencagd/curve/bspline_curve.h>
#include <opencagd/curve/bspline_basis.h>

#include <stdexcept>

namespace opencagd {
namespace curve {

BSplineCurve::BSplineCurve(std::size_t degree,
                           const std::vector<double>& knots,
                           const std::vector<geometry::Point3D>& control_points)
    : knot_vector_(degree, knots), control_points_(control_points)
{
    if (control_points_.size() != knot_vector_.num_basis_functions())
        throw std::invalid_argument(
            "B-spline control-point count must equal knot_count - degree - 1");
}

std::size_t BSplineCurve::degree() const { return knot_vector_.degree(); }
const KnotVector& BSplineCurve::knot_vector() const { return knot_vector_; }
const std::vector<geometry::Point3D>& BSplineCurve::control_points() const { return control_points_; }
geometry::Interval BSplineCurve::domain() const { return knot_vector_.domain(); }

geometry::Point3D BSplineCurve::evaluate(double u) const
{
    require_parameter(u);
    const std::size_t span = knot_vector_.find_span(u);
    const std::vector<double> basis = basis_functions(knot_vector_, span, u);
    const std::size_t first = span - degree();

    geometry::Vector3D value;
    for (std::size_t j = 0; j <= degree(); ++j)
        value += control_points_[first + j].as_vector() * basis[j];
    return geometry::Point3D(value.x(), value.y(), value.z());
}

geometry::Vector3D BSplineCurve::derivative(double u, std::size_t order) const
{
    require_parameter(u);
    if (order == 0)
        throw std::invalid_argument("BSplineCurve::derivative order must be >= 1");
    if (order > degree()) return geometry::Vector3D();

    const std::size_t span = knot_vector_.find_span(u);
    const std::vector<std::vector<double> > ders =
        basis_function_derivatives(knot_vector_, span, u, order);
    const std::size_t first = span - degree();

    geometry::Vector3D value;
    for (std::size_t j = 0; j <= degree(); ++j)
        value += control_points_[first + j].as_vector() * ders[order][j];
    return value;
}

} // namespace curve
} // namespace opencagd
