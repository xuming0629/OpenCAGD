#include <opencagd/curve/knot_vector.h>
#include <opencagd/math/numeric.h>

#include <algorithm>
#include <stdexcept>

namespace opencagd {
namespace curve {

KnotVector::KnotVector() : degree_(0), knots_(2, 0.0)
{
    knots_[1] = 1.0;
}

KnotVector::KnotVector(std::size_t degree, const std::vector<double>& knots)
    : degree_(degree), knots_(knots)
{
    validate();
}

void KnotVector::validate() const
{
    if (knots_.size() < 2 * degree_ + 2)
        throw std::invalid_argument("Knot vector is too short for the requested degree");
    if (!std::is_sorted(knots_.begin(), knots_.end()))
        throw std::invalid_argument("Knot vector must be nondecreasing");
    if (domain_max() < domain_min())
        throw std::invalid_argument("Invalid knot-vector domain");
}

std::size_t KnotVector::degree() const { return degree_; }
std::size_t KnotVector::size() const { return knots_.size(); }
std::size_t KnotVector::num_basis_functions() const { return knots_.size() - degree_ - 1; }
const std::vector<double>& KnotVector::values() const { return knots_; }

double KnotVector::operator[](std::size_t index) const
{
    if (index >= knots_.size()) throw std::out_of_range("KnotVector index out of range");
    return knots_[index];
}

geometry::Interval KnotVector::domain() const
{
    return geometry::Interval(domain_min(), domain_max());
}

double KnotVector::domain_min() const { return knots_[degree_]; }
double KnotVector::domain_max() const { return knots_[knots_.size() - degree_ - 1]; }

std::size_t KnotVector::find_span(double u) const
{
    const std::size_t n = num_basis_functions() - 1;
    const double umin = domain_min();
    const double umax = domain_max();
    const double eps = math::tolerance();

    if (u < umin - eps || u > umax + eps)
        throw std::out_of_range("Parameter lies outside the B-spline domain");

    if (u >= umax - eps) return n;
    if (u <= umin + eps) return degree_;

    std::size_t low = degree_;
    std::size_t high = n + 1;
    std::size_t mid = (low + high) / 2;

    while (u < knots_[mid] || u >= knots_[mid + 1])
    {
        if (u < knots_[mid]) high = mid;
        else low = mid;
        mid = (low + high) / 2;
    }
    return mid;
}

} // namespace curve
} // namespace opencagd
