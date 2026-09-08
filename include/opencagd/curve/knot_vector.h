#pragma once

#include <cstddef>
#include <vector>
#include <opencagd/core/export.h>
#include <opencagd/geometry/domain.h>

namespace opencagd {
namespace curve {

class OPENCAGD_CURVE_API KnotVector
{
public:
    KnotVector();
    KnotVector(std::size_t degree, const std::vector<double>& knots);

    std::size_t degree() const;
    std::size_t size() const;
    std::size_t num_basis_functions() const;
    const std::vector<double>& values() const;
    double operator[](std::size_t index) const;

    geometry::Interval domain() const;
    double domain_min() const;
    double domain_max() const;
    std::size_t find_span(double u) const;

private:
    void validate() const;

    std::size_t degree_;
    std::vector<double> knots_;
};

} // namespace curve
} // namespace opencagd
