#include <opencagd/curve/bezier_curve.h>
#include <opencagd/curve/bernstein.h>
#include <opencagd/math/numeric.h>

#include <stdexcept>

namespace opencagd {
namespace curve {

namespace {
geometry::Point3D point_from_weighted_sum(const std::vector<geometry::Point3D>& points,
                                           const std::vector<double>& weights)
{
    geometry::Vector3D value;
    for (std::size_t i = 0; i < points.size(); ++i)
        value += points[i].as_vector() * weights[i];
    return geometry::Point3D(value.x(), value.y(), value.z());
}
}

BezierCurve::BezierCurve(const std::vector<geometry::Point3D>& control_points)
    : control_points_(control_points)
{
    if (control_points_.empty())
        throw std::invalid_argument("BezierCurve requires at least one control point");
}

std::size_t BezierCurve::degree() const { return control_points_.size() - 1; }
const std::vector<geometry::Point3D>& BezierCurve::control_points() const { return control_points_; }

geometry::Interval BezierCurve::domain() const { return geometry::Interval(0.0, 1.0); }

geometry::Point3D BezierCurve::evaluate(double t) const
{
    require_parameter(t);
    return point_from_weighted_sum(control_points_, bernstein_basis(degree(), t));
}

geometry::Point3D BezierCurve::evaluate_de_casteljau(double t) const
{
    const std::vector<std::vector<geometry::Point3D> > levels = construction_levels(t);
    return levels.back().front();
}

std::vector<std::vector<geometry::Point3D> > BezierCurve::construction_levels(double t) const
{
    require_parameter(t);
    std::vector<std::vector<geometry::Point3D> > levels;
    levels.push_back(control_points_);
    while (levels.back().size() > 1)
    {
        const std::vector<geometry::Point3D>& previous = levels.back();
        std::vector<geometry::Point3D> next;
        next.reserve(previous.size() - 1);
        for (std::size_t i = 0; i + 1 < previous.size(); ++i)
            next.push_back(geometry::lerp(previous[i], previous[i + 1], t));
        levels.push_back(next);
    }
    return levels;
}

geometry::Vector3D BezierCurve::derivative(double t, std::size_t order) const
{
    require_parameter(t);
    if (order == 0)
        throw std::invalid_argument("BezierCurve::derivative order must be >= 1");
    if (order > degree()) return geometry::Vector3D();

    std::vector<geometry::Vector3D> work;
    work.reserve(control_points_.size());
    for (std::size_t i = 0; i < control_points_.size(); ++i)
        work.push_back(control_points_[i].as_vector());

    std::size_t current_degree = degree();
    for (std::size_t k = 0; k < order; ++k)
    {
        std::vector<geometry::Vector3D> next;
        next.reserve(current_degree);
        for (std::size_t i = 0; i < current_degree; ++i)
            next.push_back((work[i + 1] - work[i]) * static_cast<double>(current_degree));
        work.swap(next);
        --current_degree;
    }

    const std::vector<double> basis = bernstein_basis(current_degree, t);
    geometry::Vector3D result;
    for (std::size_t i = 0; i < work.size(); ++i)
        result += work[i] * basis[i];
    return result;
}

BezierCurve BezierCurve::derivative_curve() const
{
    if (degree() == 0)
        return BezierCurve(std::vector<geometry::Point3D>(1, geometry::Point3D()));

    std::vector<geometry::Point3D> points;
    points.reserve(degree());
    const double n = static_cast<double>(degree());
    for (std::size_t i = 0; i < degree(); ++i)
    {
        const geometry::Vector3D q = (control_points_[i + 1] - control_points_[i]) * n;
        points.push_back(geometry::Point3D(q.x(), q.y(), q.z()));
    }
    return BezierCurve(points);
}

std::pair<BezierCurve, BezierCurve> BezierCurve::subdivide(double t) const
{
    require_parameter(t);
    const std::size_t n = degree();
    std::vector<geometry::Point3D> work = control_points_;
    std::vector<geometry::Point3D> left(n + 1);
    std::vector<geometry::Point3D> right(n + 1);
    left[0] = work[0];
    right[n] = work[n];

    for (std::size_t r = 1; r <= n; ++r)
    {
        for (std::size_t i = 0; i <= n - r; ++i)
            work[i] = geometry::lerp(work[i], work[i + 1], t);
        left[r] = work[0];
        right[n - r] = work[n - r];
    }
    return std::make_pair(BezierCurve(left), BezierCurve(right));
}

} // namespace curve
} // namespace opencagd
