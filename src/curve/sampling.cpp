#include <opencagd/curve/sampling.h>

#include <stdexcept>

namespace opencagd {
namespace curve {

std::vector<geometry::Point3D> sample_curve(const ParametricCurve& curve, std::size_t count)
{
    if (count < 2) throw std::invalid_argument("Curve sampling requires at least two samples");
    const geometry::Interval d = curve.domain();
    std::vector<geometry::Point3D> points;
    points.reserve(count);

    for (std::size_t i = 0; i < count; ++i)
    {
        const double a = static_cast<double>(i) / static_cast<double>(count - 1);
        const double u = d.min + (d.max - d.min) * a;
        points.push_back(curve.evaluate(u));
    }
    return points;
}

} // namespace curve
} // namespace opencagd
