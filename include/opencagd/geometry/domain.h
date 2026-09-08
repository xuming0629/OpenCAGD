#pragma once

#include <opencagd/core/export.h>

namespace opencagd {
namespace geometry {

struct OPENCAGD_CORE_API Interval
{
    double min;
    double max;

    Interval();
    Interval(double min_value, double max_value);

    double length() const;
    bool contains(double value, double eps = 0.0) const;
};

struct OPENCAGD_CORE_API SurfaceDomain
{
    Interval u;
    Interval v;

    SurfaceDomain();
    SurfaceDomain(const Interval& u_domain, const Interval& v_domain);
    bool contains(double u_value, double v_value, double eps = 0.0) const;
};

} // namespace geometry
} // namespace opencagd
