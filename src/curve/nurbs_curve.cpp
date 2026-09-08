#include <opencagd/curve/nurbs_curve.h>
#include <opencagd/curve/bspline_basis.h>
#include <opencagd/math/numeric.h>

#include <cmath>
#include <stdexcept>

namespace opencagd {
namespace curve {

NURBSCurve::NURBSCurve(std::size_t degree,
                       const std::vector<double>& knots,
                       const std::vector<geometry::Point3D>& control_points,
                       const std::vector<double>& weights)
    : knot_vector_(degree, knots), control_points_(control_points), weights_(weights)
{
    validate();
}

void NURBSCurve::validate() const
{
    if (control_points_.size() != knot_vector_.num_basis_functions())
        throw std::invalid_argument(
            "NURBS control-point count must equal knot_count - degree - 1");
    if (weights_.size() != control_points_.size())
        throw std::invalid_argument("NURBS requires one weight per control point");
    for (std::size_t i = 0; i < weights_.size(); ++i)
        if (weights_[i] <= 0.0)
            throw std::invalid_argument("NURBS weights must be positive");
}

std::size_t NURBSCurve::degree() const { return knot_vector_.degree(); }
const KnotVector& NURBSCurve::knot_vector() const { return knot_vector_; }
const std::vector<geometry::Point3D>& NURBSCurve::control_points() const { return control_points_; }
const std::vector<double>& NURBSCurve::weights() const { return weights_; }
geometry::Interval NURBSCurve::domain() const { return knot_vector_.domain(); }

std::vector<double> NURBSCurve::rational_basis_functions(double u) const
{
    require_parameter(u);
    const std::vector<double> nonrational = all_basis_functions(knot_vector_, u);
    std::vector<double> rational(nonrational.size(), 0.0);
    double denominator = 0.0;
    for (std::size_t i = 0; i < nonrational.size(); ++i)
        denominator += nonrational[i] * weights_[i];
    if (std::abs(denominator) <= math::tolerance())
        throw std::runtime_error("NURBS rational basis denominator is zero");
    for (std::size_t i = 0; i < nonrational.size(); ++i)
        rational[i] = nonrational[i] * weights_[i] / denominator;
    return rational;
}

geometry::Point3D NURBSCurve::evaluate(double u) const
{
    require_parameter(u);
    const std::size_t span = knot_vector_.find_span(u);
    const std::vector<double> basis = basis_functions(knot_vector_, span, u);
    const std::size_t first = span - degree();

    geometry::Vector3D numerator;
    double denominator = 0.0;
    for (std::size_t j = 0; j <= degree(); ++j)
    {
        const std::size_t index = first + j;
        const double factor = basis[j] * weights_[index];
        numerator += control_points_[index].as_vector() * factor;
        denominator += factor;
    }
    if (std::abs(denominator) <= math::tolerance())
        throw std::runtime_error("NURBS curve denominator is numerically zero");
    const geometry::Vector3D value = numerator / denominator;
    return geometry::Point3D(value.x(), value.y(), value.z());
}

geometry::Vector3D NURBSCurve::derivative(double u, std::size_t order) const
{
    require_parameter(u);
    if (order == 0)
        throw std::invalid_argument("NURBSCurve::derivative order must be >= 1");

    const std::size_t span = knot_vector_.find_span(u);
    const std::vector<std::vector<double> > ders =
        basis_function_derivatives(knot_vector_, span, u, order);
    const std::size_t first = span - degree();

    std::vector<geometry::Vector3D> A(order + 1, geometry::Vector3D());
    std::vector<double> W(order + 1, 0.0);

    for (std::size_t k = 0; k <= order; ++k)
    {
        for (std::size_t j = 0; j <= degree(); ++j)
        {
            const std::size_t index = first + j;
            const double factor = ders[k][j] * weights_[index];
            A[k] += control_points_[index].as_vector() * factor;
            W[k] += factor;
        }
    }

    if (std::abs(W[0]) <= math::tolerance())
        throw std::runtime_error("NURBS curve derivative denominator is numerically zero");

    std::vector<geometry::Vector3D> C(order + 1, geometry::Vector3D());
    C[0] = A[0] / W[0];

    for (std::size_t k = 1; k <= order; ++k)
    {
        geometry::Vector3D value = A[k];
        for (std::size_t i = 1; i <= k; ++i)
        {
            const double coefficient =
                static_cast<double>(math::binomial(k, i)) * W[i];
            value -= C[k - i] * coefficient;
        }
        C[k] = value / W[0];
    }

    return C[order];
}

} // namespace curve
} // namespace opencagd
