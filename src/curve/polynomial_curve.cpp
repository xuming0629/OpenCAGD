#include <opencagd/curve/polynomial_curve.h>

#include <stdexcept>

namespace opencagd {
namespace curve {

PolynomialCurve::PolynomialCurve(
    const std::vector<geometry::Point3D>& coefficients,
    const geometry::Interval& domain)
    : coefficients_(coefficients), domain_(domain)
{
    if (coefficients_.empty())
        throw std::invalid_argument("PolynomialCurve requires at least one coefficient");
}

std::size_t PolynomialCurve::degree() const { return coefficients_.size() - 1; }
const std::vector<geometry::Point3D>& PolynomialCurve::coefficients() const { return coefficients_; }
geometry::Interval PolynomialCurve::domain() const { return domain_; }

geometry::Point3D PolynomialCurve::evaluate(double t) const
{
    require_parameter(t);
    geometry::Vector3D value = coefficients_.back().as_vector();
    for (std::size_t i = coefficients_.size() - 1; i > 0; --i)
        value = value * t + coefficients_[i - 1].as_vector();
    return geometry::Point3D(value.x(), value.y(), value.z());
}

geometry::Vector3D PolynomialCurve::derivative(double t, std::size_t order) const
{
    require_parameter(t);
    if (order == 0)
        throw std::invalid_argument("PolynomialCurve::derivative order must be >= 1");
    if (order > degree()) return geometry::Vector3D();

    std::vector<geometry::Vector3D> coefficients;
    coefficients.reserve(coefficients_.size());
    for (std::size_t i = 0; i < coefficients_.size(); ++i)
        coefficients.push_back(coefficients_[i].as_vector());

    std::size_t current_degree = degree();
    for (std::size_t k = 0; k < order; ++k)
    {
        std::vector<geometry::Vector3D> next(current_degree);
        for (std::size_t i = 1; i <= current_degree; ++i)
            next[i - 1] = coefficients[i] * static_cast<double>(i);
        coefficients.swap(next);
        --current_degree;
    }

    geometry::Vector3D value = coefficients.back();
    for (std::size_t i = coefficients.size() - 1; i > 0; --i)
        value = value * t + coefficients[i - 1];
    return value;
}

} // namespace curve
} // namespace opencagd
