#include <opencagd/surface/bezier_surface.h>
#include <opencagd/curve/bernstein.h>

#include <stdexcept>

namespace opencagd {
namespace surface {

BezierSurface::BezierSurface(const ControlNet& control_points)
    : control_points_(control_points)
{
    validate();
}

void BezierSurface::validate() const
{
    if (control_points_.empty() || control_points_[0].empty())
        throw std::invalid_argument("BezierSurface control net must not be empty");
    const std::size_t columns = control_points_[0].size();
    for (std::size_t i = 0; i < control_points_.size(); ++i)
        if (control_points_[i].size() != columns)
            throw std::invalid_argument("BezierSurface control net must be rectangular");
}

std::size_t BezierSurface::degree_u() const { return control_points_.size() - 1; }
std::size_t BezierSurface::degree_v() const { return control_points_[0].size() - 1; }
const BezierSurface::ControlNet& BezierSurface::control_points() const { return control_points_; }

geometry::SurfaceDomain BezierSurface::domain() const
{
    return geometry::SurfaceDomain(geometry::Interval(0.0, 1.0), geometry::Interval(0.0, 1.0));
}

geometry::Point3D BezierSurface::evaluate(double u, double v) const
{
    require_parameter(u, v);
    const std::vector<double> Bu = curve::bernstein_basis(degree_u(), u);
    const std::vector<double> Bv = curve::bernstein_basis(degree_v(), v);
    geometry::Vector3D result;

    for (std::size_t i = 0; i <= degree_u(); ++i)
        for (std::size_t j = 0; j <= degree_v(); ++j)
            result += control_points_[i][j].as_vector() * (Bu[i] * Bv[j]);

    return geometry::Point3D(result.x(), result.y(), result.z());
}

geometry::Vector3D BezierSurface::derivative(
    double u,
    double v,
    std::size_t order_u,
    std::size_t order_v) const
{
    require_parameter(u, v);
    if (order_u == 0 && order_v == 0)
        throw std::invalid_argument("BezierSurface::derivative requires a non-zero derivative order");
    if (order_u > degree_u() || order_v > degree_v())
        return geometry::Vector3D();

    std::vector<std::vector<geometry::Vector3D> > net(
        control_points_.size(),
        std::vector<geometry::Vector3D>(control_points_[0].size()));

    for (std::size_t i = 0; i < control_points_.size(); ++i)
        for (std::size_t j = 0; j < control_points_[i].size(); ++j)
            net[i][j] = control_points_[i][j].as_vector();

    std::size_t du = degree_u();
    std::size_t dv = degree_v();

    for (std::size_t k = 0; k < order_u; ++k)
    {
        std::vector<std::vector<geometry::Vector3D> > next(
            du,
            std::vector<geometry::Vector3D>(dv + 1));
        for (std::size_t i = 0; i < du; ++i)
            for (std::size_t j = 0; j <= dv; ++j)
                next[i][j] = (net[i + 1][j] - net[i][j]) * static_cast<double>(du);
        net.swap(next);
        --du;
    }

    for (std::size_t k = 0; k < order_v; ++k)
    {
        std::vector<std::vector<geometry::Vector3D> > next(
            du + 1,
            std::vector<geometry::Vector3D>(dv));
        for (std::size_t i = 0; i <= du; ++i)
            for (std::size_t j = 0; j < dv; ++j)
                next[i][j] = (net[i][j + 1] - net[i][j]) * static_cast<double>(dv);
        net.swap(next);
        --dv;
    }

    const std::vector<double> Bu = curve::bernstein_basis(du, u);
    const std::vector<double> Bv = curve::bernstein_basis(dv, v);
    geometry::Vector3D result;
    for (std::size_t i = 0; i <= du; ++i)
        for (std::size_t j = 0; j <= dv; ++j)
            result += net[i][j] * (Bu[i] * Bv[j]);
    return result;
}

} // namespace surface
} // namespace opencagd
