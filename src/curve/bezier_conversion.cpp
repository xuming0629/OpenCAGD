#include <opencagd/curve/bezier_conversion.h>
#include <opencagd/math/numeric.h>

namespace opencagd {
namespace curve {

PolynomialCurve to_power_basis(const BezierCurve& curve)
{
    const std::size_t n = curve.degree();
    std::vector<geometry::Vector3D> coefficient_vectors(n + 1, geometry::Vector3D());
    const std::vector<geometry::Point3D>& control = curve.control_points();

    for (std::size_t i = 0; i <= n; ++i)
    {
        for (std::size_t j = 0; j <= n - i; ++j)
        {
            const std::size_t k = i + j;
            const double sign = (j % 2 == 0) ? 1.0 : -1.0;
            const double factor =
                static_cast<double>(math::binomial(n, i)) *
                static_cast<double>(math::binomial(n - i, j)) * sign;
            coefficient_vectors[k] += control[i].as_vector() * factor;
        }
    }

    std::vector<geometry::Point3D> coefficients;
    coefficients.reserve(coefficient_vectors.size());
    for (std::size_t i = 0; i < coefficient_vectors.size(); ++i)
        coefficients.push_back(geometry::Point3D(
            coefficient_vectors[i].x(), coefficient_vectors[i].y(), coefficient_vectors[i].z()));
    return PolynomialCurve(coefficients, curve.domain());
}

} // namespace curve
} // namespace opencagd
