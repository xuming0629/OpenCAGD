#include <opencagd/geometry/domain.h>

#include <stdexcept>

namespace opencagd {
namespace geometry {

Interval::Interval() : min(0.0), max(1.0) {}
Interval::Interval(double min_value, double max_value) : min(min_value), max(max_value)
{
    if (max < min) throw std::invalid_argument("Interval max must be >= min");
}

double Interval::length() const { return max - min; }
bool Interval::contains(double value, double eps) const
{
    return value >= min - eps && value <= max + eps;
}

SurfaceDomain::SurfaceDomain() : u(), v() {}
SurfaceDomain::SurfaceDomain(const Interval& u_domain, const Interval& v_domain)
    : u(u_domain), v(v_domain) {}

bool SurfaceDomain::contains(double u_value, double v_value, double eps) const
{
    return u.contains(u_value, eps) && v.contains(v_value, eps);
}

} // namespace geometry
} // namespace opencagd
