#include <opencagd/surface/nurbs_surface.h>
#include <opencagd/curve/bspline_basis.h>
#include <opencagd/math/numeric.h>

#include <cmath>
#include <stdexcept>

namespace opencagd {
namespace surface {

NURBSSurface::NURBSSurface(
    std::size_t degree_u,
    const std::vector<double>& knots_u,
    std::size_t degree_v,
    const std::vector<double>& knots_v,
    const ControlNet& control_points,
    const WeightNet& weights)
    : knot_u_(degree_u, knots_u),
      knot_v_(degree_v, knots_v),
      control_points_(control_points),
      weights_(weights)
{
    validate();
}

void NURBSSurface::validate() const
{
    const std::size_t rows = knot_u_.num_basis_functions();
    const std::size_t cols = knot_v_.num_basis_functions();
    if (control_points_.size() != rows || weights_.size() != rows)
        throw std::invalid_argument("NURBS surface U control/weight count mismatch");

    for (std::size_t i = 0; i < rows; ++i)
    {
        if (control_points_[i].size() != cols || weights_[i].size() != cols)
            throw std::invalid_argument("NURBS surface V control/weight count mismatch");
        for (std::size_t j = 0; j < cols; ++j)
            if (weights_[i][j] <= 0.0)
                throw std::invalid_argument("NURBS surface weights must be positive");
    }
}

std::size_t NURBSSurface::degree_u() const { return knot_u_.degree(); }
std::size_t NURBSSurface::degree_v() const { return knot_v_.degree(); }
const curve::KnotVector& NURBSSurface::knot_vector_u() const { return knot_u_; }
const curve::KnotVector& NURBSSurface::knot_vector_v() const { return knot_v_; }
const NURBSSurface::ControlNet& NURBSSurface::control_points() const { return control_points_; }
const NURBSSurface::WeightNet& NURBSSurface::weights() const { return weights_; }

geometry::SurfaceDomain NURBSSurface::domain() const
{
    return geometry::SurfaceDomain(knot_u_.domain(), knot_v_.domain());
}

geometry::Point3D NURBSSurface::evaluate(double u, double v) const
{
    require_parameter(u, v);
    const std::size_t span_u = knot_u_.find_span(u);
    const std::size_t span_v = knot_v_.find_span(v);
    const std::vector<double> Nu = curve::basis_functions(knot_u_, span_u, u);
    const std::vector<double> Nv = curve::basis_functions(knot_v_, span_v, v);
    const std::size_t first_u = span_u - degree_u();
    const std::size_t first_v = span_v - degree_v();

    geometry::Vector3D numerator;
    double denominator = 0.0;

    for (std::size_t a = 0; a <= degree_u(); ++a)
    {
        for (std::size_t b = 0; b <= degree_v(); ++b)
        {
            const std::size_t iu = first_u + a;
            const std::size_t iv = first_v + b;
            const double factor = Nu[a] * Nv[b] * weights_[iu][iv];
            numerator += control_points_[iu][iv].as_vector() * factor;
            denominator += factor;
        }
    }

    if (std::abs(denominator) <= math::tolerance())
        throw std::runtime_error("NURBS surface denominator is numerically zero");

    const geometry::Vector3D value = numerator / denominator;
    return geometry::Point3D(value.x(), value.y(), value.z());
}

geometry::Vector3D NURBSSurface::derivative(
    double u,
    double v,
    std::size_t order_u,
    std::size_t order_v) const
{
    require_parameter(u, v);
    if (order_u == 0 && order_v == 0)
        throw std::invalid_argument("NURBSSurface::derivative requires a non-zero derivative order");

    const std::size_t span_u = knot_u_.find_span(u);
    const std::size_t span_v = knot_v_.find_span(v);
    const std::vector<std::vector<double> > Nu =
        curve::basis_function_derivatives(knot_u_, span_u, u, order_u);
    const std::vector<std::vector<double> > Nv =
        curve::basis_function_derivatives(knot_v_, span_v, v, order_v);
    const std::size_t first_u = span_u - degree_u();
    const std::size_t first_v = span_v - degree_v();

    std::vector<std::vector<geometry::Vector3D> > A(
        order_u + 1,
        std::vector<geometry::Vector3D>(order_v + 1, geometry::Vector3D()));
    std::vector<std::vector<double> > W(
        order_u + 1,
        std::vector<double>(order_v + 1, 0.0));

    for (std::size_t ku = 0; ku <= order_u; ++ku)
    {
        for (std::size_t kv = 0; kv <= order_v; ++kv)
        {
            for (std::size_t a = 0; a <= degree_u(); ++a)
            {
                for (std::size_t b = 0; b <= degree_v(); ++b)
                {
                    const std::size_t iu = first_u + a;
                    const std::size_t iv = first_v + b;
                    const double factor = Nu[ku][a] * Nv[kv][b] * weights_[iu][iv];
                    A[ku][kv] += control_points_[iu][iv].as_vector() * factor;
                    W[ku][kv] += factor;
                }
            }
        }
    }

    if (std::abs(W[0][0]) <= math::tolerance())
        throw std::runtime_error("NURBS surface derivative denominator is numerically zero");

    std::vector<std::vector<geometry::Vector3D> > S(
        order_u + 1,
        std::vector<geometry::Vector3D>(order_v + 1, geometry::Vector3D()));

    for (std::size_t ku = 0; ku <= order_u; ++ku)
    {
        for (std::size_t kv = 0; kv <= order_v; ++kv)
        {
            geometry::Vector3D value = A[ku][kv];
            for (std::size_t i = 0; i <= ku; ++i)
            {
                for (std::size_t j = 0; j <= kv; ++j)
                {
                    if (i == 0 && j == 0) continue;
                    const double coefficient =
                        static_cast<double>(math::binomial(ku, i)) *
                        static_cast<double>(math::binomial(kv, j)) *
                        W[i][j];
                    value -= S[ku - i][kv - j] * coefficient;
                }
            }
            S[ku][kv] = value / W[0][0];
        }
    }

    return S[order_u][order_v];
}

} // namespace surface
} // namespace opencagd
